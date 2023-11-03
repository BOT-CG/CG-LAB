#pragma once
#include "windowFactory.h"

#include <glad/glad.h>
#include "shader.h"
#include <vector>
#include <chrono>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Cube
{
public:
    Cube();
    Cube( GLFWWindowFactory* window);

    ~Cube();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    Shader shader;
    Shader lightShader;
    std::vector<unsigned int> texture;
    GLFWWindowFactory* window;

    void setupBuffers();
    void setElements();
    void loadTexture();
    void bindTexture(GLuint& textureId, const char* path);
};
