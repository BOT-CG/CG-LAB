#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <random>

template <typename T>
constexpr inline T pi = static_cast<T>(3.14159265358979323846);

template <typename T>
constexpr inline T radianToDegree(T radian) noexcept
{
    // return radian / std::numbers::pi_v<T> * (T)180.0;
    return radian / pi<T> * static_cast<T>(180.0);
}

template <typename T>
constexpr inline T degreeToRadian(T degree) noexcept
{
    return degree / static_cast<T>(180.0) * pi<T>;
    // return degree / (T)180.0 * std::numbers::pi_v<T>;
}

inline bool floatEqual(float a, float b) noexcept
{
    return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

template <class T>
static T generateRandom(T min, T max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dis(min, max);
    return dis(gen);
}
