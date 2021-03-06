// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 2014 Samuel Villarreal
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

#ifndef __VIEWBOUNDS_H__
#define __VIEWBOUNDS_H__

class kexCamera;

class kexViewBounds {
public:
                        kexViewBounds(void);

    void                Clear(void);
    const bool          IsClosed(void) const;
    void                Fill(void);
    void                AddPoint(const float x, const float y, const float z);
    void                AddVector(kexCamera *camera, kexVec3 &vector);
    void                AddBox(kexCamera *camera, kexBBox &box);
    bool                ViewBoundInside(const kexViewBounds &viewBounds);
    void                DebugDraw(void);

    kexViewBounds       &operator=(const kexViewBounds &viewBounds);
    
    float               *Min(void) { return min; }
    float               *Max(void) { return max; }
    const float         ZFar(void) const { return zfar; }
    
    void                AddX(const float x) { if(x < min[0]) min[0] = x; if(x > max[0]) max[0] = x; }
    void                AddY(const float y) { if(y < min[1]) min[1] = y; if(y > max[1]) max[1] = y; }
    
private:
    float               min[2];
    float               max[2];
    float               zmin;
    float               zfar;
};

#endif
