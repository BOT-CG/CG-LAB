#include "TriangleModel.h"
#include <iostream>

const int VERTEX_ATTR_POSITION = 0;
const int NUM_COMPONENTS_PER_VERTEX = 3;

TriangleModel::TriangleModel()
{
    compileShaders();
    setupBuffers();
}

TriangleModel::~TriangleModel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void TriangleModel::draw()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TriangleModel::compileShaders()
{
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(0.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void TriangleModel::setupBuffers()
{
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,

        1.0f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.25f, 0.0f, 0.0f

    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, NUM_COMPONENTS_PER_VERTEX, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)0);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
}
