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
// DESCRIPTION: Collision detection
//
//-----------------------------------------------------------------------------

#include "common.h"
#include "mathlib.h"
#include "game.h"
#include "actor.h"
#include "level.h"

#define TRYMOVE_COUNT           3
#define ONESIDED_FLOOR_DISTMAX  1.024f
#define EPSILON_FLOOR           0.975f

typedef enum
{
    TRT_NOHIT       = 0,
    TRT_SLOPE       = 1,
    TRT_STEEPSLOPE  = 2,
    TRT_OUTEREDGE   = 3,
    TRT_OBJECT      = 4
} tracetype_e;

typedef struct
{
    vec3_t      start;
    vec3_t      end;
    plane_t     *pl;
    plane_t     *hitpl;
    vec3_t      normal;
    vec3_t      hit;
    float       frac;
    float       sidedist;
    tracetype_e type;
} trace_t;

//
// G_FindClosestPlane
//

plane_t *G_FindClosestPlane(vec3_t coord)
{
    unsigned int i;
    float dist;
    float curdist;
    plane_t *plane;
    kbool ok;

    // VERY TEMP

    ok = false;
    curdist = 0;
    plane = NULL;

    for(i = 0; i < g_currentmap->numplanes; i++)
    {
        plane_t *p;

        p = &g_currentmap->planes[i];

        if(Plane_PointInRange(p, coord[0], coord[2]))
        {
            dist = coord[1] - Plane_GetDistance(p, coord);

            if(p->flags & CLF_ONESIDED && dist < -16)
            {
                continue;
            }

            if(dist < 0)
            {
                dist = -dist;
            }

            if(ok)
            {
                if(dist < curdist)
                {
                    curdist = dist;
                    plane = p;
                }
            }
            else
            {
                plane = p;
                curdist = dist;
                ok = true;
            }
        }
    }

    return plane;
}

//
// G_ClipVelocity
//

static void G_ClipVelocity(vec3_t out, vec3_t velocity, vec3_t normal, float fudge)
{
    float d;
    vec3_t n;

    d = Vec_Dot(velocity, normal) * fudge;
    Vec_Scale(n, normal, d);
    Vec_Sub(out, velocity, n);
    d = Vec_Unit3(out);

    if(d != 0)
    {
        Vec_Scale(out, out, Vec_Unit3(velocity) / d);
    }
}

//
// G_SlideOnCrease
//

static void G_SlideOnCrease(vec3_t out, vec3_t velocity, vec3_t v1, vec3_t v2)
{
    vec3_t dir;

    Vec_Cross(dir, v1, v2);
    Vec_Normalize3(dir);
    Vec_Scale(out, dir, Vec_Dot(velocity, dir));
}

//
// G_TraceObject
//
// Line-circle intersection test on an object
//

static kbool G_TraceObject(trace_t *trace, vec3_t objpos, float radius)
{
    vec3_t dir;
    float x;
    float z;
    float vd;
    float d;

    Vec_Sub(dir, trace->end, trace->start);
    vd = Vec_Unit3(dir);
    Vec_Normalize3(dir);

    // validate position
    x = objpos[0] - trace->start[0];
    z = objpos[2] - trace->start[2];
    d = dir[0] * x + dir[2] * z;

    if(d >= 0 && vd != 0)
    {
        float dx;
        float dz;
        float ld;
        float len;

        vd = 1.0f / vd;

        dx = vd * dir[0];
        dz = vd * dir[2];
        ld = dx * x + dz * z;

        x = x - ld * dx;
        z = z - ld * dz;

        len = radius * radius - (x * x + z * z);

        // is the ray inside the radius?
        if(len > 0)
        {
            vec3_t end;
            vec3_t lerp;
            vec3_t n;
            vec3_t vec;
            float f;

            f = ld - (float)sqrt(len) * vd;

            // setup normal to clip the raytrace with
            Vec_Add(end, trace->start, dir);
            Vec_Lerp3(lerp, f, trace->start, end);
            Vec_Set3(n,
                lerp[0] - objpos[0],
                0,
                lerp[2] - objpos[2]);

            Vec_Sub(dir, trace->end, trace->start);
            d = Vec_Dot(n, dir);

            if(d != 0)
            {
                Vec_Sub(vec, lerp, trace->start);
                d = Vec_Dot(n, vec) / d;

                trace->frac = d;

                Vec_Normalize3(n);
                Vec_Copy3(trace->normal, n);
                return true;
            }
        }
    }

    return false;
}

//
// G_CheckObjects
//
// Scans through planes for linked objects and test
// collision against them
//

