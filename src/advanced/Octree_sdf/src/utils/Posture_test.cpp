#include "Posture.h"

#include <iostream>

void postureLineInterpolatorTest1()
{
    // N1920X-73.035625136912Y15.012525368425Z-25.559818694236I-0.895613236182J0.294075028467K0.333761604754
    // N1921X-73.298416736741Y14.253715872141Z-25.596409814273I-0.895657987845J0.293816281320K0.333869378110

    Posture start({ -73.035625136912, 15.012525368425, -25.559818694236 }, { -0.895613236182, 0.294075028467, 0.333761604754 });
    Posture end({ -73.298416736741, 14.253715872141, -25.596409814273 }, { -0.895657987845, 0.293816281320, 0.333869378110 });
    PostureLineInterpolator interpolator(start, end, 0.1f, 0.1f);
    Posture posture;
    while (!interpolator.getPostureByStep(posture)) {
        std::cout << posture.toString() << std::endl;
    }
}

void postureLineInterpolatorTest2()
{
    // N277X-44.965915282213Y89.513518195458Z-29.920910244412I-0.040989431677J0.918786021517K0.392622099679
    // N278X-45.642191932882Y89.466555026859Z-29.881613151149I-0.040989431677J0.918786021517K0.392622099679

    Posture start({ -44.965915282213, 89.513518195458, -29.920910244412 }, { -0.040989431677, 0.918786021517, 0.392622099679 });
    Posture end({ -45.642191932882, 89.466555026859, -29.881613151149 }, { -0.040989431677, 0.918786021517, 0.392622099679 });
    PostureLineInterpolator interpolator(start, end, 0.1f, degreeToRadian(0.5f));
    Posture posture;
    while (!interpolator.getPostureByStep(posture)) {
        std::cout << posture.toString() << std::endl;
    }
}

void postureArcInterpolatorTest()
{
    Posture start({ 0, 50, 30 }), end({ 50, 0, 30 });
    PostureArcInterpolator interpolator(start, end, PostureArcInterpolator::Plane::XY, 50, true, 1, 1);
    Posture posture;
    while (!interpolator.getPostureByStep(posture)) {
        std::cout << posture.center << std::endl;
    }
}

int main()
{
    postureLineInterpolatorTest1();
    postureLineInterpolatorTest2();
    postureArcInterpolatorTest();
    return 0;
}
