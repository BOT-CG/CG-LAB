#pragma once

#include "Vector3D.h"

#include <algorithm>
#include <array>
#include <ostream>

template <class T>
class Triangle3D {
public:
    constexpr inline Triangle3D() noexcept = default;
    constexpr inline Triangle3D(const Vector3D<T>& _v0, const Vector3D<T>& _v1, const Vector3D<T>& _v2, const Vector3D<T>& _n) noexcept
        : Vertices { _v0, _v1, _v2 }
        , Normal(_n.normalize())
    {
    }
    constexpr inline Triangle3D(const Vector3D<T>& _v0, const Vector3D<T>& _v1, const Vector3D<T>& _v2) noexcept
        : Triangle3D(_v0, _v1, _v2, (_v1 - _v0).cross(_v2 - _v0))
    {
    }
    constexpr inline Triangle3D(const std::array<Vector3D<T>, 3>& _vertices, const Vector3D<T>& _n) noexcept
        : Vertices(_vertices)
        , Normal(_n.normalize())
    {
    }
    constexpr inline Triangle3D(const Triangle3D& t) noexcept
        : Vertices(t.Vertices)
        , Normal(t.Normal)
    {
    }

    ~Triangle3D() = default;

    constexpr inline bool equal(const Triangle3D& t) const noexcept
    {
        return std::all_of(Vertices.begin(), Vertices.end(), [&t](const Vector3D<T>& v) {
            return std::any_of(t.Vertices.begin(), t.Vertices.end(), [&v](const Vector3D<T>& v2) {
                return v.equal(v2);
            });
        });
    }

    constexpr inline T minX() const noexcept
    {
        return std::min({ Vertices[0].x, Vertices[1].x, Vertices[2].x });
    }
    constexpr inline T maxX() const noexcept
    {
        return std::max({ Vertices[0].x, Vertices[1].x, Vertices[2].x });
    }
    constexpr inline T minY() const noexcept
    {
        return std::min({ Vertices[0].y, Vertices[1].y, Vertices[2].y });
    }
    constexpr inline T maxY() const noexcept
    {
        return std::max({ Vertices[0].y, Vertices[1].y, Vertices[2].y });
    }
    constexpr inline T minZ() const noexcept
    {
        return std::min({ Vertices[0].z, Vertices[1].z, Vertices[2].z });
    }
    constexpr inline T maxZ() const noexcept
    {
        return std::max({ Vertices[0].z, Vertices[1].z, Vertices[2].z });
    }

    constexpr inline bool isInside(const Vector3D<T>& p) const noexcept
    {
        const Vector3D<T> v01 = Vertices[1] - Vertices[0];
        const Vector3D<T> v12 = Vertices[2] - Vertices[1];
        const Vector3D<T> v20 = Vertices[0] - Vertices[2];
        const Vector3D<T> v0p = p - Vertices[0];
        const Vector3D<T> v1p = p - Vertices[1];
        const Vector3D<T> v2p = p - Vertices[2];
        const T c0 = v0p.cross(v01).dot(Normal);
        const T c1 = v1p.cross(v12).dot(Normal);
        const T c2 = v2p.cross(v20).dot(Normal);
        return (c0 >= 0 && c1 >= 0 && c2 >= 0) || (c0 <= 0 && c1 <= 0 && c2 <= 0);
    }

    constexpr inline T distanceAsPlane(const Vector3D<T>& p) const noexcept
    {
        return Normal.dot(p - Vertices[0]);
    }

    constexpr inline bool isOnPlane(const Vector3D<T>& p) const noexcept
    {
        return distanceAsPlane(p) == 0;
    }

    constexpr inline bool isSameSideAsNormal(const Vector3D<T>& p) const noexcept
    {
        return distanceAsPlane(p) > 0;
    }

    constexpr inline Vector3D<T> projection(const Vector3D<T>& p) const noexcept
    {
        return p - Normal * distanceAsPlane(p);
    }

    constexpr inline T distance(const Vector3D<T>& p) const noexcept
    {
        if (isInside(projection(p))) {
            return std::abs(distanceAsPlane(p));
        }
        T distanceToLine1 = p.distanceToLineSegment(Vertices[0], Vertices[1]);
        T distanceToLine2 = p.distanceToLineSegment(Vertices[1], Vertices[2]);
        T distanceToLine3 = p.distanceToLineSegment(Vertices[2], Vertices[0]);
        T minDistance = std::min({ distanceToLine1, distanceToLine2, distanceToLine3 });
        return minDistance;
    }

    constexpr inline T signedDistance(const Vector3D<T>& p) const noexcept
    {
        return isSameSideAsNormal(p) ? distance(p) : -distance(p);
    }

    constexpr inline Triangle3D& operator=(const Triangle3D& t) noexcept
    {
        Vertices = t.Vertices;
        Normal = t.Normal;
        return *this;
    }

    Vector3D<T> Normal;
    std::array<Vector3D<T>, 3> Vertices;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Triangle3D<T>& t)
{
    os << "Triangle3D(" << t.Vertices[0] << ", " << t.Vertices[1] << ", " << t.Vertices[2] << ", " << t.Normal << ")";
    return os;
}

using Triangle3Df = Triangle3D<float>;
