#include "mesh_loader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "pgr.h"

//helped with ai, since I have no idea how .obj files 
// are organized and what's important and what not
void loadObjMesh(const std::string &path, LoadedObjMesh& outMesh) {
    
    outMesh.vertices.clear();
    outMesh.indices.clear();
    
    std::ifstream file(path);

    if (!file.is_open()) {
        pgr::dieWithError("Could not load an .obj model\n");
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    std::unordered_map<std::string, unsigned int> createdVertices;

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        //from line 'v 1.0 2.0 3.0' there will be 'v' in type variable
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        //load positions
        if (type == "v") {
            glm::vec3 position;
            ss >> position.x >> position.y >> position.z;
            positions.push_back(position);
        }
        //load normals
        else if (type == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        //load UV, the texture coordinate
        else if (type == "vt") {
            glm::vec2 texCoord;
            ss >> texCoord.x >> texCoord.y;
            //[0, 0] is bottom left
            texCoord.y = 1.0f - texCoord.y;
            texCoords.push_back(texCoord);
        }

        //load face / area
        else if (type == "f") {
            std::vector<unsigned int> faceIndices;
            std::string token;

            while (ss >> token) {
                //check if token already exists
                auto existing = createdVertices.find(token);

                if (existing != createdVertices.end()) {
                    faceIndices.push_back(existing->second);
                    continue;
                }

                int positionIndex = -1;
                int texCoordIndex = -1;
                int normalIndex = -1;

                size_t firstSlash = token.find('/');
                //token without the slash, string number, 12
                if (firstSlash == std::string::npos) {
                    //convert to int, -1 casue OBJ indexes from 1
                    positionIndex = std::stoi(token) - 1;
                }
                else {
                    // token with slash, multiple numbers, like 12/4/2
                    //get rid off the other slashes, 12
                    std::string positionPart = token.substr(0, firstSlash);
                    if (!positionPart.empty()) {
                        positionIndex = std::stoi(positionPart) - 1;
                    }
                    //search for a second number after slash, 12/4
                    size_t secondSlash = token.find('/', firstSlash + 1);
                    if (secondSlash == std::string::npos) {
                        std::string texCoordPart = token.substr(firstSlash + 1);
                        if (!texCoordPart.empty()) {
                            texCoordIndex = std::stoi(texCoordPart) - 1;
                        }
                    }
                    else {
                        //for token 12/4/9, get texcoordpart = 4, normal part = 9
                        std::string texCoordPart = token.substr(firstSlash + 1, secondSlash - firstSlash - 1);

                        std::string normalPart = token.substr(secondSlash + 1);

                        if (!texCoordPart.empty()) {
                            texCoordIndex = std::stoi(texCoordPart) - 1;
                        }

                        if (!normalPart.empty()) {
                            normalIndex = std::stoi(normalPart) - 1;
                        }
                    }
                }
                LoadedVertex vertex{};

                //check and set position
                if (positionIndex >= 0 && positionIndex < static_cast<int>(positions.size())) {
                    vertex.position = positions[positionIndex];
                }
                else {
                    pgr::dieWithError("VROOM: invalid position index in\n");
                }

                //check and set normal position
                if (normalIndex >= 0 && normalIndex < static_cast<int>(normals.size())) {
                    vertex.normal = normals[normalIndex];
                }
                else {
                    //if vertex position valid set vertex normal to this (default)
                    vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
                }

                //check and set UV
                if (texCoordIndex >= 0 && texCoordIndex < static_cast<int>(texCoords.size())) {
                    vertex.texCoord = texCoords[texCoordIndex];
                }
                else {
                    //once again the default value for any screw up
                    vertex.texCoord = glm::vec2(0.0f, 0.0f);
                }

                
                unsigned int newVertexIndex = static_cast<unsigned int>(outMesh.vertices.size());
                //add vertex to the return Mesh
                outMesh.vertices.push_back(vertex);
                //save into map the token / index duo
                createdVertices[token] = newVertexIndex;

                faceIndices.push_back(newVertexIndex);
            }

            //Face with less than 3 vertices can't be a triangle
            if (faceIndices.size() < 3) {
                continue;
            }

            //Make it into triangle
            for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
                outMesh.indices.push_back(faceIndices[0]);
                outMesh.indices.push_back(faceIndices[i]);
                outMesh.indices.push_back(faceIndices[i + 1]);
            }
        }
    }
    //just to make sure
    std::cout << "Loaded OBJ: " << path << std::endl;
    std::cout << "Vertices: " << outMesh.vertices.size() << std::endl;
    std::cout << "Indices: " << outMesh.indices.size() << std::endl;
    std::cout << "Triangles: " << outMesh.indices.size() / 3 << std::endl;
}