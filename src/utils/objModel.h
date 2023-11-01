#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>  // For vec2, vec3

#include <shader.h>  // Assuming you have this file from your previous code

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};
struct Texture {
    unsigned int id;
    string type;
    string path;
};
struct Material {
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    GLuint texture;
    GLuint specularMap;
    float shininess;

};

using namespace std;

class Model {
public:
    struct face{
        int vertexIndex[3];
        int texCoordIndex[3];
        int normalIndex[3];
    };
    vector<Texture> textures_loaded;  // to avoid loading textures more than once
    vector<Vertex> vertices;
    vector<face> faces;

    
    string directory;  // directory of the model file

    // Constructor, expects a filepath to a 3D model.
    Model(string const &path) {
        folderPath = path.substr(0, path.find_last_of("/"));
        materialPath = path.substr(0, path.find_last_of(".")) + ".mtl";
        loadModel(path);
        loadMaterial(materialPath, material);
        setupBuffer();
        material.shininess = 108.0f; 
   

    }

    // Draws the model, and thus all its meshes
    void Draw(Shader &shader);

private:
    unsigned int VAO, VBO, EBO;
    Material material;
    string materialPath;
    string folderPath;
    // Loads an OBJ model from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);
    void setupBuffer();
    // Helper functions for parsing vertices, texture coordinates, and faces from the OBJ file
    void parseVertex(const vector<string> &tokens);
    void parseTexCoord(const vector<string> &tokens);
    void parseFace(const vector<string> &tokens);
    void processFaceVertex(const string& token, 
                       const vector<glm::vec3>& tempPositions, 
                       const vector<glm::vec2>& tempTexCoords, 
                       const vector<glm::vec3>& tempNormals,
                       vector<Vertex>& vertices) ;
    // Helper function to split a string by whitespace
    vector<string> split(const string &s, char delimiter);
    void loadMaterial(const std::string& mtlPath, Material& material);
    GLuint loadTexture(const std::string& texturePath);
};

#endif
