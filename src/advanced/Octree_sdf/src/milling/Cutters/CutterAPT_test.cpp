#include "CutterAPT.h"
#include "CutterFactory.h"

#include <iomanip>
#include <iostream>
#include <vector>

int main()
{
    // float _d, float _r, float _e, float _f, float _alpha, float _beta, float _h
    CutterAPT cutterAPT(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f);
    Posture cutterAPTCurrentPosture = Posture {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };
    std::unique_ptr<Cutter> cutter = CutterFactory::create(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 20.0f);
    Posture cutterCurrentPosture = Posture {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    std::vector<Vector3Df> points = {
        { 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 5.0f },
        { 0.0f, 0.0f, 10.0f },
        { 0.0f, 0.0f, 20.0f },
        { 0.0f, 0.0f, 100.0f },
        { 5.0f, 0.0f, 0.0f },
        { 5.0f, 0.0f, 10.0f },
        { 5.0f, 0.0f, 20.0f },
        { 5.0f, 0.0f, 100.0f },
        { 15.0f, 0.0f, 50.0f },
    };
    for (auto& p : points) {
        std::cout << p << ": " << std::fixed << std::setprecision(3) << cutterAPT.signedDistance(cutterAPTCurrentPosture, p) << " " << cutter->signedDistance(cutterCurrentPosture, p) << std::endl;
    }

    return 0;
}
