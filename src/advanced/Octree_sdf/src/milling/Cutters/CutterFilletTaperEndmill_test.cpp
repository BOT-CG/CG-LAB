#include "CutterFilletTaperEndmill.h"
#include "utils.h"

#include <iomanip>
#include <iostream>
#include <vector>

int main()
{
    // float _d, float _r, float _e, float _f, float _alpha, float _beta, float _h
    CutterFilletTaperEndmill cutterFilletTaperEndmill(10.0f, 3.0f, degreeToRadian(10.0f), 100.0f);
    Posture currentPosture = Posture {
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
        std::cout << p << ": " << std::fixed << std::setprecision(3) << cutterFilletTaperEndmill.signedDistance(currentPosture, p) << std::endl;
    }

    return 0;
}
