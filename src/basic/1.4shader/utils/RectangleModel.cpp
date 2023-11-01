#include "RectangleModel.h"
#include <iostream>

const int VERTEX_ATTR_POSITION = 0;
const int NUM_COMPONENTS_PER_VERTEX = 2;

RectangleModel::RectangleModel(const Shader& shader):shader(shader)
{
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
    // glUseProgram(shaderProgram);
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RectangleModel::compileShaders()
{
    shader = Shader("shader.vs", "shader.fs");
    return;

    // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // glCompileShader(vertexShader);

    // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);

    // shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);

    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);
}


void RectangleModel::setElements()
{
    float vertices[] = {
        -0.75f, -0.75f, 1.0f, 0.0f, 0.0f,
        0.75f, -0.75f, 1.0f, 0.5f, 0.0f,
        -0.75f, 0.75f, 1.0f, 0.0f, 1.0f,
        0.75f, 0.75f, 0.0f,1.0f,0.0f,
    };

    int indices[] = {
        0, 1, 2,
        1, 2, 3
        };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2*sizeof(float)));
    glEnableVertexAttribArray(1);
}