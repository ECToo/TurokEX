// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2007-2012 Samuel Villarreal
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

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "enet/enet.h"
#include "kernel.h"

typedef enum
{
    BT_ATTACK       = 1,
    BT_JUMP         = 2,
    BT_NEXTWEAP     = 4,
    BT_PREVWEAP     = 8,
    BT_CENTER       = 16,
    BT_FORWARD      = 32,
    BT_BACKWARD     = 64,
    BT_STRAFELEFT   = 128,
    BT_STRAFERIGHT  = 256
} buttoncode2_t;

#define CL_TICDIFF_TURN1        (1 << 0)
#define CL_TICDIFF_TURN2        (1 << 1)
#define CL_TICDIFF_BUTTONS      (1 << 2)

typedef struct
{
    short   angle[2];
    short   buttons;
    byte    msec;
} ticcmd_t;

#define ANGLETOSHORT(x) ((short)((x * 65536.0f) * 0.000030517578125f))
#define SHORTTOANGLE(x) ((float)((x / 65536.0f) / 0.000030517578125f))

#define MAXEVENTS 64

extern event_t  events[MAXEVENTS];
extern int      eventhead;
extern int      eventtail;

typedef enum
{
    CL_STATE_UNINITIALIZED,
    CL_STATE_CONNECTING,
    CL_STATE_CONNECTED,
    CL_STATE_DISCONNECTED,
    CL_STATE_READY
} client_state_e;

#include "player.h"

typedef struct
{
    ENetHost        *host;
    client_state_e  state;
    ENetPeer        *peer;
    int             client_id;
    int             time;
    float           runtime;
    kbool           local;
    int             tics;
    ticcmd_t        cmd;
    player_t        *localplayer;
} client_t;

extern client_t client;

kbool CL_Responder(event_t *ev);
void CL_WriteTiccmd(ENetPacket *packet, ticcmd_t *cmd);
void CL_BuildTiccmd(void);
void CL_PostEvent(event_t *ev);
void CL_ProcessEvents(void);
kbool CL_Responder(event_t *ev);
int CL_Random(void);
void CL_Connect(const char *address);
void CL_Run(int msec);
void CL_Init(void);

#endif