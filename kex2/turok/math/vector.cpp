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
// DESCRIPTION: Vector operations
//
//-----------------------------------------------------------------------------

#include <math.h>
#ifndef EDITOR
#include "common.h"
#endif
#include "mathlib.h"

const kexVec3 kexVec3::vecRight(1, 0, 0);
const kexVec3 kexVec3::vecUp(0, 1, 0);
const kexVec3 kexVec3::vecForward(0, 0, 1);

//
// kexVec2::kexVec2
//

kexVec2::kexVec2(void) {
    Clear();
}

//
// kexVec2::kexVec2
//

kexVec2::kexVec2(const float x, const float z) {
    Set(x, z);
}

//
// kexVec2::Set
//

void kexVec2::Set(const float x, const float z) {
    this->x = x;
    this->z = z;
}

//
// kexVec2::Clear
//

void kexVec2::Clear(void) {
    x = z = 0.0f;
}

//
// kexVec2::Dot
//

float kexVec2::Dot(const kexVec2 &vec) const {
    return (x * vec.x + z * vec.z);
}

//
// kexVec2::Dot
//

float kexVec2::Dot(const kexVec2 &vec1, const kexVec2 &vec2) {
    return (vec1.x * vec2.x + vec1.z * vec2.z);
}

//
// kexVec2::Dot
//

float kexVec2::Dot(const kexVec3 &vec) const {
    return (x * vec.x + z * vec.z);
}

//
// kexVec2::Dot
//

float kexVec2::Dot(const kexVec3 &vec1, const kexVec3 &vec2) {
    return (vec1.x * vec2.x + vec1.z * vec2.z);
}

//
// kexVec2::Cross
//

kexVec2 kexVec2::Cross(const kexVec2 &vec) const {
    return kexVec2(
        vec.z - z,
        x - vec.x
    );
}

//
// kexVec2::Cross
//

kexVec2 &kexVec2::Cross(const kexVec2 &vec1, const kexVec2 &vec2) {
    x = vec2.z - vec1.z;
    z = vec1.x - vec2.x;

    return *this;
}

//
// kexVec2::Cross
//

kexVec2 kexVec2::Cross(const kexVec3 &vec) const {
    return kexVec2(
        vec.z - z,
        x - vec.x
    );
}

//
// kexVec2::Cross
//

kexVec2 &kexVec2::Cross(const kexVec3 &vec1, const kexVec3 &vec2) {
    x = vec2.z - vec1.z;
    z = vec1.x - vec2.x;

    return *this;
}

//
// kexVec2::UnitSq
//

float kexVec2::UnitSq(void) const {
    return x * x + z * z;
}

//
// kexVec2::Unit
//

float kexVec2::Unit(void) const {
    return kexMath::Sqrt(UnitSq());
}

//
// kexVec2::DistanceSq
//

float kexVec2::DistanceSq(const kexVec2 &vec) const {
    return (
        (x - vec.x) * (x - vec.x) +
        (z - vec.z) * (z - vec.z)
    );
}

//
// kexVec2::Distance
//

float kexVec2::Distance(const kexVec2 &vec) const {
    return kexMath::Sqrt(DistanceSq(vec));
}

//
// kexVec2::Normalize
//

kexVec2 &kexVec2::Normalize(void) {
    float d = Unit();
    if(d != 0.0f) {
        d = 1.0f / d;
        *this *= d;
    }
    return *this;
}

//
// kexVec2::Lerp
//

kexVec2 kexVec2::Lerp(const kexVec2 &next, float movement) const {
    return (next - *this) * movement + *this;
}

//
// kexVec2::Lerp
//

kexVec2 &kexVec2::Lerp(const kexVec2 &start, const kexVec2 &next, float movement) {
    *this = (next - start) * movement + start;
    return *this;
}

//
// kexVec2::ToString
//

kexStr kexVec2::ToString(void) const {
    kexStr str;
    str = str + x + " " + z;
    return str;
}

