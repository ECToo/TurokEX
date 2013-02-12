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
// DESCRIPTION: Common Stuff
//
//-----------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>

#include "common.h"
#include "zone.h"
#include "kernel.h"
#include "js.h"

CVAR_EXTERNAL(kf_basepath);
CVAR_EXTERNAL(developer);

int     myargc;
char**  myargv;

static char com_buffer[4096];

//
// Com_Printf
//

void Com_Printf(char *string, ...)
{
    va_list	va;
    char *src;
    
    va_start(va, string);
    vsprintf(com_buffer, string, va);
    va_end(va);

#ifdef _WIN32
    Sys_Printf(com_buffer);
#endif

    Con_Printf(COLOR_WHITE, com_buffer);
    printf(com_buffer);

    src = (char*)com_buffer;
    J_CallClassFunction(JS_EV_SYS, "event_OutputText", &src, 1);
}

//
// Com_CPrintf
//

void Com_CPrintf(rcolor color, char *string, ...)
{
    va_list	va;
    char *src[2];
    
    va_start(va, string);
    vsprintf(com_buffer, string, va);
    va_end(va);

#ifdef _WIN32
    Sys_Printf(com_buffer);
#endif

    Con_Printf(color, com_buffer);
    printf(com_buffer);

    src[0] = (char*)com_buffer;
    src[1] = kva("%i,%i,%i",
        color & 0xff,
        (color >> 8) & 0xff,
        (color >> 16) & 0xff);

    J_CallClassFunction(JS_EV_SYS, "event_OutputText", src, 2);
}

//
// Com_Warning
//

void Com_Warning(char *string, ...)
{
    va_list	va;
    char *src[2];
    
    va_start(va, string);
    vsprintf(com_buffer, string, va);
    va_end(va);

#ifdef _WIN32
    Sys_Printf(com_buffer);
#endif

    Con_Printf(COLOR_YELLOW, com_buffer);
    printf(com_buffer);

    src[0] = (char*)com_buffer;
    src[1] = "255,255,0";

    J_CallClassFunction(JS_EV_SYS, "event_OutputText", src, 2);
}

//
// Com_DPrintf
//

void Com_DPrintf(char *string, ...)
{
    if(developer.value)
    {
        static char com_buffer[1024];
        va_list	va;
        
        va_start(va, string);
        vsprintf(com_buffer, string, va);
        va_end(va);

        Com_CPrintf(RGBA(0xE0, 0xE0, 0xE0, 0xff), com_buffer);
    }
}

//
// Com_Error
//

void Com_Error(char* string, ...)
{
    va_list	va;

    va_start(va, string);
    vsprintf(com_buffer, string, va);
    va_end(va);
    
    fprintf(stderr, "Error - %s\n", com_buffer);
    fflush(stderr);

#ifdef _WIN32
    Sys_Error(com_buffer);
#endif

    printf(com_buffer);

    exit(0);    // just in case...
}

//
// Com_CheckParam
//

int Com_CheckParam(char *check)
{
    int i;
    
    for(i = 1; i < myargc; i++)
    {
        if(!stricmp(check, myargv[i]))
        {
            return i;
        }
    }
    
    return 0;
}

//
// Com_Shutdown
//

void Com_Shutdown(void)
{
    Z_FreeTags(PU_STATIC, PU_STATIC);
}

//
// Com_ReadConfigFile
//

void Com_ReadConfigFile(const char *file)
{
    char *buffer;
    int len;

    len = KF_ReadTextFile(file, &buffer);

    if(len == -1)
    {
        Com_Warning("Warning: %s not found\n", file);
        return;
    }

    Cmd_ExecuteCommand(buffer);
    Z_Free(buffer);
}

//
// Com_WriteConfigFile
//

void Com_WriteConfigFile(void)
{
    FILE *f;

    f = fopen(kva("%s\\config.cfg", kf_basepath.string), "w");
    if(f)
    {
        Key_WriteBindings(f);
        Cvar_WriteToFile(f);
        fclose(f);
    }
}

//
// Com_HashFileName
//

unsigned int Com_HashFileName(const char *name)
{
    unsigned int hash   = 1315423911;
    unsigned int i      = 0;
    char *str           = (char*)name;

    for(i = 0; i < strlen(name)-1 && *str != '\0'; str++, i++)
    {
        hash ^= ((hash << 5) + toupper((int)*str) + (hash >> 2));
    }

    return hash & (MAX_HASH-1);
}

//
// Com_NormalizeSlashes
//
// Remove trailing slashes, translate backslashes to slashes
// The string to normalize is passed and returned in str
//
// killough 11/98: rewritten
//

void Com_NormalizeSlashes(char *str)
{
    char *p;
   
    // Convert all slashes/backslashes to DIR_SEPARATOR
    for(p = str; *p; p++)
    {
        if((*p == '/' || *p == '\\') && *p != DIR_SEPARATOR)
        {
            *p = DIR_SEPARATOR;
        }
    }

    // Collapse multiple slashes
    for(p = str; (*str++ = *p); )
    {
        if(*p++ == DIR_SEPARATOR)
        {
            while(*p == DIR_SEPARATOR) p++;
        }
    }
}

//
// Com_StripPath
//

void Com_StripPath(char *name)
{
    char *search;
    int len = 0;
    int pos = 0;
    int i = 0;

    len = strlen(name) - 1;
    pos = len + 1;

    for(search = name + len;
        *search != ASCII_BACKSLASH && *search != ASCII_SLASH; search--, pos--)
    {
        if(search == name)
        {
            return;
        }
    }

    if(pos <= 0)
    {
        return;
    }

    for(i = 0; pos < len+1; pos++, i++)
    {
        name[i] = name[pos];
    }

    name[i] = '\0';
}

//
// Com_StripExt
//

void Com_StripExt(char *name)
{
    char *search;

    search = name + strlen(name) - 1;

    while(*search != ASCII_BACKSLASH && *search != ASCII_SLASH
        && search != name)
    {
        if(*search == '.')
        {
            *search = '\0';
            return;
        }

        search--;
    }
}

//
// Com_SubStr
//

char *Com_SubStr(const char *from, unsigned int idx1, unsigned int idx2)
{
    return Z_Strndup(from+idx1, idx2, PU_STATIC, NULL);
}

//
// fcmp
//

kbool fcmp(float f1, float f2)
{
    float precision = 0.00001f;
    if(((f1 - precision) < f2) && 
        ((f1 + precision) > f2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//
// kva
//

char *kva(char *str, ...)
{
    va_list v;
    static char vastr[1024];
	
    va_start(v, str);
    vsprintf(vastr, str,v);
    va_end(v);
    
    return vastr;	
}

