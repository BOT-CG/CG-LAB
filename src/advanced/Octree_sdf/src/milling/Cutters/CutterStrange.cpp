#include "CutterStrange.h"
#include "utils.h"

#include <cmath>

std::function<float(float, float)> CutterStrange::defaultF = [](float x, float z) {
    return -std::cos(2 * x) + 0.2f * x * x - z + 1;
};

static std::pair<float, float> df(std::function<float(float, float)> f, float x, float z)
{
    constexpr float eps = 1e-4f;
    return { (f(x + eps, z) - f(x - eps, z)) / (2 * eps), (f(x, z + eps) - f(x, z - eps)) / (2 * eps) };
}

static float findX(std::function<float(float, float)> f, float z, float fxz)
{
    constexpr float eps = 1e-4f;
    float x = generateRandom(0.0f, 100.0f);

    uint8_t iter = 0;
    while (iter++ < 100 && std::abs(f(x, z) - fxz) > eps) {
        float dfx = df(f, x, z).first;
        x -= (f(x, z) - fxz) / dfx;
    }

    return x;
}

CutterStrange::CutterStrange(float _height, std::function<float(float, float)> _f)
    : height(_height)
    , f(_f)
    , radius(findX(_f, height, 0.0f))
{
}

OBB3Df CutterStrange::getBBox(const Posture& currentPosture) const
{
    return createBBox(currentPosture, radius, height);
}

bool CutterStrange::isInside(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);
    return f(x, z) < 0;
}

float CutterStrange::signedDistance(const Posture& currentPosture, const Vector3Df& p) const
{
    float x = distanceToZAxis(currentPosture, p);
    float z = projectToZAxisLength(currentPosture, p);
    auto [dx, dz] = df(f, x, z);
    float d = std::hypot(dx, dz);
    return -f(x, z) / d;
}

bool CutterStrange::getSweptVolume(SweptVolmueParam& sweptVolmueParam, float& Heigth, Vector3Df& CutterPoint, const float& Dexel_X, const float& Dexel_Y)
{
    return false;
}

float CutterStrange::getDexelHeightZ(float distance) const
{
    return 0;
}
