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

#ifdef _WIN32
#include <windows.h>
#include <commctrl.h>
#include <rpcdce.h>
#include <io.h>
#include "resource.h"
#endif

#include <stdarg.h>
#include "types.h"
#include "common.h"
#include "pak.h"
#include "zone.h"

void TX_StoreTextures(void);
void TX_StoreFonts(void);
void SND_StoreSounds(void);
void SND_StoreSoundShaders(void);
void MDL_StoreModels(void);
void LV_StoreLevels(void);
void FX_StoreParticleEffects(void);

#define MAX_ARGS 256
char	*ArgBuffer[MAX_ARGS+1];
int		myargc = 0;
char**	myargv;

#ifdef _WIN32
HINSTANCE	hAppInst	= NULL;
HWND		hwnd		= NULL;
HWND		hwndWait	= NULL;
HWND		hwndLoadBar	= NULL;
HWND        hwndDataBar = NULL;
#endif

#ifdef _WIN32

#define TOTALSTEPS	5174

dboolean __stdcall LoadingDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        hwndLoadBar = GetDlgItem(hWnd, IDC_PROGRESS1);
        hwndDataBar = GetDlgItem(hWnd, IDC_PROGRESS2);
        SendMessage(hwndLoadBar, PBM_SETRANGE, 0, MAKELPARAM(0, TOTALSTEPS));
        SendMessage(hwndLoadBar, PBM_SETSTEP, (WPARAM)(int)1, 0);
        SendMessage(hwndDataBar, PBM_SETRANGE, 0, MAKELPARAM(0, 1));
        SendMessage(hwndDataBar, PBM_SETSTEP, (WPARAM)(int)1, 0);
        return true;
    case WM_PAINT:
        return true;
    case WM_DESTROY:
        return true;
    case WM_CLOSE:
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        }
        break;
    }
    return false;
}

void StoreExternalFile(const char *name, const char *store)
{
    byte *data;
    int len;

    data = NULL;

    len = Com_ReadBinaryFile(va("%s/content/%s", Com_GetExePath(), name), &data);
    if(len <= 0)
    {
        Com_Error("Couldn't find file: %s\n", name);
        return;
    }

    PK_AddFile(store, data, len, true);
    Com_Free(&data);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    char *p;
    char *start;
    char **arg;
    
    hAppInst = hInstance;
    
    arg = ArgBuffer;
    p = lpCmdLine;
    myargc = 1;
    *(arg++) = "KPFGen";
    
    while(*p && (myargc < MAX_ARGS))
    {
        while(*p && isspace(*p)) p++;
        if(!*p) break;

        start = p;
        while(*p && !isspace(*p)) p++;

        *arg = (char *)Com_Alloc(p - start + 1);
        (*arg)[p - start] = 0;
        strncpy(*arg, start, p - start);

        arg++;
        myargc++;
    }

    *arg = NULL;
    myargv = ArgBuffer;
    
    InitCommonControls();
    PK_Init();
    Z_Init();

    hwndWait = CreateDialogA(hAppInst, MAKEINTRESOURCEA(IDD_DIALOG1), NULL, (DLGPROC)LoadingDlgProc);

    PK_AddFolder("scripts/");
    PK_AddFolder("defs/");
    TX_StoreFonts();
    SND_StoreSounds();
    Com_GetCartFile(
        "PC Turok Cart File (.dat) \0*.dat\0All Files (*.*)\0*.*\0",
        "Locate CARTFILE.DAT");
    SND_StoreSoundShaders();
    TX_StoreTextures();
    FX_StoreParticleEffects();
    MDL_StoreModels();
    LV_StoreLevels();
    Com_CloseCartFile();
    PK_WriteFile(va("%s/game.kpf", Com_GetExePath()));
    
    return 0;
}
#else
#error TODO: support main in *unix

int main(int argc, char* argv[])
{
    myargc = argc;
    myargv = argv;
    
    return 0;
}

#endif

