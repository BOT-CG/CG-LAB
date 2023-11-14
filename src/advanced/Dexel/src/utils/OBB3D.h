#pragma once

#include "BBox3D.h"
#include "Vector3D.h"

#include <cmath>
#include <ostream>

template <class T>
class OBB3D : public BBox3D<T> {
public:
    OBB3D() = default;
    ~OBB3D() = default;

    constexpr inline OBB3D(const Vector3D<T>& center, const Vector3D<T>& axisX, const Vector3D<T>& axisY, const Vector3D<T>& axisZ) noexcept
        : center(center)
    {
        axis[0] = axisX;
        axis[1] = axisY;
        axis[2] = axisZ;
    }
    constexpr inline OBB3D(const Vector3D<T>& center, const T halfSize) noexcept
        : center(center)
    {
        axis[0] = Vector3D<T>(halfSize, 0, 0);
        axis[1] = Vector3D<T>(0, halfSize, 0);
        axis[2] = Vector3D<T>(0, 0, halfSize);
    }
    constexpr inline OBB3D(const Vector3D<T>& center, const T halfSizeX, const T halfSizeY, const T halfSizeZ) noexcept
        : center(center)
    {
        axis[0] = Vector3D<T>(halfSizeX, 0, 0);
        axis[1] = Vector3D<T>(0, halfSizeY, 0);
        axis[2] = Vector3D<T>(0, 0, halfSizeZ);
    }

    constexpr inline bool isAxisAligned() const noexcept override
    {
        return false;
    }

    constexpr inline Vector3D<T> getCenter() const noexcept override
    {
        return center;
    }

    constexpr inline Vector3D<T> getMin() const noexcept override
    {
        return center - axis[0] - axis[1] - axis[2];
    }

    constexpr inline Vector3D<T> getMax() const noexcept override
    {
        return center + axis[0] + axis[1] + axis[2];
    }

    constexpr inline Vector3D<T> getAxis(int index) const noexcept override
    {
        return axis[index];
    }

    constexpr inline T getHalfSize(int index) const noexcept override
    {
        return axis[index].length();
    }

    constexpr inline T signedDistanceToPoint(const Vector3D<T>& p) const noexcept override
    {
        Vector3D<T> d = p - center;
        T d0 = axis[0].length() - std::abs(d.dot(axis[0]));
        T d1 = axis[1].length() - std::abs(d.dot(axis[1]));
        T d2 = axis[2].length() - std::abs(d.dot(axis[2]));
        if (isInside(p)) {
            return std::min({ d0, d1, d2 });
        }
        d0 = std::min(d0, (T)0);
        d1 = std::min(d1, (T)0);
        d2 = std::min(d2, (T)0);
        return -std::sqrt(d0 * d0 + d1 * d1 + d2 * d2);
    }

    constexpr inline bool isEmpty() const noexcept override
    {
        return axis[0].isZero() && axis[1].isZero() && axis[2].isZero();
    }

    constexpr inline bool isValid() const noexcept override
    {
        return !isEmpty();
    }

    constexpr inline bool isInside(const Vector3D<T>& p) const noexcept override
    {
        auto d = p - center;
        return std::abs(d.dot(axis[0])) <= axis[0].length() && std::abs(d.dot(axis[1])) <= axis[1].length() && std::abs(d.dot(axis[2])) <= axis[2].length();
    }

    constexpr inline bool isInsideStrictly(const Vector3D<T>& p) const noexcept override
    {
        auto d = p - center;
        return std::abs(d.dot(axis[0])) < axis[0].length() && std::abs(d.dot(axis[1])) < axis[1].length() && std::abs(d.dot(axis[2])) < axis[2].length();
    }

    bool isInside(const BBox3D<T>& b) const noexcept override
    {
        return b.isInside(getMin()) && b.isInside(getMax());
    }

    bool isInsideStrictly(const BBox3D<T>& b) const noexcept override
    {
        return b.isInsideStrictly(getMin()) && b.isInsideStrictly(getMax());
    }

    bool intersects(const BBox3D<T>& b) const noexcept override
    {
        if (!b.isAxisAligned()) {
            return intersects(dynamic_cast<const OBB3D<T>&>(b));
        }

        return intersects(OBB3D<T>(b.getCenter(), b.getHalfSize(0), b.getHalfSize(1), b.getHalfSize(2)));
    }

    bool intersects(const OBB3D<T>& o) const noexcept
    {
        for (int i = 0; i < 3; i++) {
            if (isSeparatingAxis(o, axis[i])) {
                return false;
            }
            if (isSeparatingAxis(o, o.axis[i])) {
                return false;
            }
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (isSeparatingAxis(o, o.axis[i].cross(axis[j]))) {
                    return false;
                }
            }
        }

        return true;
    }

    constexpr inline T projectionLength(const Vector3D<T>& a) const noexcept
    {
        return axis[0].projectionLength(a) + axis[1].projectionLength(a) + axis[2].projectionLength(a);
    }

    constexpr inline bool isSeparatingAxis(const OBB3D<T>& o, const Vector3D<T>& a) const noexcept
    {
        return (o.center - center).projectionLength(a) >= o.projectionLength(a) + projectionLength(a);
    }

private:
    Vector3D<T> center;
    Vector3D<T> axis[3];

    void print(std::ostream& os) const noexcept override
    {
        os << "OBB3D: center: " << center << " axis: " << axis[0] << " " << axis[1] << " " << axis[2];
    }
};

using OBB3Df = OBB3D<float>;