static kbool G_CheckObjects(trace_t *trace, plane_t *plane)
{
    blockobj_t *obj;
    sector_t *sector;

    if(plane == NULL)
    {
        return false;
    }

    // TODO: this doesn't seem like the right way to do this...
    sector = &g_currentmap->sectors[plane - g_currentmap->planes];

    // go through the list
    for(obj = sector->blocklist.next; obj != &sector->blocklist; obj = obj->next)
    {
        if(G_TraceObject(trace, obj->object->origin, obj->object->width))
        {
            // check object height
            // TODO: the original game doesn't do height clipping but I'd like
            // to do it for kex someday
            if(trace->end[1] > obj->object->origin[1] + obj->object->height)
            {
                continue;
            }

            trace->type = TRT_OBJECT;
            return true;
        }
    }

    return false;
}

//
// G_TracePlane
//
// Check if the ray intersected with plane
//

static kbool G_TracePlane(trace_t *trace, plane_t *pl)
{
    float d;
    float dstart;
    float dend;

    // special cases for those annoying one-sided planes
    if(pl->flags & CLF_ONESIDED && !Plane_IsAWall(pl))
    {
        if(trace->start[1] -
            Plane_GetDistance(pl, trace->start) < -ONESIDED_FLOOR_DISTMAX)
        {
            return false;
        }
    }
    else if(!Plane_IsAWall(pl))
    {
        // ignore if the plane isn't steep enough
        if(pl->normal[1] >= EPSILON_FLOOR)
        {
            return false;
        }
    }

    d       = Vec_Dot(pl->points[0], pl->normal);
    dstart  = Vec_Dot(trace->start, pl->normal) - d;
    dend    = Vec_Dot(trace->end, pl->normal) - d;

    if(dstart > 0 && dend >= dstart)
    {
        // in front of the plane
        return false;
    }

    if(dend <= 0 && dend < dstart)
    {
        // intersected
        trace->type = Plane_IsAWall(pl) ? TRT_STEEPSLOPE : TRT_SLOPE;
        trace->frac = (dstart - 0.03125f) / (dstart - dend);
        trace->hitpl = pl;

        Vec_Copy3(trace->normal, pl->normal);
        return true;
    }

    return false;
}

//
// G_CheckEdgeSide
//
// Simple line-line intersection test to see if
// ray has crossed the plane's edge
//

static kbool G_CheckEdgeSide(trace_t *trace, vec3_t vp1, vec3_t vp2)
{
    float x;
    float z;
    float dx;
    float dz;
    float d;

    x = vp1[0] - vp2[0];
    z = vp2[2] - vp1[2];

    d = z * (trace->end[0] - trace->start[0]) +
        x * (trace->end[2] - trace->start[2]);

    if(d < 0)
    {
        dx = vp1[0] - trace->end[0];
        dz = vp1[2] - trace->end[2];

        d = (dz * x + dx * z) / d;

        if(d < trace->sidedist)
        {
            trace->sidedist = d;
            return true;
        }
    }

    return false;
}

//
// G_TraceEdge
//
// Sets up a normal vector for an edge that isn't linked
// into another plane. Basically treat it as a solid wall
//

static void G_TraceEdge(trace_t *trace, vec3_t vp1, vec3_t vp2)
{
    float x;
    float z;

    x = vp1[0] - vp2[0];
    z = vp2[2] - vp1[2];

    Vec_Set3(trace->normal, z, 0, x);
    Vec_Normalize3(trace->normal);

    trace->type = TRT_OUTEREDGE;
}

//
// G_PathTraverse
//

void G_PathTraverse(plane_t *plane, trace_t *trace)
{
    int i;
    int point;
    plane_t *pl;

    if(plane == NULL)
    {
        return;
    }

    // we've entered a new plane
    trace->pl = plane;

    if(G_CheckObjects(trace, plane))
    {
        // an object was hit
        return;
    }

    pl = NULL;
    point = 0;
    trace->sidedist = 0;

    // check if ray crosses into an edge. if multiple edges
    // have been crossed then get the one closest to the ray
    for(i = 0; i < 3; i++)
    {
        vec3_t vp1;
        vec3_t vp2;

        Vec_Copy3(vp1, plane->points[i]);
        Vec_Copy3(vp2, plane->points[(i + 1) % 3]);

        if(G_CheckObjects(trace, plane->link[i]))
        {
            // an object was hit
            return;
        }

        if(G_CheckEdgeSide(trace, vp1, vp2))
        {
            pl = plane->link[i];
            point = i;

            trace->frac = trace->sidedist;

            if(pl == NULL)
            {
                // a null edge was crossed. treat it as
                // a solid wall
                G_TraceEdge(trace, vp1, vp2);
                return;
            }
        }
    }

    if(pl)
    {
        // check to see if the ray can bump into it
        if(Plane_IsAWall(pl))
        {
            float y = trace->start[1] + 1.024f;

            // must not be over it
            if( y <= pl->points[0][1]   ||
                y <= pl->points[1][1]   ||
                y <= pl->points[2][1])
            {
                // trace it to see if its valid or not
                if(G_TracePlane(trace, pl))
                {
                    return;
                }
            }
        }

        trace->type = TRT_NOHIT;

        // traverse into next plane
        G_PathTraverse(pl, trace);
    }
}

