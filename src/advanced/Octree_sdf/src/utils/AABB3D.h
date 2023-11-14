#pragma once

#include "BBox3D.h"
#include "Vector3D.h"

#include <algorithm>
#include <ostream>

template <class T>
class AABB3D : public BBox3D<T> {
public:
    AABB3D() = default;
    ~AABB3D() = default;

    constexpr inline AABB3D(const Vector3D<T>& min, const Vector3D<T>& max) noexcept
        : min(min)
        , max(max)
    {
    }
    constexpr inline AABB3D(const Vector3D<T>& center, const T halfSize) noexcept
        : min(center - Vector3D<T>(halfSize, halfSize, halfSize))
        , max(center + Vector3D<T>(halfSize, halfSize, halfSize))
    {
    }
    constexpr inline AABB3D(const Vector3D<T>& center, const T halfSizeX, const T halfSizeY, const T halfSizeZ) noexcept
        : min(center - Vector3D<T>(halfSizeX, halfSizeY, halfSizeZ))
        , max(center + Vector3D<T>(halfSizeX, halfSizeY, halfSizeZ))
    {
    }

    constexpr inline bool isAxisAligned() const noexcept override
    {
        return true;
    }

    constexpr inline Vector3D<T> getCenter() const noexcept override
    {
        return (min + max) * (T)0.5;
    }

    constexpr inline Vector3D<T> getMin() const noexcept override
    {
        return min;
    }

    constexpr inline Vector3D<T> getMax() const noexcept override
    {
        return max;
    }

    constexpr inline Vector3D<T> getAxis(int index) const noexcept override
    {
        return Vector3D<T>(
            index == 0 ? (max.x - min.x) : 0,
            index == 1 ? (max.y - min.y) : 0,
            index == 2 ? (max.z - min.z) : 0);
    }

    constexpr inline T getHalfSize(int index) const noexcept override
    {
        switch (index) {
        case 0:
            return (max.x - min.x) * (T)0.5;
        case 1:
            return (max.y - min.y) * (T)0.5;
        case 2:
            return (max.z - min.z) * (T)0.5;
        default:
            return (T)0;
        }
    }

    constexpr inline T signedDistanceToPoint(const Vector3D<T>& p) const noexcept override
    {
        if (isInside(p)) {
            T dx = std::min(p.x - min.x, max.x - p.x);
            T dy = std::min(p.y - min.y, max.y - p.y);
            T dz = std::min(p.z - min.z, max.z - p.z);
            return std::min({ dx, dy, dz });
        }

        T dx = std::min({ p.x - min.x, (T)0, max.x - p.x });
        T dy = std::min({ p.y - min.y, (T)0, max.y - p.y });
        T dz = std::min({ p.z - min.z, (T)0, max.z - p.z });
        return -std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    constexpr inline bool isEmpty() const noexcept override
    {
        return min == max;
    }

    constexpr inline bool isValid() const noexcept override
    {
        return min < max;
    }

    constexpr inline bool isInside(const Vector3D<T>& p) const noexcept override
    {
        return min <= p && p <= max;
    }

    constexpr inline bool isInsideStrictly(const Vector3D<T>& p) const noexcept override
    {
        return min < p && p < max;
    }

    bool isInside(const BBox3D<T>& b) const noexcept override
    {
        return min <= b.getMin() && b.getMax() <= max;
    }

    bool isInsideStrictly(const BBox3D<T>& b) const noexcept override
    {
        return min < b.getMin() && b.getMax() < max;
    }

    bool intersects(const BBox3D<T>& b) const noexcept override
    {
        if (b.isAxisAligned()) {
            return intersects(dynamic_cast<const AABB3D<T>&>(b));
        }
        return b.intersects(*this);
    }

    bool intersects(const AABB3D<T>& a) const noexcept
    {
        return min <= a.max && a.min <= max;
    }

    bool intersects(const Vector3D<T>& p, const Vector3D<T>& q) const noexcept
    {
        return min <= p && p <= max && min <= q && q <= max;
    }

    bool intersects(const Vector3D<T>& p, const Vector3D<T>& q, Vector3D<T>& intersection) const noexcept
    {
        if (min <= p && p <= max && min <= q && q <= max) {
            intersection = std::max(p, q);
            return true;
        }
        return false;
    }

    constexpr inline bool operator==(const AABB3D<T>& a) const noexcept
    {
        return min == a.min && max == a.max;
    }

private:
    Vector3D<T> min, max;

    void print(std::ostream& os) const noexcept override
    {
        os << "AABB3D(" << min << ", " << max << ")";
    }
};

using AABB3Df = AABB3D<float>;
