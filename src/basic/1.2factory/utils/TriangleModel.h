#pragma once

#include <glad/glad.h>

class TriangleModel
{
public:
    TriangleModel();
    ~TriangleModel();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shaderProgram;

    void compileShaders();
    void setupBuffers();
};