//
// G_Trace
//
//

trace_t G_Trace(vec3_t start, vec3_t end, plane_t *plane)
{
    trace_t trace;

    Vec_Copy3(trace.start, start);
    Vec_Copy3(trace.end, end);
    Vec_Set3(trace.normal, 0, 0, 0);

    trace.pl        = plane;
    trace.hitpl     = NULL;
    trace.frac      = 1;
    trace.sidedist  = 0;
    trace.type      = TRT_NOHIT;

    // try to trace something as early as possible
    if(!G_TracePlane(&trace, trace.pl))
    {
        int i;

        // look for edges in the initial plane that can be collided with
        for(i = 0; i < 3; i++)
        {
            vec3_t vp1;
            vec3_t vp2;

            if(plane->link[i] != NULL)
            {
                continue;
            }

            Vec_Copy3(vp1, plane->points[i]);
            Vec_Copy3(vp2, plane->points[(i + 1) % 3]);

            // check to see if an edge was crossed
            if(G_CheckEdgeSide(&trace, vp1, vp2))
            {
                G_TraceEdge(&trace, vp1, vp2);
                break;
            }
        }

        // start traversing into other planes if we couldn't trace anything
        if(trace.type == TRT_NOHIT)
        {
            G_PathTraverse(plane, &trace);
        }
    }

    Vec_Lerp3(trace.hit, trace.frac, start, end);

    return trace;
}

//
// G_GroundMove
//
// Trace against surrounding planes and slide
// against it if needed, clipping velocity
// along the way
//

void G_GroundMove(actor_t *actor)
{
    if(actor->plane == NULL ||
        (actor->plane &&
        !Plane_PointInRange(actor->plane,
        actor->origin[0], actor->origin[2])))
    {
        // if player is in the void or current plane isn't
        // in range then scan through all planes and find
        // the cloest one to the actor
        actor->plane = G_FindClosestPlane(actor->origin);
    }

    if(actor->plane != NULL)
    {
        trace_t trace;
        vec3_t start;
        vec3_t end;
        vec3_t vel;
        vec3_t normals[TRYMOVE_COUNT];
        int moves;
        int i;

        // set start point
        Vec_Copy3(start, actor->origin);
        Vec_Copy3(vel, actor->velocity);
        moves = 0;

        for(i = 0; i < TRYMOVE_COUNT; i++)
        {
            float tmpy;

            // set end point
            Vec_Add(end, start, vel);

            // get trace results
            trace = G_Trace(start, end, actor->plane);

            actor->plane = trace.pl;

            if(trace.type == TRT_NOHIT)
            {
                // went the entire distance
                break;
            }

            // slide along the crease between an edge and a steep slope
            if(trace.type == TRT_OUTEREDGE && Plane_IsAWall(actor->plane))
            {
                G_SlideOnCrease(actor->velocity, vel,
                    trace.normal, actor->plane->normal);
                break;
            }
            // slide along the crease between two normals
            else if(i > 0)
            {
                if(Vec_Dot(trace.normal, normals[i-1]) > 0.99f)
                {
                    // fudge velocity if this plane was already collided with
                    Vec_Add(vel, vel, trace.normal);
                }
                else if(Vec_Dot(trace.normal, vel) < 0)
                {
                    G_SlideOnCrease(actor->velocity, vel,
                        trace.normal, normals[i-1]);

                    break;
                }
            }

            // slide against the hit surface. ignore Y-velocity if on a steep slope
            tmpy = vel[1];
            G_ClipVelocity(vel, vel, trace.normal, 1.01f);
            if(trace.type == TRT_STEEPSLOPE && vel[1] > tmpy)
            {
                vel[1] = tmpy;
            }

            // handle vertical sliding if on a steep slope
            if(Plane_IsAWall(actor->plane) &&
                (actor->origin[1] -
                Plane_GetDistance(actor->plane, actor->origin)) <= 0)
            {
                vec3_t push;
                vec3_t n;

                Vec_Copy3(n, actor->plane->normal);

                // negate y-normal so its facing downward
                n[1] = -n[1];

                // scale the normal by the magnitude of velocity and
                // the steepness of the slope
                Vec_Scale(push, n, Vec_Unit3(vel) * actor->plane->normal[1]);

                // apply to velocity
                Vec_Add(vel, vel, push);
            }

            // force a deadstop if clipped velocity is against
            // the original velocity or if exceeded max amount of
            // attempted moves (don't count against objects hit)
            if(Vec_Dot(vel, actor->velocity) <= 0 ||
                (trace.type != TRT_OBJECT && i == (TRYMOVE_COUNT - 1)))
            {
                actor->velocity[0] = 0;
                actor->velocity[2] = 0;
                break;
            }

            // update velocity and try another move
            Vec_Copy3(actor->velocity, vel);
            Vec_Copy3(normals[moves++], trace.normal);
        }
    }
}