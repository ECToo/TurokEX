// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 Samuel Villarreal
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION: Main Client Code
//
//-----------------------------------------------------------------------------

#include "enet/enet.h"
#include "common.h"
#include "packet.h"
#include "client.h"
#include "system.h"
#include "render.h"
#include "gl.h"
#include "js.h"
#include "sound.h"
#include "fx.h"
#include "zone.h"
#include "debug.h"
#include "network.h"
#include "console.h"

static int clientFPS;

kexCvar cvarClientName("cl_name", CVF_STRING|CVF_CONFIG, "Player", "Name for client player");
kexCvar cvarClientFOV("cl_fov", CVF_FLOAT|CVF_CONFIG, "74.0", "Client's field of view");
kexCvar cvarPaused("cl_paused", CVF_BOOL|CVF_CHEAT|CVF_CONFIG, "0", "Pause the game");
kexCvar cvarMSensitivityX("cl_msensitivityx", CVF_FLOAT|CVF_CONFIG, "5", "Mouse-X sensitivity");
kexCvar cvarMSensitivityY("cl_msensitivityy", CVF_FLOAT|CVF_CONFIG, "5", "Mouse-Y sensitivity");
kexCvar cvarMAcceleration("cl_macceleration", CVF_FLOAT|CVF_CONFIG, "0", "Mouse acceleration");
kexCvar cvarInvertLook("cl_mlookinvert", CVF_BOOL|CVF_CONFIG, "0", "Invert mouse-look");
kexCvar cvarClientPort("cl_port", CVF_INT|CVF_CONFIG, "58304", "Client's forwarding port");
kexCvar cvarClientFPS("cl_maxfps", CVF_INT|CVF_CONFIG, "60", 1, 60, "Game render FPS");

kexClient client;

//
// kexClient::Destroy
//

void kexClient::Destroy(void) {
    if(GetHost()) {
        if(GetPeer() != NULL) {
            enet_peer_disconnect(GetPeer(), 0);
            enet_peer_reset(GetPeer());
        }

        DestroyHost();
        SetState(CL_STATE_DISCONNECTED);
    }
}

//
// kexClient::Shutdown
//

void kexClient::Shutdown(void) {
    Destroy();
}

//
// kexClient::Connect
//

void kexClient::Connect(const char *address) {
    ENetAddress addr;
    char ip[32];

    enet_address_set_host(&addr, address);
    addr.port = cvarClientPort.GetInt();
    SetPeer(enet_host_connect(GetHost(), &addr, 2, 0));
    player->info.peer = GetPeer();
    enet_address_get_host_ip(&addr, ip, 32);
    common.Printf("Connecting to %s:%u...\n", ip, addr.port);

    if(GetPeer() == NULL) {
        common.Warning("No available peers for initiating an ENet connection.\n");
        return;
    }

    SetState(CL_STATE_CONNECTING);
}

//
// kexClient::ProcessPackets
//

void kexClient::ProcessPackets(const ENetPacket *packet) {
    unsigned int type = 0;

    packetManager.Read8((ENetPacket*)packet, &type);

    switch(type)
    {
    case sp_ping:
        common.Printf("Recieved acknowledgement from server\n");
        break;

    case sp_clientinfo:
        packetManager.Read8((ENetPacket*)packet, &id);
        player->info.id = id;
        SetState(CL_STATE_READY);
        common.DPrintf("CL_ReadClientInfo: ID is %i\n", id);
        break;

    default:
        common.Warning("Recieved unknown packet type: %i\n", type);
        break;
    }

    DestroyPacket();
}

//
// kexClient::OnConnect
//

void kexClient::OnConnect(void) {
    SetState(CL_STATE_CONNECTED);
    P_ResetNetSeq(&client.player->info);
}

//
// kexClient::OnDisconnect
//

void kexClient::OnDisconnect(void) {
    SetState(CL_STATE_DISCONNECTED);
}

//
// kexClient::Run
//

