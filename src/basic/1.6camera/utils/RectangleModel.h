#pragma once

#include <glad/glad.h>
#include "shader.h"
#include <vector>
#include <chrono>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class RectangleModel
{
public:
    RectangleModel();
    RectangleModel(const Shader& shader);

    ~RectangleModel();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;

    std::vector<unsigned int> texture;


    void setupBuffers();
    void setElements();
    void loadTexture();
    void bindTexture(GLuint& textureId, const char* path);
};
