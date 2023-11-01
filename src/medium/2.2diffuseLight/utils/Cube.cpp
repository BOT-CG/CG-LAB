#include "Cube.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const int VERTEX_ATTR_POSITION = 0;
const int NUM_COMPONENTS_PER_VERTEX = 2;

Cube::Cube(GLFWWindowFactory* window):window(window)
{

    shader = Shader("shader.vs", "shader.fs");
    lightShader= (Shader("lightShader.vs", "lightShader.fs"));
    loadTexture();
    setElements();

}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Cube::draw()
{
    std::vector<std::vector<float>> lights = {{2.0f,1.5f,-1.55f,1.0f}};
    shader.use();

    glBindVertexArray(VAO); 
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // Compute blend ratio using sin function for oscillation effect and normalize to [0,1]
    // Adjust the frequency by dividing milliseconds by a value (e.g., 1000.0 for oscillation every second)
    // float blendRatio = 0.5f * (std::sin(milliseconds / 1000.0) + 1.0f);
    float blendRatio = 0.5f ;  
    shader.setFloat("blendRatio", blendRatio);
    shader.setVec3("lightColor", glm::vec3(1.0f,0.0f,0.5f));
    auto view = window->getViewMatrix();
    auto projection = glm::perspective(glm::radians(GLFWWindowFactory::camera.Zoom), (float)GLFWWindowFactory::SCR_WIDTH / (float)GLFWWindowFactory::SCR_HEIGHT, 0.1f, 100.0f);

    
    // pass projection matrix to shader (note that in this case it could change every frame)
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f,0.0f,-3.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f,0.0f,0.0f));
     
    shader.setMat4("model", model);
    shader.setMat4("projection", window->getProjectionMatrix());
    shader.setMat4("view", window->getViewMatrix());
    shader.setVec3("lightColor", glm::vec3(1.0f,0.0f,0.5f));
    shader.setVec3("lightPos", lights[0][0],lights[0][1],lights[0][2]);
   
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


    lightShader.use();
    for(auto light : lights){
        auto  model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(light[0],light[1],light[2]));
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", window->getViewMatrix());
        lightShader.setMat4("projection", window->getProjectionMatrix());
        lightShader.setVec3("lightColor", glm::vec3(1.0f,0.0f,0.5f));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
   

    // glDrawArrays(GL_TRIANGLES, 0, 6);
}




void Cube::setElements()
{
    float vertices[] = {
        // Front
        -0.75f, -0.75f,0.0f, 0.0f, 0.0f, 0.0f, 0.0f,1.0f,  // Bottom-left vertex
        0.75f, -0.75f,0.0f, 1.0f, 0.0f,  0.0f, 0.0f,1.0f, // Bottom-right vertex
        -0.75f, 0.75f,0.0f, 0.0f, 1.0f, 0.0f, 0.0f,1.0f,  // Top-left vertex
        0.75f, 0.75f,0.0f, 1.0f, 1.0f,  0.0f, 0.0f,1.0f,  // Top-right vertex

        // Back
        0.75f, -0.75f, -1.5f, 0.0f, 0.0f,  0.0f, 0.0f,-1.0f,// Bottom-left vertex
        -0.75f, -0.75f, -1.5f, 1.0f, 0.0f, 0.0f, 0.0f,-1.0f,  // Bottom-right vertex
        0.75f, 0.75f, -1.5f, 0.0f, 1.0f,   0.0f, 0.0f,-1.0f,// Top-left vertex
        -0.75f, 0.75f, -1.5f, 1.0f, 1.0f,  0.0f, 0.0f,-1.0f,  // Top-right vertex

        // Left
        -0.75f, -0.75f, -1.5f, 0.0f, 0.0f, -1.0f,0.0f,0.0f,  // Bottom-left vertex
        -0.75f, -0.75f, 0.0f, 1.0f, 0.0f, -1.0f,0.0f,0.0f,  // Bottom-right vertex
        -0.75f, 0.75f, -1.5f, 0.0f, 1.0f, -1.0f,0.0f,0.0f,  // Top-left vertex
        -0.75f, 0.75f, 0.0f, 1.0f, 1.0f,  -1.0f,0.0f,0.0f,  // Top-right vertex

        // Right
        0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 1.0f,0.0f,0.0f, // Bottom-left vertex
        0.75f, -0.75f, -1.5f, 1.0f, 0.0f,1.0f,0.0f,0.0f,   // Bottom-right vertex
        0.75f, 0.75f, 0.0f, 0.0f, 1.0f,  1.0f,0.0f,0.0f, // Top-left vertex
        0.75f, 0.75f, -1.5f, 1.0f, 1.0f, 1.0f,0.0f,0.0f,   // Top-right vertex

        // Top
        -0.75f, 0.75f, 0.0f, 0.0f, 0.0f,  0.0f,1.0f,0.0f,// Bottom-left vertex
        0.75f, 0.75f, 0.0f, 1.0f, 0.0f,  0.0f,1.0f,0.0f, // Bottom-right vertex
        -0.75f, 0.75f, -1.5f, 0.0f, 1.0f,  0.0f,1.0f,0.0f, // Top-left vertex
        0.75f, 0.75f, -1.5f, 1.0f, 1.0f,   0.0f,1.0f,0.0f, // Top-right vertex

        // Bottom
        -0.75f, -0.75f, -1.5f, 0.0f, 0.0f, 0.0f,-1.0f,0.0f, // Bottom-left vertex
        0.75f, -0.75f, -1.5f, 1.0f, 0.0f,  0.0f,-1.0f,0.0f, // Bottom-right vertex
        -0.75f, -0.75f, 0.0f, 0.0f, 1.0f,  0.0f,-1.0f,0.0f, // Top-left vertex
        0.75f, -0.75f, 0.0f, 1.0f, 1.0f ,  0.0f,-1.0f,0.0f,// Top-right vertex

 
    };

    int indices[] = {
        0, 1, 2,
        2, 1, 3,

        4, 5, 6,
        6, 5, 7,

        8, 9, 10,
        10, 9, 11,

        12, 13, 14,
        14, 13, 15,

        16, 17, 18,
        18, 17, 19,

        20, 21, 22,
        22, 21, 23

    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5*sizeof(float)));
    glEnableVertexAttribArray(2);


    glBindVertexArray(0);

}
void Cube::loadTexture(){
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

void Cube::bindTexture(GLuint& textureId, const char* path){
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