//
// kexVec2::ToFloatPtr
//

float *kexVec2::ToFloatPtr(void) {
    return reinterpret_cast<float*>(&x);
}

//
// kexVec2::ToVec3
//

kexVec3 kexVec2::ToVec3(void) {
    return kexVec3(x, 0, z);
}

//
// kexVec2::operator+
//

kexVec2 kexVec2::operator+(const kexVec2 &vec) {
    return kexVec2(x + vec.x, z + vec.z);
}

//
// kexVec2::operator+
//

kexVec2 kexVec2::operator+(const kexVec2 &vec) const {
    return kexVec2(x + vec.x, z + vec.z);
}

//
// kexVec2::operator+
//

kexVec2 kexVec2::operator+(kexVec2 &vec) {
    return kexVec2(x + vec.x, z + vec.z);
}

//
// kexVec2::operator+=
//

kexVec2 &kexVec2::operator+=(const kexVec2 &vec) {
    x += vec.x;
    z += vec.z;
    return *this;
}

//
// kexVec2::operator-
//

kexVec2 kexVec2::operator-(const kexVec2 &vec) const {
    return kexVec2(x - vec.x, z - vec.z);
}

//
// kexVec2::operator-
//

kexVec2 kexVec2::operator-(void) const {
    return kexVec2(-x, -z);
}

//
// kexVec2::operator-=
//

kexVec2 &kexVec2::operator-=(const kexVec2 &vec) {
    x -= vec.x;
    z -= vec.z;
    return *this;
}

//
// kexVec2::operator*
//

kexVec2 kexVec2::operator*(const kexVec2 &vec) {
    return kexVec2(x * vec.x, z * vec.z);
}

//
// kexVec2::operator*=
//

kexVec2 &kexVec2::operator*=(const kexVec2 &vec) {
    x *= vec.x;
    z *= vec.z;
    return *this;
}

//
// kexVec2::operator*
//

kexVec2 kexVec2::operator*(const float val) {
    return kexVec2(x * val, z * val);
}

//
// kexVec2::operator*
//

kexVec2 kexVec2::operator*(const float val) const {
    return kexVec2(x * val, z * val);
}

//
// kexVec2::operator*=
//

kexVec2 &kexVec2::operator*=(const float val) {
    x *= val;
    z *= val;
    return *this;
}

//
// kexVec2::operator/
//

kexVec2 kexVec2::operator/(const kexVec2 &vec) {
    return kexVec2(x / vec.x, z / vec.z);
}

//
// kexVec2::operator/=
//

kexVec2 &kexVec2::operator/=(const kexVec2 &vec) {
    x /= vec.x;
    z /= vec.z;
    return *this;
}

//
// kexVec2::operator/
//

kexVec2 kexVec2::operator/(const float val) {
    return kexVec2(x / val, z / val);
}

//
// kexVec2::operator/=
//

kexVec2 &kexVec2::operator/=(const float val) {
    x /= val;
    z /= val;
    return *this;
}

//
// kexVec2::operator=
//

kexVec2 &kexVec2::operator=(const kexVec2 &vec) {
    x = vec.x;
    z = vec.z;
    return *this;
}

//
// kexVec2::operator=
//

kexVec2 &kexVec2::operator=(const kexVec3 &vec) {
    x = vec.x;
    z = vec.z;
    return *this;
}

//
// kexVec2::operator=
//

kexVec2 &kexVec2::operator=(kexVec3 &vec) {
    x = vec.x;
    z = vec.z;
    return *this;
}

//
// kexVec2::operator=
//

kexVec2 &kexVec2::operator=(const float *vecs) {
    x = vecs[0];
    z = vecs[2];
    return *this;
}

//
// kexVec2::operator[]
//

float kexVec2::operator[](int index) const {
    assert(index >= 0 && index < 2);
    return (&x)[index];
}

//
// kexVec2::operator[]
//

float &kexVec2::operator[](int index) {
    assert(index >= 0 && index < 2);
    return (&x)[index];
}

