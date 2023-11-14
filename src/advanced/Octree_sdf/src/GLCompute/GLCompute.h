#pragma once

#include "Vector3D.h"

#include <vector>

class GLComputeInstance;

class GLCompute {
public:
    static void computeSTL2UDF(const std::vector<float>& triangleVertices, std::vector<PointAndDistance>& pointsAndDistances);
    static void computeSTL2SDF(const std::vector<float>& triangleVertices, const std::vector<float>& triangleNormals, std::vector<PointAndDistance>& pointsAndDistances);

protected:
    GLCompute() = default;
    ~GLCompute() = default;

private:
    static GLComputeInstance* instance;
    static GLComputeInstance* getInstance();
};
