#pragma once
#include "pgr.h"
#include "constants.h"
#include <vector>

//----------------------------------------------------------------------------------------
/**
 * \file    initalize.h
 * \author  Jakub Zimmer
 * \date    2026
 * \brief   Initialization functions and program structs
 */
 //----------------------------------------------------------------------------------------

//Geometry buffers for a renderable mesh (VBO, EBO, VAO + counts)
struct MeshGeometry {
    GLuint vertexBufferObject = 0;
    GLuint elementBufferObject = 0;
    GLuint vertexArrayObject = 0;
    GLsizei vertexCount = 0;
    GLsizei indexCount = 0;
};

//Lighting and texture parameters used by the fragment shader for a single object
struct Material {
    //default material for a car
    glm::vec3 ambient = glm::vec3(0.25f, 0.20f, 0.08f);
    glm::vec3 diffuse = glm::vec3(0.8f, 0.45f, 0.05f);
    glm::vec3 specular = glm::vec3(0.4f, 0.4f, 0.4f);
    float shininess = 30.0f;

    bool useTexture = false;
};

//A renderable instance: mesh + transform + material + per-object effects (water, sprite sheet)
struct Object {
    int meshId = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);

    GLuint texture = 0;
    Material material;

    //only for water texture to move
    float waterTex = 0.0f;

    //for sprite sheet film
    glm::vec4 spriteSheet = glm::vec4(0.0f);

    //Picking state — set when an object is reddened, restored on R+click
    glm::vec3 originalDiffuse = glm::vec3(0.0f);
    bool isPicked = false;
};

//First-person walker position, controlled by WASD via movePerson()
struct Person {
    glm::vec3 position = glm::vec3(0.0f);
};

//Axis-aligned bounding box used by movePerson() for player-vs-world collision
struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

/**
 *@brief Camera state for free, two static and one dynamic mode
 *
 *Tracks position/orientation, the four mode flags, and the transition snapshot used
 *for the linear interpolation between modes (no-jump switching)
 */
struct Camera {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 front    = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f);
    float yaw = -90.0f, pitch = 0.0f;

    bool toggleFree = true;
    bool toggleStatic1 = false;
    bool toggleStatic2 = false;
    bool toggleDynamic1 = false;
    bool toggleDynamic2 = false;

    //Drag-look offsets applied to static cameras while right-mouse is dragged
    float staticYawOffset = 0.0f;
    float staticPitchOffset = 0.0f;

    //transition (linear interpolation between modes)
    bool  transitioning = false;
    //seconds, set from app.gameState.elapsedTime
    float transitionStart = 0.0f;
    //1s feels good; tune to taste
    float transitionDuration = 1.0f; 
    //snapshot at the moment the user pressed 1/2/3/4
    glm::vec3 fromPosition;
    glm::vec3 fromFront;
};

//Per-frame scene flags, keyboard state and elapsed time
struct SceneState {
    bool sceneOver = false;
    bool keyMap[KEYS_COUNT] = {};
    float elapsedTime = 0.0f;
    
    bool toggle_flight = false;

    //Right-mouse drag state for camera panning in static modes
    bool dragLooking = false;
    int dragStartX = 0;
    int dragStartY = 0;
    float dragStartYaw = 0.0f;
    float dragStartPitch = 0.0f;
};

//Cached uniform locations and program handle for the main object shader
struct SCommonShaderProgram {
    GLuint program = 0;

    GLint PVMmatrixLocation = -1;
    GLint VmatrixLocation = -1;
    GLint MmatrixLocation = -1;
    GLint PmatrixLocation = -1;
    GLint normalMatrixLocation = -1;

    GLint diffuseLocation = -1;
    GLint ambientLocation = -1;
    GLint specularLocation = -1;
    GLint shininessLocation = -1;

    GLint useTextureLocation = -1; 
    GLint texSamplerLocation = -1;

    GLint fogDensityLocation = -1;
    GLint fogColorLocation = -1;

    GLint sunParamLocation = -1;
};

//Cached uniform locations and program handle for the skybox shader
struct SkyboxFarPlaneShaderProgram {
    int curSkyboxId = 0;
    //identifier for the shader program
    GLuint program = 0;
    //uniforms locations
    GLint viewMatrixLocation = -1;
    GLint projectionMatrixLocation = -1;
    GLint samplerLocation = -1;
};

//Root scene state: meshes, objects, player, camera, shaders, skybox and colliders
struct AppState {
    std::vector<MeshGeometry> meshes;
    std::vector<Object> objects;
    Person person;
    Camera camera;
    SceneState gameState;
    SCommonShaderProgram shaderProgram;

    SkyboxFarPlaneShaderProgram skyboxProgram;
    MeshGeometry skyboxGeometries[2];
    GLuint skyboxTextures[2] = { 0, 0 };

    float fogDensity = 0.01f;

    std::vector<AABB> colliders;
};

extern AppState app;

/**
 *@brief Load runtime-tunable values (move speed, sprint speed, sensitivity,
 *       camera elevation max, fog density, lap time period) from
 *       data/config.txt. File format is "key value" pairs, one per line;
 *       unknown keys are silently ignored. Aborts via pgr::dieWithError
 *       if the file cannot be opened
 */
void initConstants();

//Compile and link the main object shader and cache its uniform locations
void initalizeSkyboxShaderProgram();

//Compile and link the skybox shader and cache its uniform locations
void initializeShaderPrograms();

/**
 *@brief Reset per-scene runtime flags and the key map
 *@param state Reference to the SceneState to reset
 */
void initCallBacks(SceneState& state);

/**
 *@brief Upload a hardcoded mesh (vertex array + index array) into a VBO/EBO/VAO
 *@param geometry            Mesh handles to populate
 *@param n_attribs_per_vertex Floats per vertex (8 for pos3+normal3+uv2)
 *@param n_vertices          Vertex count
 *@param n_triangles         Triangle count (index count = n_triangles * 3)
 *@param vertices            Pointer to the interleaved vertex array
 *@param triangles           Pointer to the index array
 */
void initObjectMesh(MeshGeometry& geometry, const int n_attribs_per_vertex, const int n_vertices, const int n_triangles, const float* vertices, const unsigned int* triangles);

//Load both skybox cube meshes and their textures from disk
void initSkybox();

//Build the full scene: meshes, objects, lighting setup and collider AABBs
void initScene();

//Release all GPU resources owned by app.meshes and app.skybox*
void cleanUpScene();

/**
 *@brief Release the GPU buffers (VBO/EBO/VAO) of a single mesh
 *@param geometry Mesh to free; handles are zeroed after deletion
 */
void cleanUpModel(MeshGeometry& geometry);