#include "objModel.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


void Model::loadModel(string const &path){
    // Read file
    ifstream file(path);
    if(!file.is_open()){
        cout << "ERROR::MODEL::FILE_NOT_SUCCESFULLY_READ" << endl;
        return;
    }
    string line;
    vector<glm::vec3> tempPositions;
    vector<glm::vec3> tempNormals;
    vector<glm::vec2> tempTexCoords;

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        ss >> token;

        if (token == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            tempPositions.push_back(position);
        }
        else if (token == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            texCoord.y = 1.0f - texCoord.y; // flip y-coord
            tempTexCoords.push_back(texCoord);
        }
        else if (token == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            tempNormals.push_back(normal);
        }else if (token == "f") {
            auto faceTokens = split(line, ' ');
            if(faceTokens.size() == 4){
                processFaceVertex(faceTokens[1], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[2], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[3], tempPositions, tempTexCoords, tempNormals, vertices);
            } 
            if(faceTokens.size()==5){ 
                processFaceVertex(faceTokens[1], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[2], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[4], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[4], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[2], tempPositions, tempTexCoords, tempNormals, vertices);
                processFaceVertex(faceTokens[3], tempPositions, tempTexCoords, tempNormals, vertices);
            }
        }
    }
    file.close();
}
void Model::processFaceVertex(const string& token, 
                       const vector<glm::vec3>& tempPositions, 
                       const vector<glm::vec2>& tempTexCoords, 
                       const vector<glm::vec3>& tempNormals, 
                       vector<Vertex>& vertices) {
    vector<string> subTokens = split(token, '/');
    int posIndex = stoi(subTokens[0]) - 1;
    int texIndex = stoi(subTokens[1]) - 1;
    int normIndex = stoi(subTokens[2]) - 1;

    Vertex vertex;
    vertex.Position = tempPositions[posIndex];
    vertex.TexCoords = tempTexCoords[texIndex];
    vertex.Normal = tempNormals[normIndex];
    vertices.push_back(vertex);
}

vector<string> Model::split(const string& s, char delimiter){
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while(getline(tokenStream, token, delimiter)){
        tokens.push_back(token);
    }
    return tokens;
}

void Model::setupBuffer(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // vertex normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);	
    // vertex texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);	
    // set the vertex attribute pointers
    // vertex Positions
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);  // Unbind the VAO
}

void Model::Draw(Shader &shader) {
    // Use the specified shader program
    shader.use();

    // Set the vec3 members
    shader.setFloat("material.shininess", material.shininess);
    shader.setVec3("material.ambient", material.ambient);
    shader.setVec3("material.diffuse", material.diffuse);
    shader.setVec3("material.specular", material.specular);

    // Set the texture sampler2D
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.texture);
    shader.setInt("material.texture", 0);

    // Set the normalMap
    shader.setBool("material.sampleNormalMap",false);
    // code to implement normal map
    
    // Set the specularMap
    if(material.specularMap != 0){
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, material.specularMap);
        shader.setInt("material.specularMap", 2);
    }

    
    // Bind the Vertex Array Object of this model
    glBindVertexArray(VAO);

    // Draw the vertices as triangles
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    // Optionally, unbind the VAO (not strictly necessary but can help avoid bugs in some situations)
    glBindVertexArray(0);
}

void Model::loadMaterial(const std::string& mtlPath, Material& material) {
    std::ifstream file(mtlPath);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << mtlPath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "newmtl") {
            ss >> material.name;
        }
        else if (token == "Ka") {
            ss >> material.ambient.r >> material.ambient.g >> material.ambient.b;
        }
        else if (token == "Kd") {
            ss >> material.diffuse.r >> material.diffuse.g >> material.diffuse.b;
        }
        else if (token == "Ks") {
            ss >> material.specular.r >> material.specular.g >> material.specular.b;
        }
        else if (token == "map_Kd") {
            std::string texturePath;
            ss >> texturePath;

            texturePath = folderPath + "/" + texturePath;
            // Load the texture using stb_image
            material.texture = loadTexture(texturePath);
        }
        else if (token == "map_Ks") {
            std::string texturePath;
            ss >> texturePath;

            texturePath = folderPath + "/" + texturePath;
            // Load the texture using stb_image
            material.specularMap = loadTexture(texturePath);
        }
    }

    file.close();
}


GLuint Model::loadTexture(const std::string& texturePath) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 4)
            format = GL_RGBA;
        else if (nrChannels == 3)
            format = GL_RGB;
        else
            format = GL_RED;  // or handle other formats as needed

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);  // Use format variable here

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        return texture;
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath << std::endl;
        stbi_image_free(data);
        return 0;
    }
}
