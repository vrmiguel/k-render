#ifndef __KRENDER_VEC_H
#define __KRENDER_VEC_H

#include <cmath>
#include <fstream>

//! Light modification of Dmitry V. Sokolov's implementation of Vec2 and Vec3 templates.
//! See ktypes.h/.cpp for Dmitry's copyright notice.

template <class t> struct Vec2 {
    struct {t x, y; };
    Vec2() : x(0), y(0) {}
    Vec2(t _x, t _y) : x(_x), y(_y) {}
    inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(x+V.x, y+V.y); } // Add two Vec2s
    inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(x-V.x, y-V.y); } // Subtract two Vec2
    inline Vec2<t> operator *(float f)          const { return Vec2<t>(x*f, y*f); }     // Multiply Vec2 by a scalar
    template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
    union {
        struct {t x, y, z;};
        struct {t ivert, iuv, inorm; };
        t raw[3];
    };
    Vec3() : x(0), y(0), z(0) {}
    Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
    inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
    inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
    inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
    inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
    inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
    float norm () const
    {
        return std::sqrt(x*x+y*y+z*z);
    }
    Vec3<t> & normalize(t l=1)
    {
        *this = (*this)*(l/norm());
        return *this;
    }
    template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};

template <class t> Vec3<t> cross(Vec3<t> v1,Vec3<t> v2) {
    return Vec3<t>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

#endif // __KRENDER_VEC_H
