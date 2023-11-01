#include "RectangleModel.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const int VERTEX_ATTR_POSITION = 0;
const int NUM_COMPONENTS_PER_VERTEX = 2;

RectangleModel::RectangleModel(const Shader& shader):shader(shader)
{
     
    loadTexture();
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
    glBindVertexArray(VAO);
    // glBindTexture(GL_TEXTURE_2D, texture);
    shader.use();
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // Compute blend ratio using sin function for oscillation effect and normalize to [0,1]
    // Adjust the frequency by dividing milliseconds by a value (e.g., 1000.0 for oscillation every second)
    float blendRatio = 0.5f * (std::sin(milliseconds / 1000.0) + 1.0f);

    shader.setFloat("blendRatio", blendRatio);
    glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RectangleModel::compileShaders()
{
    shader = Shader("shader.vs", "shader.fs");
    return;

}


void RectangleModel::setElements()
{
float vertices[] = {
    // Rectangle 1
    -0.75f, -0.75f, 0.0f, 0.0f,
    -0.1f, -0.75f, 1.0f, 0.0f,
    -0.75f, -0.1f, 0.0f, 1.0f,
    -0.1f, -0.1f, 1.0f, 1.0f,
    
    // Rectangle 2
    0.1f, -0.75f, 0.0f, 0.0f,
    0.75f, -0.75f, 1.0f, 0.0f,
    0.1f, -0.1f, 0.0f, 1.0f,
    0.75f, -0.1f, 1.0f, 1.0f,
    
    // Rectangle 3
    -0.75f, 0.1f, 0.0f, 0.0f,
    -0.1f, 0.1f, 1.0f, 0.0f,
    -0.75f, 0.75f, 0.0f, 1.0f,
    -0.1f, 0.75f, 1.0f, 1.0f,
    
    // Rectangle 4
    0.1f, 0.1f, 0.0f, 0.0f,
    0.75f, 0.1f, 1.0f, 0.0f,
    0.1f, 0.75f, 0.0f, 1.0f,
    0.75f, 0.75f, 1.0f, 1.0f
};

int indices[] = {
    // Rectangle 1
    0, 1, 2,
    1, 2, 3,
    
    // Rectangle 2
    4, 5, 6,
    5, 6, 7,
    
    // Rectangle 3
    8, 9, 10,
    9, 10, 11,
    
    // Rectangle 4
    12, 13, 14,
    13, 14, 15
};


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}
void RectangleModel::loadTexture(){
    std::vector<std::string> path = {"logo.png","tex.png"};
    texture.resize(path.size());
    for(int i = 0; i < path.size(); i++){
        bindTexture(texture[i], path[i].c_str());
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    shader.use();
    shader.setInt("texture0", 0);  // Texture unit 0
    shader.setInt("texture1", 1);  // Texture unit 1
}

void RectangleModel::bindTexture(GLuint& textureId, const char* path){
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    stbi_set_flip_vertically_on_load(true);  
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 3)
            format = GL_RGB;
        else
            format = GL_RED;  // or handle other formats as needed
        
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}