void kexClient::Run(const int msec) {

    curtime += msec;

    if(curtime < (1000 / cvarClientFPS.GetInt()))
        return;

    SetRunTime((float)curtime / 1000.0f);
    SetTime(GetTime() + curtime);

    clientFPS = (int)(1000.0f / (GetRunTime() * 1000.0f));

    curtime = 0;

    CheckMessages();

    inputSystem.PollInput();

    ProcessEvents();

    P_BuildCommands();

    P_LocalPlayerTick();

    console.Tick();

    FX_Ticker();

    //TEMP
    G_ClientThink();

    R_DrawFrame();

    Debug_DrawStats();

    console.Draw();

    R_FinishFrame();

    Snd_UpdateListener();

    Debug_UpdateStatFrame();

    UpdateTicks();
}

//
// kexClient::MessageServer
//

void kexClient::MessageServer(char *string) {
    ENetPacket *packet;

    if(!(packet = packetManager.Create()))
        return;

    packetManager.Write8(packet, cp_msgserver);
    packetManager.WriteString(packet, string);
    packetManager.Send(packet, GetPeer());
}

//
// kexClient::CreateHost
//

void kexClient::CreateHost(void) {
    SetHost(enet_host_create(NULL, 1, 2, 0, 0));

    if(!GetHost()) {
        common.Error("kexClient::Init: failed to create client");
    }
}

//
// kexClient::PostEvent
// Called by the I/O functions when input is detected
//

void kexClient::PostEvent(event_t *ev) {
    events[eventhead] = *ev;
    eventhead = (++eventhead)&(MAXEVENTS-1);
}

//
// kexClient::GetEvent
//

event_t *kexClient::GetEvent(void) {
    event_t *ev;

    if(eventtail == eventhead)
        return NULL;

    ev = &events[eventtail];
    eventtail = (++eventtail)&(MAXEVENTS-1);

    return ev;
}

//
// kexClient::ProcessEvents
// Send all the events of the given timestamp down the responder chain
//

void kexClient::ProcessEvents(void) {
    event_t *ev;
    event_t *oldev = NULL; // TEMP
    
    while((ev = GetEvent()) != NULL) {
        if(ev == oldev)
            return;

        if(console.ProcessInput(ev))
            continue;

        // TODO - TEMP
        eventtail = (--eventtail)&(MAXEVENTS-1);
        P_Responder(GetEvent());

        oldev = ev;
    }
}

//
// FCmd_Ping
//

static void FCmd_Ping(void) {
    ENetPacket *packet;

    if(!(packet = packetManager.Create()))
        return;

    packetManager.Write8(packet, cp_ping);
    packetManager.Send(packet, client.GetPeer());
}

//
// FCmd_Say
//

static void FCmd_Say(void) {
    ENetPacket *packet;

    if(command.GetArgc() < 2)
        return;

    if(!(packet = packetManager.Create()))
        return;

    packetManager.Write8(packet, cp_say);
    packetManager.WriteString(packet, command.GetArgv(1));
    packetManager.Send(packet, client.GetPeer());
}

//
// FCmd_MsgServer
//

static void FCmd_MsgServer(void) {
    if(command.GetArgc() < 2)
        return;

    client.MessageServer(command.GetArgv(1));
}

//
// kexClient::Init
//

void kexClient::Init(void) {
    Destroy();
    CreateHost();

    curtime = 0;
    id = -1;
    bLocal = (common.CheckParam("-client") == 0);
    playerActor = NULL;
    player = &localPlayer;
    
    SetTime(0);
    SetTicks(0);
    SetPeer(NULL);
    SetState(CL_STATE_UNINITIALIZED);

    command.Add("ping", FCmd_Ping);
    command.Add("say", FCmd_Say);
    command.Add("msgserver", FCmd_MsgServer);

    common.Printf("Client Initialized\n");

    /*
    Debug_RegisterPerfStatVar((float*)&clientFPS, "FPS", false);
    Debug_RegisterPerfStatVar(&client.runtime, "Runtime", true);
    Debug_RegisterPerfStatVar((float*)&client.time, "Client Time", false);
    Debug_RegisterPerfStatVar((float*)&client.tics, "Client Ticks", false);
    */
}
