#pragma once

#include "Vector3D.h"
#include <cstdint>

#if _MSC_VER || defined(__BMI2__)
#define USE_BMI2
#include <immintrin.h>
#endif

class Morton {
public:
    Morton() = default;
    ~Morton() = default;

    static uint64_t encode(const Vector3Du& v)
    {
#ifdef USE_BMI2
        return (_pdep_u64(v.z, z3_mask) | _pdep_u64(v.y, y3_mask) | _pdep_u64(v.x, x3_mask));
#else
        return (dilateBits(v.x) << 2) | (dilateBits(v.y) << 1) | dilateBits(v.z);
#endif
    }

    static uint64_t encode(uint32_t x, uint32_t y, uint32_t z)
    {
        return encode(Vector3Du(x, y, z));
    }

    static Vector3Du decode(uint64_t morton)
    {
#ifdef USE_BMI2
        return {
            (uint32_t)_pext_u64(morton, x3_mask),
            (uint32_t)_pext_u64(morton, y3_mask),
            (uint32_t)_pext_u64(morton, z3_mask)
        };
#else
        return Vector3Du(compactBits(morton >> 2), compactBits(morton >> 1), compactBits(morton));
#endif
    }

    static void decode(uint64_t morton, uint32_t& x, uint32_t& y, uint32_t& z)
    {
        Vector3Du v = decode(morton);
        x = v.x;
        y = v.y;
        z = v.z;
    }

private:
    static const uint64_t x3_mask = 0x4924924924924924; // 0b...00100100
    static const uint64_t y3_mask = 0x2492492492492492; // 0b...10010010
    static const uint64_t z3_mask = 0x9249249249249249; // 0b...01001001

    static uint64_t dilateBits(const uint32_t a)
    {
        uint64_t x = a & 0x1fffff;
        x = (x | x << 32) & 0x1f00000000ffff;
        x = (x | x << 16) & 0x1f0000ff0000ff;
        x = (x | x << 8) & 0x100f00f00f00f00f;
        x = (x | x << 4) & 0x10c30c30c30c30c3;
        x = (x | x << 2) & 0x1249249249249249;
        return x;
    }
    static uint32_t compactBits(const uint64_t a)
    {
        uint64_t x = a & 0x1249249249249249;
        x = (x ^ (x >> 2)) & 0x30c30c30c30c30c3;
        x = (x ^ (x >> 4)) & 0xf00f00f00f00f00f;
        x = (x ^ (x >> 8)) & 0x00ff0000ff0000ff;
        x = (x ^ (x >> 16)) & 0x00ff00000000ffff;
        x = (x ^ (x >> 32)) & 0x1fffff;
        return (uint32_t)x;
    }
};