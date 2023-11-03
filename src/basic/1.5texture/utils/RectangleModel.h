#pragma once

#include <glad/glad.h>
#include "shader.h"
#include <vector>
#include <chrono>
#include <cmath>
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
    std::vector<unsigned int> texture;


    void compileShaders();
    void setupBuffers();
    void setElements();
    void loadTexture();
    void bindTexture(GLuint& textureId, const char* path);
};