//
// kexVec3::kexVec3
//

kexVec3::kexVec3(void) {
    Clear();
}

//
// kexVec3::kexVec3
//

kexVec3::kexVec3(const float x, const float y, const float z) {
    Set(x, y, z);
}

//
// kexVec3::Set
//

void kexVec3::Set(const float x, const float y, const float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

//
// kexVec3::Clear
//

void kexVec3::Clear(void) {
    x = y = z = 0.0f;
}

//
// kexVec3::Dot
//

float kexVec3::Dot(const kexVec3 &vec) const {
    return (x * vec.x + y * vec.y + z * vec.z);
}

//
// kexVec3::Dot
//

float kexVec3::Dot(const kexVec3 &vec1, const kexVec3 &vec2) {
    return (vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z);
}

//
// kexVec3::Cross
//

kexVec3 kexVec3::Cross(const kexVec3 &vec) const {
    return kexVec3(
        vec.z * y - z * vec.y,
        vec.x * z - x * vec.z,
        x * vec.y - vec.x * y
    );
}

//
// kexVec3::Cross
//

kexVec3 &kexVec3::Cross(const kexVec3 &vec1, const kexVec3 &vec2) {
    x = vec2.z * vec1.y - vec1.z * vec2.y;
    y = vec2.x * vec1.z - vec1.x * vec2.z;
    z = vec1.x * vec2.y - vec2.x * vec1.y;

    return *this;
}

//
// kexVec3::UnitSq
//

float kexVec3::UnitSq(void) const {
    return x * x + y * y + z * z;
}

//
// kexVec3::Unit
//

float kexVec3::Unit(void) const {
    return kexMath::Sqrt(UnitSq());
}

//
// kexVec3::DistanceSq
//

float kexVec3::DistanceSq(const kexVec3 &vec) const {
    return (
        (x - vec.x) * (x - vec.x) +
        (y - vec.y) * (y - vec.y) +
        (z - vec.z) * (z - vec.z)
    );
}

//
// kexVec3::Distance
//

float kexVec3::Distance(const kexVec3 &vec) const {
    return kexMath::Sqrt(DistanceSq(vec));
}

//
// kexVec3::Normalize
//

kexVec3 &kexVec3::Normalize(void) {
    float d = Unit();
    if(d != 0.0f) {
        d = 1.0f / d;
        *this *= d;
    }
    return *this;
}

//
// kexVec3::PointAt
//

kexVec3 kexVec3::PointAt(kexVec3 &location) const {
    float an1 = (float)atan2(location.x - x, location.z - z);
    float an2 = (float)atan2(location.Distance(*this), location.y - y);

    return kexVec3(
        kexMath::Sin(an1),
        kexMath::Cos(an2),
        kexMath::Cos(an1)
    );
}

//
// kexVec3::Lerp
//

kexVec3 kexVec3::Lerp(const kexVec3 &next, float movement) const {
    return (next - *this) * movement + *this;
}

//
// kexVec3::Lerp
//

kexVec3 &kexVec3::Lerp(const kexVec3 &start, const kexVec3 &next, float movement) {
    *this = (next - start) * movement + start;
    return *this;
}

//
// kexVec3::ToQuat
//

kexQuat kexVec3::ToQuat(void) {
    float d = Unit();

    if(d == 0.0f)
        return kexQuat();

    kexVec3 scv = *this * (1.0f / d);
    float angle = kexMath::ACos(scv.z);

    return kexQuat(angle, vecForward.Cross(scv).Normalize());
}

//
// kexVec3::ToYaw
//

float kexVec3::ToYaw(void) const {
    float d = x * x + z * z;

    if(d == 0.0f)
        return 0.0f;

    float an = -(z / kexMath::Sqrt(d));

    if(an >  1.0f) an =  1.0f;
    if(an < -1.0f) an = -1.0f;

    if(-x <= 0.0f) {
        return -kexMath::ACos(an);
    }

    return kexMath::ACos(an);
}

//
// kexVec3::ToPitch
//

float kexVec3::ToPitch(void) const {
    float d = UnitSq();
    
    if(d == 0.0f)
        return 0.0f;
        
    return kexMath::ACos(y / kexMath::Sqrt(d));
}

//
// kexVec3::ToString
//

kexStr kexVec3::ToString(void) const {
    kexStr str;
    str = str + x + " " + y + " " + z;
    return str;
}

//
// kexVec3::ToFloatPtr
//

float *kexVec3::ToFloatPtr(void) {
    return reinterpret_cast<float*>(&x);
}

//
// kexVec3::ScreenProject
//

kexVec3 kexVec3::ScreenProject(kexMatrix &proj, kexMatrix &model,
                               const int width, const int height,
                               const int wx, const int wy) {
    kexVec4 projVec;
    kexVec4 modelVec;

    modelVec.ToVec3() = *this;
    modelVec |= model;

    projVec = (modelVec | proj);
    projVec.x *= modelVec.w;
    projVec.y *= modelVec.w;
    projVec.z *= modelVec.w;

    if(projVec.w != 0) {
        projVec.w = 1.0f / projVec.w;
        projVec.x *= projVec.w;
        projVec.y *= projVec.w;
        projVec.z *= projVec.w;

        return kexVec3(
            (projVec.x * 0.5f + 0.5f) * width + wx,
            (-projVec.y * 0.5f + 0.5f) * height + wy,
            (1.0f + projVec.z) * 0.5f);
    }

    return kexVec3(*this);
}

//
// kexVec3::operator+
//

kexVec3 kexVec3::operator+(const kexVec3 &vec) {
    return kexVec3(x + vec.x, y + vec.y, z + vec.z);
}

//
// kexVec3::operator+
//

kexVec3 kexVec3::operator+(const kexVec3 &vec) const {
    return kexVec3(x + vec.x, y + vec.y, z + vec.z);
}

//
// kexVec3::operator+
//

kexVec3 kexVec3::operator+(kexVec3 &vec) {
    return kexVec3(x + vec.x, y + vec.y, z + vec.z);
}

//
// kexVec3::operator+=
//

kexVec3 &kexVec3::operator+=(const kexVec3 &vec) {
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

//
// kexVec3::operator-
//

kexVec3 kexVec3::operator-(const kexVec3 &vec) const {
    return kexVec3(x - vec.x, y - vec.y, z - vec.z);
}

//
// kexVec3::operator-
//

kexVec3 kexVec3::operator-(void) const {
    return kexVec3(-x, -y, -z);
}

//
// kexVec3::operator-=
//

kexVec3 &kexVec3::operator-=(const kexVec3 &vec) {
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

//
// kexVec3::operator*
//

kexVec3 kexVec3::operator*(const kexVec3 &vec) {
    return kexVec3(x * vec.x, y * vec.y, z * vec.z);
}

//
// kexVec3::operator*=
//

kexVec3 &kexVec3::operator*=(const kexVec3 &vec) {
    x *= vec.x;
    y *= vec.y;
    z *= vec.z;
    return *this;
}

//
// kexVec3::operator*
//

kexVec3 kexVec3::operator*(const float val) {
    return kexVec3(x * val, y * val, z * val);
}

//
// kexVec3::operator*
//

kexVec3 kexVec3::operator*(const float val) const {
    return kexVec3(x * val, y * val, z * val);
}

//
// kexVec3::operator*=
//

kexVec3 &kexVec3::operator*=(const float val) {
    x *= val;
    y *= val;
    z *= val;
    return *this;
}

//
// kexVec3::operator/
//

kexVec3 kexVec3::operator/(const kexVec3 &vec) {
    return kexVec3(x / vec.x, y / vec.y, z / vec.z);
}

//
// kexVec3::operator/=
//

kexVec3 &kexVec3::operator/=(const kexVec3 &vec) {
    x /= vec.x;
    y /= vec.y;
    z /= vec.z;
    return *this;
}

//
// kexVec3::operator/
//

kexVec3 kexVec3::operator/(const float val) {
    return kexVec3(x / val, y / val, z / val);
}

//
// kexVec3::operator/=
//

kexVec3 &kexVec3::operator/=(const float val) {
    x /= val;
    y /= val;
    z /= val;
    return *this;
}

//
// kexVec3::operator|
//

kexVec3 kexVec3::operator|(const kexQuat &quat) {
    float xx = quat.x * quat.x;
    float yx = quat.y * quat.x;
    float zx = quat.z * quat.x;
    float wx = quat.w * quat.x;
    float yy = quat.y * quat.y;
    float zy = quat.z * quat.y;
    float wy = quat.w * quat.y;
    float zz = quat.z * quat.z;
    float wz = quat.w * quat.z;
    float ww = quat.w * quat.w;

    return kexVec3(
        ((yx + yx) - (wz + wz)) * y +
        ((wy + wy + zx + zx)) * z +
        (((ww + xx) - yy) - zz) * x,
        ((yy + (ww - xx)) - zz) * y +
        ((zy + zy) - (wx + wx)) * z +
        ((wz + wz) + (yx + yx)) * x,
        ((wx + wx) + (zy + zy)) * y +
        (((ww - xx) - yy) + zz) * z +
        ((zx + zx) - (wy + wy)) * x
    );
}

//
// kexVec3::operator|
//

kexVec3 kexVec3::operator|(const kexMatrix &mtx) {
    return kexVec3(
        mtx.vectors[1].x * y + mtx.vectors[2].x * z + mtx.vectors[0].x * x + mtx.vectors[3].x,
        mtx.vectors[1].y * y + mtx.vectors[2].y * z + mtx.vectors[0].y * x + mtx.vectors[3].y,
        mtx.vectors[1].z * y + mtx.vectors[2].z * z + mtx.vectors[0].z * x + mtx.vectors[3].z);
}

//
// kexVec3::operator|=
//

kexVec3 &kexVec3::operator|=(const kexQuat &quat) {
    float xx = quat.x * quat.x;
    float yx = quat.y * quat.x;
    float zx = quat.z * quat.x;
    float wx = quat.w * quat.x;
    float yy = quat.y * quat.y;
    float zy = quat.z * quat.y;
    float wy = quat.w * quat.y;
    float zz = quat.z * quat.z;
    float wz = quat.w * quat.z;
    float ww = quat.w * quat.w;
    float vx = x;
    float vy = y;
    float vz = z;

    x = ((yx + yx) - (wz + wz)) * vy +
        ((wy + wy + zx + zx)) * vz +
        (((ww + xx) - yy) - zz) * vx;
    y = ((yy + (ww - xx)) - zz) * vy +
        ((zy + zy) - (wx + wx)) * vz +
        ((wz + wz) + (yx + yx)) * vx;
    z = ((wx + wx) + (zy + zy)) * vy +
        (((ww - xx) - yy) + zz) * vz +
        ((zx + zx) - (wy + wy)) * vx;

    return *this;
}

//
// kexVec3::operator|=
//

kexVec3 &kexVec3::operator|=(const kexMatrix &mtx) {
    float _x = x;
    float _y = y;
    float _z = z;
    
    x = mtx.vectors[1].x * _y + mtx.vectors[2].x * _z + mtx.vectors[0].x * _x + mtx.vectors[3].x;
    y = mtx.vectors[1].y * _y + mtx.vectors[2].y * _z + mtx.vectors[0].y * _x + mtx.vectors[3].y;
    z = mtx.vectors[1].z * _y + mtx.vectors[2].z * _z + mtx.vectors[0].z * _x + mtx.vectors[3].z;

    return *this;
}

//
// kexVec3::operator=
//

kexVec3 &kexVec3::operator=(const kexVec3 &vec) {
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

//
// kexVec3::operator=
//

kexVec3 &kexVec3::operator=(const float *vecs) {
    x = vecs[0];
    y = vecs[1];
    z = vecs[2];
    return *this;
}

//
// kexVec3::operator[]
//

float kexVec3::operator[](int index) const {
    assert(index >= 0 && index < 3);
    return (&x)[index];
}

//
// kexVec3::operator[]
//

float &kexVec3::operator[](int index) {
    assert(index >= 0 && index < 3);
    return (&x)[index];
}

#ifndef EDITOR
//
// kexVec3::ObjectConstruct1
//

void kexVec3::ObjectConstruct1(kexVec3 *thisvec) {
    new(thisvec)kexVec3();
}

//
// kexVec3::ObjectConstruct2
//

void kexVec3::ObjectConstruct2(float x, float y, float z, kexVec3 *thisvec) {
    new(thisvec)kexVec3(x, y, z);
}

//
// kexVec3::ObjectConstructCopy
//

void kexVec3::ObjectConstructCopy(const kexVec3 &in, kexVec3 *thisvec) {
    new(thisvec)kexVec3(in);
}
#endif

//
// kexVec4::kexVec4
//

kexVec4::kexVec4(void) {
    Clear();
}

//
// kexVec4::kexVec4
//

kexVec4::kexVec4(const float x, const float y, const float z, const float w) {
    Set(x, y, z, w);
}

//
// kexVec4::Set
//

void kexVec4::Set(const float x, const float y, const float z, const float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

//
// kexVec4::Clear
//

void kexVec4::Clear(void) {
    x = y = z = w = 0.0f;
}

//
// kexVec4::ToVec3
//

kexVec3 const &kexVec4::ToVec3(void) const {
    return *reinterpret_cast<const kexVec3*>(this);
}

//
// kexVec4::ToVec3
//

kexVec3 &kexVec4::ToVec3(void) {
    return *reinterpret_cast<kexVec3*>(this);
}

//
// kexVec4::ToFloatPtr
//

float *kexVec4::ToFloatPtr(void) {
    return reinterpret_cast<float*>(&x);
}

//
// kexVec4::operator|
//

kexVec4 kexVec4::operator|(const kexMatrix &mtx) {
    return kexVec4(
        mtx.vectors[1].x * y + mtx.vectors[2].x * z + mtx.vectors[0].x * x + mtx.vectors[3].x,
        mtx.vectors[1].y * y + mtx.vectors[2].y * z + mtx.vectors[0].y * x + mtx.vectors[3].y,
        mtx.vectors[1].z * y + mtx.vectors[2].z * z + mtx.vectors[0].z * x + mtx.vectors[3].z,
        mtx.vectors[1].w * y + mtx.vectors[2].w * z + mtx.vectors[0].w * x + mtx.vectors[3].w);
}

//
// kexVec4::operator|=
//

kexVec4 &kexVec4::operator|=(const kexMatrix &mtx) {
    float _x = x;
    float _y = y;
    float _z = z;
    float _w = w;
    
    x = mtx.vectors[1].x * _y + mtx.vectors[2].x * _z + mtx.vectors[0].x * _x + mtx.vectors[3].x;
    y = mtx.vectors[1].y * _y + mtx.vectors[2].y * _z + mtx.vectors[0].y * _x + mtx.vectors[3].y;
    z = mtx.vectors[1].z * _y + mtx.vectors[2].z * _z + mtx.vectors[0].z * _x + mtx.vectors[3].z;
    w = mtx.vectors[1].w * _y + mtx.vectors[2].w * _z + mtx.vectors[0].w * _x + mtx.vectors[3].w;

    return *this;
}

//
// kexVec4::operator[]
//

float kexVec4::operator[](int index) const {
    assert(index >= 0 && index < 3);
    return (&x)[index];
}

//
// kexVec4::operator[]
//

float kexVec4::operator[](int index) {
    assert(index >= 0 && index < 3);
    return (&x)[index];
}
