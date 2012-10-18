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
// DESCRIPTION: Plane operations
//
//-----------------------------------------------------------------------------

#include <math.h>
#include "common.h"
#include "mathlib.h"
#include "zone.h"

//
// Plane_SetTemp
//

void Plane_SetTemp(plane_t *plane, vec3_t p1, vec3_t p2, vec3_t p3)
{
    memcpy(&plane->points[0], p1, sizeof(vec3_t));
    memcpy(&plane->points[1], p2, sizeof(vec3_t));
    memcpy(&plane->points[2], p3, sizeof(vec3_t));

    plane->link[0] = plane->link[1]= plane->link[2] = NULL;
    plane->flags = 0;
}

//
// Plane_GetNormal
//

void Plane_GetNormal(vec3_t normal, plane_t *plane)
{
    vec3_t vp1;
    vec3_t vp2;
    vec3_t vn;

    Vec_Sub(vp1, plane->points[1], plane->points[0]);
    Vec_Sub(vp2, plane->points[2], plane->points[1]);
    Vec_Cross(vn, vp1, vp2);

    normal[0] = vn[0];
    normal[1] = vn[1];
    normal[2] = vn[2];
}

//
// Plane_IsFacing
//

kbool Plane_IsFacing(plane_t *plane, float angle)
{
    float s;
    float c;
    vec3_t n;

    s = (float)sin(angle);
    c = (float)cos(angle);

    Plane_GetNormal(n, plane);

    return -s * n[0] + -c * n[2] < 0.0f;
}

//
// Plane_GetDistance
//

float Plane_GetDistance(plane_t *plane, vec3_t pos)
{
    vec3_t normal;
    float dist;
    
    if(plane)
    {
        Plane_GetNormal(normal, plane);

        if(normal[1] == 0.0f)
        {
            dist = (
                plane->points[0][1] + 
                plane->points[1][1] +
                plane->points[2][1]) * 0.3333333432674408f;
        }
        else
        {
            vec3_t vec;

            Vec_Set3(vec,
                plane->points[0][0] - pos[0],
                plane->points[0][1],
                plane->points[0][2] - pos[2]);

            dist = Vec_Dot(vec, normal) / normal[1];
        }
        
        return dist;
    }

    return -(float)D_MAXINT;
}

//
// Plane_GetHeight
//

float Plane_GetHeight(plane_t *plane, vec3_t pos)
{
    vec3_t normal;
    float dist;
    
    if(plane)
    {
        Plane_GetNormal(normal, plane);

        if(Plane_IsAWall(plane))
        {
            dist = (
                plane->height[0] +
                plane->height[1] +
                plane->height[2]) * 0.3333333432674408f;
        }
        else
        {
            vec3_t vec;

            Vec_Set3(vec,
                plane->points[0][0] - pos[0],
                plane->height[0],
                plane->points[0][2] - pos[2]);

            dist = Vec_Dot(vec, normal) / normal[1];
        }
        
        return dist;
    }

    return 0.0f;
}

//
// Plane_IsAWall
//

kbool Plane_IsAWall(plane_t *plane)
{
    vec3_t normal;

    if(plane == NULL)
    {
        return false;
    }

    Plane_GetNormal(normal, plane);
    Vec_Normalize3(normal);

    return (normal[1] <= 0.5f);
}

//
// Plane_GetYaw
//

float Plane_GetYaw(plane_t *p, int point)
{
    float x;
    float z;
    float d;

    x = p->points[(point + 1) % 3][0] - p->points[point][0];
    z = p->points[(point + 1) % 3][2] - p->points[point][2];
    d = (float)sqrt(x * x + z * z);

    if(d != 0.0f)
    {
        float an = (float)acos(x / d);

        if(z >= 0.0f)
        {
            an = -an;
        }

        return an;
    }

    return 0.0f;
}

//
// Plane_GetPitch
//

