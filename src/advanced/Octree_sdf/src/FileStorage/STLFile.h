#pragma once

#include "Triangle3D.h"
#include "Vector3D.h"

#include <string>
#include <vector>

class STLFile {
public:
    STLFile() = default;
    virtual ~STLFile() = default;

    enum class Format {
        Ascii,
        Binary,
        Invalid
    };

    static Format checkFormat(const std::string& filename);

    static bool Load(const std::string& filename, std::vector<Triangle3Df>& triangles);
    static bool Load(const std::string& filename, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);
    static void LoadBinary(const std::string& filename, std::vector<Triangle3Df>& triangles);
    static void LoadBinary(const std::string& filename, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);
    static void LoadAscii(const std::string& filename, std::vector<Triangle3Df>& triangles);
    static void LoadAscii(const std::string& filename, std::vector<Vector3Df>& vertices, std::vector<Vector3Df>& normals);

    static void Save(const std::vector<Triangle3Df>& triangles, const std::string& filename);
    static void Save(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::string& filename);
    static void Save1Vertex1Normal(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::string& filename);
    static void Save3Vertices1Normal(const std::vector<Vector3Df>& vertices, const std::vector<Vector3Df>& normals, const std::string& filename);
};
