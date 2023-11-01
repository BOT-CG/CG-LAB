#pragma once

#include <glad/glad.h>
#include "shader.h"

class RectangleModel
{
public:
    RectangleModel(const Shader& shader);
    ~RectangleModel();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;

    unsigned int shaderProgram;

    void compileShaders();
    void setupBuffers();
    void setElements();
};