float Plane_GetPitch(plane_t *p)
{
    if(Plane_IsAWall(p))
    {
        vec3_t t1;
        vec3_t n;
        float an;

        Plane_GetNormal(n, p);
        Vec_Set3(t1, 0, 1.0f, 0);
        Vec_Normalize3(n);

        an = (float)acos(t1[0] * n[0] + t1[1] * n[1] + t1[2] * n[2]);

        return an;
    }

    return 0.0f;
}

//
// Plane_GetSlope
//

float Plane_GetSlope(plane_t *plane, float x1, float z1, float x2, float z2)
{
    float dist1;
    float dist2;
    float d;
    float xz;
    vec3_t v1;
    vec3_t v2;

    Vec_Set3(v1, x1, 0, z1);
    Vec_Set3(v2, x2, 0, z2);

    dist1 = Plane_GetDistance(plane, v1);
    dist2 = Plane_GetDistance(plane, v2);

    xz = (x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1);
    d = (dist2 - dist1) * (dist2 - dist1) + xz;

    if(d != 0.0f)
    {
        float an = (float)sqrt(xz / d);

        if(an >  1.0f) an =  1.0f;
        if(an < -1.0f) an = -1.0f;

        if(dist2 <= dist1)
        {
            return -(float)acos(an);
        }

        return (float)acos(an);
    }

    return 0.0f;
}

//
// Plane_GetRotation
//

void Plane_GetRotation(vec4_t vec, plane_t *p)
{
    vec3_t n1;
    vec3_t n2;
    vec3_t cp;
    float an;
    float s;
    float c;

    Plane_GetNormal(n1, p);
    Vec_Normalize3(n1);
    Vec_Set3(n2, 0, 1, 0);
    Vec_Cross(cp, n2, n1);
    Vec_Normalize3(cp);

    an = (float)acos(n2[0] * n1[0] + n2[1] * n1[1] + n2[2] * n1[2]) * 0.5f;
    s = (float)sin(an);
    c = (float)cos(an);

    vec[0] = cp[0] * s;
    vec[1] = cp[1] * s;
    vec[2] = cp[2] * s;
    vec[3] = c;
}

//
// Plane_AdjustRotation
//

void Plane_AdjustRotation(vec4_t out, plane_t *p)
{
    vec3_t n1;
    vec3_t n2;
    vec3_t cp;
    float d;

    Plane_GetNormal(n1, p);
    Vec_Normalize3(n1);
    Vec_Set3(n2, 0, 1, 0);
    Vec_Cross(cp, n2, n1);

    d = (float)sqrt(cp[0] * cp[0] + cp[1] * cp[1] + cp[2] * cp[2]);

    if(d == 0.0f)
    {
        Vec_Set4(out, 0, 0, 0, 1);
    }
    else
    {
        float an;
        float s;
        float c;

        Vec_Scale(cp, cp, 1.0f / d);

        an = (float)acos(n2[0] * n1[0] + n2[1] * n1[1] + n2[2] * n1[2]) * 0.5f;
        s = (float)sin(an);
        c = (float)cos(an);

        out[0] = cp[0] * s;
        out[1] = cp[1] * s;
        out[2] = cp[2] * s;
        out[3] = c;
    }
}

//
// Plane_PointInRange
//

kbool Plane_PointInRange(plane_t *p, float x, float z)
{
    kbool s1;
    kbool s2;
    kbool s3;

#define M_POINTONSIDE(v1, v2, v3, x, z) \
    ((x - v2[0])        *   \
    (v3[2] - v2[2])     -   \
    (v3[0] - v2[0])     *   \
    (z - v2[2]))        *   \
    ((v1[0] - v2[0])    *   \
    (v3[2] - v2[2])     -   \
    (v3[0] - v2[0])     *   \
    (v1[2] - v2[2]))

    s1 = M_POINTONSIDE(p->points[0], p->points[1], p->points[2], x, z) >= 0;
    s2 = M_POINTONSIDE(p->points[1], p->points[0], p->points[2], x, z) >= 0;
    s3 = M_POINTONSIDE(p->points[2], p->points[0], p->points[1], x, z) >= 0;

#undef M_POINTONSIDE

    return (s1 && s2 && s3);
}
