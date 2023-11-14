#include "Triangle3D.h"
#include "Vector3D.h"

#include <iostream>

void testTriangle(const Triangle3Df& t, const Vector3Df& v)
{
    std::cout << "t: " << t << std::endl;
    std::cout << "v: " << v << std::endl;
    std::cout << "distanceAsPlane: " << t.distanceAsPlane(v) << std::endl;
    std::cout << "isSameSideAsNormal: " << std::boolalpha << t.isSameSideAsNormal(v) << std::endl;
    std::cout << "projection: " << t.projection(v) << std::endl;
    std::cout << "isInside(projection(v)): " << std::boolalpha << t.isInside(t.projection(v)) << std::endl;
    std::cout << "distance: " << t.distance(v) << std::endl;
}

int main()
{
    testTriangle({ Vector3Df(1.0f, 0, 0), Vector3Df(0, 1.0f, 0), Vector3Df(0, 0, 1.0f), Vector3Df(1.0f, 1.0f, 1.0f) }, Vector3Df(0, 0, 0));
    testTriangle({ Vector3Df(1.0f, 0, 0), Vector3Df(0, 1.0f, 0), Vector3Df(0, 0, 1.0f), Vector3Df(1.0f, 1.0f, 1.0f) }, Vector3Df(0, 0, 1.0f));
    testTriangle({ Vector3Df(1.0f, 0, 0), Vector3Df(0, 1.0f, 0), Vector3Df(0, 0, 1.0f), Vector3Df(1.0f, 1.0f, 1.0f) }, Vector3Df(0, 0, 2.0f));
    return 0;
}
