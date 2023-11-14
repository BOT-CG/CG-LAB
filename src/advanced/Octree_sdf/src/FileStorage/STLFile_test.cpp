#include "STLFile.h"

#include "Triangle3D.h"
#include "Vector3D.h"

#include <fstream>
#include <iostream>
#include <vector>

static const std::vector<Vector3Df> vertices = {
    Vector3Df(0, 0, 0),
    Vector3Df(-1, 0, 0),
    Vector3Df(0, -1, 0),
    Vector3Df(0, 0, 0),
    Vector3Df(-1, 0, 0),
    Vector3Df(0, 0, 1),
    Vector3Df(0, 0, 0),
    Vector3Df(0, -1, 0),
    Vector3Df(0, 0, 1),
    Vector3Df(-1, 0, 0),
    Vector3Df(0, -1, 0),
    Vector3Df(0, 0, 1),
};
static const std::vector<Vector3Df> normals1 = {
    Vector3Df(0, 0, -1).normalize(),
    Vector3Df(0, 1, 0).normalize(),
    Vector3Df(1, 0, 0).normalize(),
    Vector3Df(-1, -1, 1).normalize(),
};
static const std::vector<Vector3Df> normals2 = {
    Vector3Df(0, 0, -1),
    Vector3Df(0, 0, -1),
    Vector3Df(0, 0, -1),
    Vector3Df(0, 1, 0),
    Vector3Df(0, 1, 0),
    Vector3Df(0, 1, 0),
    Vector3Df(1, 0, 0),
    Vector3Df(1, 0, 0),
    Vector3Df(1, 0, 0),
    Vector3Df(-1, -1, 1),
    Vector3Df(-1, -1, 1),
    Vector3Df(-1, -1, 1),
};
static const std::vector<Triangle3Df> triangles = {
    Triangle3Df(vertices[0], vertices[1], vertices[2], normals1[0]),
    Triangle3Df(vertices[3], vertices[4], vertices[5], normals1[1]),
    Triangle3Df(vertices[6], vertices[7], vertices[8], normals1[2]),
    Triangle3Df(vertices[9], vertices[10], vertices[11], normals1[3]),
};

void testSave1()
{
    STLFile::Save(vertices, normals1, "test1.stl");
}
void testSave2()
{
    STLFile::Save(vertices, normals2, "test2.stl");
}
void testSave3()
{
    STLFile::Save(triangles, "test3.stl");
}
void testLoad1()
{
    std::vector<Vector3Df> verticesFromFile;
    std::vector<Vector3Df> normalsFromFile;
    STLFile::Load("test2.stl", verticesFromFile, normalsFromFile);

    bool isVerticesEqual = std::equal(vertices.begin(), vertices.end(), verticesFromFile.begin(), [](const Vector3Df& v1, const Vector3Df& v2) {
        return v1.equal(v2);
    });
    bool isNormalsEqual = std::equal(normals1.begin(), normals1.end(), normalsFromFile.begin(), [](const Vector3Df& v1, const Vector3Df& v2) {
        return v1.equal(v2);
    });
    std::cout << "isVerticesEqual: " << std::boolalpha << isVerticesEqual << std::endl;
    std::cout << "isNormalsEqual: " << std::boolalpha << isNormalsEqual << std::endl;
}
void testLoad2()
{
    std::vector<Triangle3Df> trianglesFromFile;
    STLFile::Load("test3.stl", trianglesFromFile);

    bool isTrianglesEqual = std::equal(triangles.begin(), triangles.end(), trianglesFromFile.begin(), trianglesFromFile.end(), [](const Triangle3Df& t1, const Triangle3Df& t2) {
        return t1.equal(t2);
    });
    std::cout << "isTrianglesEqual: " << std::boolalpha << isTrianglesEqual << std::endl;
}

int main()
{
    testSave1();
    testSave2();
    testSave3();
    testLoad1();
    testLoad2();
    return 0;
}
