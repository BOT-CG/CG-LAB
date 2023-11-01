#pragma once

#include <glad/glad.h>

class RectangleModel
{
public:
    RectangleModel();
    ~RectangleModel();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int shaderProgram;

    void compileShaders();
    void setupBuffers();
    void setElements();
};
