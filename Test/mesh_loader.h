#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

//----------------------------------------------------------------------------------------
/**
 * \file    mesh_loader.h
 * \author  Jakub Zimmer
 * \date    2026
 * \brief   Mesh loader from a .obj file
 */
 //----------------------------------------------------------------------------------------

/**
 *@brief Minimal Wavefront .obj loader (positions, normals, UVs). No materials,
 *        no groups — used only for the skybox cubes
 *@param path Path to the .obj file relative to the working directory
 *@param out  Populated with the interleaved vertex array and index array
 *@return true on success, false if the file could not be opened or parsed
 */
struct LoadedVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct LoadedObjMesh {
    std::vector<LoadedVertex> vertices;
    std::vector<unsigned int> indices;
};

void loadObjMesh(const std::string &path, LoadedObjMesh &outMesh);
