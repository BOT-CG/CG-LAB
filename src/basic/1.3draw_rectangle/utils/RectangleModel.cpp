#include "RectangleModel.h"
#include <iostream>

const int VERTEX_ATTR_POSITION = 0;
const int NUM_COMPONENTS_PER_VERTEX = 2;

RectangleModel::RectangleModel()
{
    compileShaders();
    setElements();
}

RectangleModel::~RectangleModel()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void RectangleModel::draw()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RectangleModel::compileShaders()
{
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec2 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
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

void RectangleModel::setupBuffers()
{
    float vertices[] = {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,

        -0.5f, 0.5f,
        0.5f, -0.5f,
        0.5f, 0.5f

    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, NUM_COMPONENTS_PER_VERTEX, GL_FLOAT, GL_FALSE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)0);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
}

void RectangleModel::setElements()
{
    float vertices[] = {
        -0.75f, -0.75f,
        0.75f, -0.75f,
        -0.75f, 0.75f,
        0.75f, 0.75f

    };

    int indices[] = {
        0, 1, 2,
        1, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, NUM_COMPONENTS_PER_VERTEX, GL_FLOAT, GL_TRUE, NUM_COMPONENTS_PER_VERTEX * sizeof(float), (void *)0);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
}