#include "pgr.h"
#include "initialize.h"
#include "constants.h"
#include "data_models.h"
#include "mesh_loader.h"

#include <fstream>
#include <sstream>
#include <iostream>

AppState app{};

void initConstants() {
	//read config.txt
	std::ifstream file("data/config.txt");
	if (!file.is_open()) {
		std::cerr << "Warning: could not open data/config.txt — keeping current values\n";
		return;
	}


	std::string key;
	float value;

	while (file >> key >> value) {
		if (key == "move_speed") {
			MOVE_SPEED = value;
		}
		else if (key == "sprint_speed") {
			SPRINT_SPEED = value;
		}
		else if (key == "sensitivity") {
			SENSITIVITY = value;
		}
		else if (key == "camera_elevation_max") {
			CAMERA_ELEVATION_MAX = value;
		}
		else if (key == "fog_density") {
			if (app.shaderProgram.program != 0) {
				glUseProgram(app.shaderProgram.program);
				glUniform1f(app.shaderProgram.fogDensityLocation, value);
			}
		}
		else if (key == "lap_time_period") {
			LAP_TIME_PERIOD = value;
		}
	}
}

void initalizeSkyboxShaderProgram() {
	std::vector<GLuint> shaders;
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "skybox.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "skybox.frag"));

	app.skyboxProgram.program = pgr::createProgram(shaders);

	glUseProgram(app.skyboxProgram.program);

	app.skyboxProgram.viewMatrixLocation = glGetUniformLocation(app.skyboxProgram.program, "view");
	app.skyboxProgram.projectionMatrixLocation = glGetUniformLocation(app.skyboxProgram.program, "projection");

	app.skyboxProgram.samplerLocation = glGetUniformLocation(app.skyboxProgram.program, "skyboxSampler");
	glUniform1i(app.skyboxProgram.samplerLocation, 0);

	glUseProgram(0);
}

void initializeShaderPrograms() {
	std::vector<GLuint> shaders;
	shaders.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "vertex_base.vert"));
	shaders.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fragment_base.frag"));

	app.shaderProgram.program = pgr::createProgram(shaders);

	glUseProgram(app.shaderProgram.program);

	app.shaderProgram.MmatrixLocation = glGetUniformLocation(app.shaderProgram.program, "model");
	app.shaderProgram.VmatrixLocation = glGetUniformLocation(app.shaderProgram.program, "view");
	app.shaderProgram.PmatrixLocation = glGetUniformLocation(app.shaderProgram.program, "projection");

	app.shaderProgram.ambientLocation = glGetUniformLocation(app.shaderProgram.program, "material.ambient");
	app.shaderProgram.diffuseLocation = glGetUniformLocation(app.shaderProgram.program, "material.diffuse");
	app.shaderProgram.specularLocation = glGetUniformLocation(app.shaderProgram.program, "material.specular");
	app.shaderProgram.shininessLocation = glGetUniformLocation(app.shaderProgram.program, "material.shininess");

	app.shaderProgram.texSamplerLocation = glGetUniformLocation(app.shaderProgram.program, "texSampler");
	app.shaderProgram.useTextureLocation = glGetUniformLocation(app.shaderProgram.program, "material.useTexture");
	glUniform1i(app.shaderProgram.texSamplerLocation, 0);

	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "globalAmbient"), 0.15, 0.15, 0.18);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "sunDirection"), -0.3f, 1.0f, 0.4f);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "sunColor"), 0.7f * 1.0f, 0.7f * 0.95f, 0.7f * 0.85f);

	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "spotLightPosition"), 0.0f, 3.0f, 69.0f);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "spotLightDirection"), 0.0, -1.0, 0.0);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "spotColor"), 4.0, 3.0, 1.5);

	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "pointLightPosition"), 8.0, 10.0, 16.0);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "pointLightColor"), 2.0, 1.0, 0.4);
	glUniform3f(glGetUniformLocation(app.shaderProgram.program, "pointLightAttenuation"), 0.0, 1.0, 0.0);

	glUniform1f(glGetUniformLocation(app.shaderProgram.program, "timeElapsed"), app.gameState.elapsedTime);

	app.shaderProgram.fogDensityLocation = glGetUniformLocation(app.shaderProgram.program, "fogDensity");
	app.shaderProgram.fogColorLocation = glGetUniformLocation(app.shaderProgram.program, "fogColor");

	app.shaderProgram.sunParamLocation = glGetUniformLocation(app.shaderProgram.program, "sunParam");

	glUniform1f(app.shaderProgram.fogDensityLocation, app.fogDensity);
	glUniform3f(app.shaderProgram.fogColorLocation, 0.7f, 0.7f, 0.7f);
	glUniform1f(app.shaderProgram.sunParamLocation, 1.5f);

	glUseProgram(0);

}

void initCallBacks(SceneState& state) {
    state.sceneOver = false;

    for (size_t i = 0; i < KEYS_COUNT; i++) {
        state.keyMap[i] = false;
    }

    state.elapsedTime = 0.0f;
}

void initObjectMesh(MeshGeometry &geometry, const int n_attribs_per_vertex, const int n_vertices, const int n_triangles, const float *vertices, const unsigned int *triangles) {
	glGenVertexArrays(1, &geometry.vertexArrayObject);
	glBindVertexArray(geometry.vertexArrayObject);

	glGenBuffers(1, &geometry.vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, n_vertices * n_attribs_per_vertex * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &geometry.elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_triangles * 3 * sizeof(unsigned), triangles, GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_attribs_per_vertex * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, n_attribs_per_vertex * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//uv
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, n_attribs_per_vertex * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	geometry.vertexCount = n_vertices;
	geometry.indexCount = n_triangles * 3;
}

void initSkybox() {
	const char* objPaths[2] = {
		"data/skybox/skybox2.obj",
		"data/skybox/skybox.obj"
	};
	const char* texPaths[2] = {
		"data/skybox/Scene_-_Root_diffuse.jpeg",
		"data/skybox/Nebula Skybox-UpscaledTopaz-enhance-3.9x.jpeg"
	};

	for (int i = 0; i < 2; i++) {
		LoadedObjMesh loaded;
		loadObjMesh(objPaths[i], loaded);

		std::vector<float> verts;
		verts.reserve(loaded.vertices.size() * 8);
		for (const LoadedVertex& v : loaded.vertices) {
			verts.push_back(v.position.x);
			verts.push_back(v.position.y);
			verts.push_back(v.position.z);
			verts.push_back(v.normal.x);
			verts.push_back(v.normal.y);
			verts.push_back(v.normal.z);
			verts.push_back(v.texCoord.x);
			verts.push_back(v.texCoord.y);
		}

		initObjectMesh(app.skyboxGeometries[i], 8, static_cast<int>(loaded.vertices.size()), static_cast<int>(loaded.indices.size() / 3), verts.data(), loaded.indices.data());
		app.skyboxTextures[i] = pgr::createTexture(texPaths[i]);
	}

	CHECK_GL_ERROR();
}

void initScene() {

	app.meshes.clear();
	app.objects.clear();

	app.camera.position = glm::vec3(0.0f, 0.0f, 73.0f);
	app.camera.front = glm::vec3(0.0f, 0.0f, -1.0f);
	app.camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

	//porsche object
	MeshGeometry porscheMesh;
	initObjectMesh(porscheMesh, porsche_n_attribs_per_vertex, porsche_n_vertices, porsche_n_triangles, porsche_vertices, porsche_triangles);
	app.meshes.push_back(porscheMesh);

	Object porsche;
	GLuint porscheTexture = pgr::createTexture("data/textures/Default_OBJ.007_baseColor.png");
	std::cout << "car texture id: " << porscheTexture << std::endl;
	porsche.texture = porscheTexture;
	porsche.material.useTexture = true;

	porsche.meshId = 0;
	porsche.position = glm::vec3(0.0f, -0.2f, 69.0f);
	porsche.rotation = glm::vec3(0.0, glm::radians(180.0f), glm::radians(180.0f));
	porsche.scale = glm::vec3(0.020);

	porsche.material.ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	porsche.material.diffuse = glm::vec3(0.45f, 0.45f, 0.45f);
	porsche.material.specular = glm::vec3(0.0f, 0.0f, 0.0f);
	porsche.material.shininess = 5.0f;

	app.objects.push_back(porsche);

	//lighting and positions for all track objects
	glm::vec3 trackPosition = glm::vec3(0.0f, -0.2f, 0.0f);
	glm::vec3 trackRotation = -glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);
	glm::vec3 trackScale = glm::vec3(22.0f);

	glm::vec3 trackAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 trackDiffuse = glm::vec3(0.6f);
	glm::vec3 trackSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
	float trackShininess = 1.0f;

	//plane object
	Object plane;
	GLuint planeTexture = pgr::createTexture("data/textures/Asph_baseColor.png");
	std::cout << "plane texture id: " << planeTexture << std::endl;
	plane.texture = planeTexture;
	plane.material.useTexture = true;

	MeshGeometry planeMesh;
	initObjectMesh(planeMesh, plane2_n_attribs_per_vertex, plane2_n_vertices, plane2_n_triangles, plane2_vertices, plane2_triangles);
	app.meshes.push_back(planeMesh);

	plane.meshId = 1;
	plane.position = trackPosition;
	plane.rotation = trackRotation;
	plane.scale = trackScale;

	plane.material.ambient = trackAmbient;
	plane.material.diffuse = trackDiffuse;
	plane.material.specular = trackSpecular;
	plane.material.shininess = trackShininess;

	app.objects.push_back(plane);

	//plane2 object
	Object plane2;
	plane2.texture = planeTexture;
	plane2.material.useTexture = true;

	MeshGeometry plane2Mesh;
	initObjectMesh(plane2Mesh, plane2_n_attribs_per_vertex, plane2_n_vertices, plane2_n_triangles, plane2_vertices, plane2_triangles);
	app.meshes.push_back(plane2Mesh);

	plane2.meshId = 2;
	plane2.position = trackPosition;
	plane2.rotation = trackRotation;
	plane2.scale = trackScale;

	plane2.material.ambient = trackAmbient;
	plane2.material.diffuse = trackDiffuse;
	plane2.material.specular = trackSpecular;
	plane2.material.shininess = trackShininess;

	app.objects.push_back(plane2);

	//kerbs object
	Object kerbs;
	GLuint kerbsTexture = pgr::createTexture("data/textures/Kerb_baseColor.png");
	std::cout << "kerbs texture id: " << kerbsTexture << std::endl;
	kerbs.texture = kerbsTexture;
	kerbs.material.useTexture = true;

	MeshGeometry kerbsMesh;
	initObjectMesh(kerbsMesh, kerbs_n_attribs_per_vertex, kerbs_n_vertices, kerbs_n_triangles, kerbs_vertices, kerbs_triangles);
	app.meshes.push_back(kerbsMesh);

	kerbs.meshId = 3;
	kerbs.position = trackPosition;
	kerbs.rotation = trackRotation;
	kerbs.scale = trackScale;

	kerbs.material.ambient = trackAmbient;
	kerbs.material.diffuse = trackDiffuse;
	kerbs.material.specular = trackSpecular;
	kerbs.material.shininess = trackShininess;

	app.objects.push_back(kerbs);

	//outside_line object
	Object outside_line;
	GLuint outside_lineTexture = pgr::createTexture("data/textures/ASPH2_baseColor.png");
	std::cout << "outside_line texture id: " << outside_lineTexture << std::endl;
	outside_line.texture = outside_lineTexture;
	outside_line.material.useTexture = true;

	MeshGeometry outside_lineMesh;
	initObjectMesh(outside_lineMesh, outside_line_n_attribs_per_vertex, outside_line_n_vertices, outside_line_n_triangles, outside_line_vertices, outside_line_triangles);
	app.meshes.push_back(outside_lineMesh);

	outside_line.meshId = 4;
	outside_line.position = trackPosition;
	outside_line.rotation = trackRotation;
	outside_line.scale = trackScale;

	outside_line.material.ambient = trackAmbient;
	outside_line.material.diffuse = trackDiffuse;
	outside_line.material.specular = trackSpecular;
	outside_line.material.shininess = trackShininess;

	app.objects.push_back(outside_line);

	//grass_inside object

	Object grass_inside;
	GLuint grass_insideTexture = pgr::createTexture("data/textures/Grass_baseColor.png");
	std::cout << "grass_inside texture id: " << grass_insideTexture << std::endl;
	grass_inside.texture = grass_insideTexture;
	grass_inside.material.useTexture = true;

	MeshGeometry grass_insideMesh;
	initObjectMesh(grass_insideMesh, grass_inside_n_attribs_per_vertex, grass_inside_n_vertices, grass_inside_n_triangles, grass_inside_vertices, grass_inside_triangles);
	app.meshes.push_back(grass_insideMesh);

	grass_inside.meshId = 5;
	grass_inside.position = trackPosition;
	grass_inside.rotation = trackRotation;
	grass_inside.scale = trackScale;

	grass_inside.material.ambient = trackAmbient;
	grass_inside.material.diffuse = trackDiffuse;
	grass_inside.material.specular = trackSpecular;
	grass_inside.material.shininess = trackShininess;

	app.objects.push_back(grass_inside);

	//plane_outside object
	Object plane_outside;
	GLuint plane_outsideTexture = pgr::createTexture("data/textures/Asphalt_baseColor.png");
	std::cout << "plane_outside texture id: " << plane_outsideTexture << std::endl;
	plane_outside.texture = plane_outsideTexture;
	plane_outside.material.useTexture = true;

	MeshGeometry plane_outsideMesh;
	initObjectMesh(plane_outsideMesh, plane_outside_n_attribs_per_vertex, plane_outside_n_vertices, plane_outside_n_triangles, plane_outside_vertices, plane_outside_triangles);
	app.meshes.push_back(plane_outsideMesh);

	plane_outside.meshId = 6;
	plane_outside.position = trackPosition;
	plane_outside.rotation = trackRotation;
	plane_outside.scale = trackScale;

	plane_outside.material.ambient = trackAmbient;
	plane_outside.material.diffuse = trackDiffuse;
	plane_outside.material.specular = trackSpecular;
	plane_outside.material.shininess = trackShininess;

	app.objects.push_back(plane_outside);

	//dirt object
	Object dirt;
	GLuint dirtTexture = pgr::createTexture("data/textures/BrownedGrass_baseColor.png");
	std::cout << "dirt texture id: " << dirtTexture << std::endl;
	dirt.texture = dirtTexture;
	dirt.material.useTexture = true;

	MeshGeometry dirtMesh;
	initObjectMesh(dirtMesh, dirt_n_attribs_per_vertex, dirt_n_vertices, dirt_n_triangles, dirt_vertices, dirt_triangles);
	app.meshes.push_back(dirtMesh);

	dirt.meshId = 7;
	dirt.position = trackPosition;
	dirt.rotation = trackRotation;
	dirt.scale = trackScale;

	dirt.material.ambient = trackAmbient;
	dirt.material.diffuse = trackDiffuse;
	dirt.material.specular = trackSpecular;
	dirt.material.shininess = trackShininess;

	app.objects.push_back(dirt);

	//lake object
	Object lake;
	GLuint lakeTexture = pgr::createTexture("data/textures/Aqua_baseColor.png");
	std::cout << "lake texture id: " << lakeTexture << std::endl;
	lake.texture = lakeTexture;
	lake.material.useTexture = true;

	MeshGeometry lakeMesh;
	initObjectMesh(lakeMesh, lake_n_attribs_per_vertex, lake_n_vertices, lake_n_triangles, lake_vertices, lake_triangles);
	app.meshes.push_back(lakeMesh);

	lake.meshId = 8;
	lake.position = trackPosition;
	lake.rotation = trackRotation;
	lake.scale = trackScale;

	lake.material.ambient = trackAmbient;
	lake.material.diffuse = trackDiffuse;
	lake.material.specular = trackSpecular;
	lake.material.shininess = trackShininess;

	lake.waterTex = 0.05f;

	app.objects.push_back(lake);

	//small_bounds object
	Object small_bounds;
	GLuint small_boundsTexture = pgr::createTexture("data/textures/BarriersCONC_baseColor.png");
	std::cout << "small_bounds texture id: " << small_boundsTexture << std::endl;
	small_bounds.texture = small_boundsTexture;
	small_bounds.material.useTexture = true;

	MeshGeometry small_boundsMesh;
	initObjectMesh(small_boundsMesh, small_bounds_n_attribs_per_vertex, small_bounds_n_vertices, small_bounds_n_triangles, small_bounds_vertices, small_bounds_triangles);
	app.meshes.push_back(small_boundsMesh);

	small_bounds.meshId = 9;
	small_bounds.position = trackPosition;
	small_bounds.rotation = trackRotation;
	small_bounds.scale = trackScale;

	small_bounds.material.ambient = trackAmbient;
	small_bounds.material.diffuse = trackDiffuse;
	small_bounds.material.specular = trackSpecular;
	small_bounds.material.shininess = trackShininess;

	app.objects.push_back(small_bounds);

	//fence object
	Object fence;
	GLuint fenceTexture = pgr::createTexture("data/textures/fence_baseColor.png");
	std::cout << "fence texture id: " << fenceTexture << std::endl;
	fence.texture = fenceTexture;
	fence.material.useTexture = true;

	MeshGeometry fenceMesh;
	initObjectMesh(fenceMesh, fence_n_attribs_per_vertex, fence_n_vertices, fence_n_triangles, fence_vertices, fence_triangles);
	app.meshes.push_back(fenceMesh);

	fence.meshId = 10;
	fence.position = trackPosition;
	fence.rotation = trackRotation;
	fence.scale = trackScale;

	fence.material.ambient = trackAmbient;
	fence.material.diffuse = trackDiffuse;
	fence.material.specular = trackSpecular;
	fence.material.shininess = trackShininess;

	app.objects.push_back(fence);

	//garage object
	Object garage;
	GLuint garageTexture = pgr::createTexture("data/textures/TSUKUB2_baseColor.png");
	std::cout << "garage texture id: " << garageTexture << std::endl;
	garage.texture = garageTexture;
	garage.material.useTexture = true;

	MeshGeometry garageMesh;
	initObjectMesh(garageMesh, garage_n_attribs_per_vertex, garage_n_vertices, garage_n_triangles, garage_vertices, garage_triangles);
	app.meshes.push_back(garageMesh);

	garage.meshId = 11;
	garage.position = trackPosition;
	garage.rotation = trackRotation;
	garage.scale = trackScale;

	garage.material.ambient = trackAmbient;
	garage.material.diffuse = trackDiffuse;
	garage.material.specular = trackSpecular;
	garage.material.shininess = trackShininess;

	app.objects.push_back(garage);

	//big_bounds object
	Object big_bounds;
	GLuint big_boundsTexture = pgr::createTexture("data/textures/outerbarrier_baseColor.png");
	std::cout << "big_bounds texture id: " << big_boundsTexture << std::endl;
	big_bounds.texture = big_boundsTexture;
	big_bounds.material.useTexture = true;

	MeshGeometry big_boundsMesh;
	initObjectMesh(big_boundsMesh, big_bounds_n_attribs_per_vertex, big_bounds_n_vertices, big_bounds_n_triangles, big_bounds_vertices, big_bounds_triangles);
	app.meshes.push_back(big_boundsMesh);

	big_bounds.meshId = 12;
	big_bounds.position = trackPosition;
	big_bounds.rotation = trackRotation;
	big_bounds.scale = trackScale;

	big_bounds.material.ambient = trackAmbient;
	big_bounds.material.diffuse = trackDiffuse;
	big_bounds.material.specular = trackSpecular;
	big_bounds.material.shininess = trackShininess;

	app.objects.push_back(big_bounds);

	//tower object
	Object tower;
	GLuint towerTexture = pgr::createTexture("data/textures/TSUKUB1_baseColor.png");
	std::cout << "tower texture id: " << towerTexture << std::endl;
	tower.texture = towerTexture;
	tower.material.useTexture = true;

	MeshGeometry towerMesh;
	initObjectMesh(towerMesh, tower_n_attribs_per_vertex, tower_n_vertices, tower_n_triangles, tower_vertices, tower_triangles);
	app.meshes.push_back(towerMesh);

	tower.meshId = 13;
	tower.position = trackPosition;
	tower.rotation = trackRotation;
	tower.scale = trackScale;

	tower.material.ambient = trackAmbient;
	tower.material.diffuse = trackDiffuse;
	tower.material.specular = trackSpecular;
	tower.material.shininess = trackShininess;

	app.objects.push_back(tower);

	//public_stand object
	Object public_stand;
	GLuint public_standTexture = pgr::createTexture("data/textures/Material_677_baseColor.png");
	std::cout << "public_stand texture id: " << public_standTexture << std::endl;
	public_stand.texture = public_standTexture;
	public_stand.material.useTexture = true;

	MeshGeometry public_standMesh;
	initObjectMesh(public_standMesh, public_stand_n_attribs_per_vertex, public_stand_n_vertices, public_stand_n_triangles, public_stand_vertices, public_stand_triangles);
	app.meshes.push_back(public_standMesh);

	public_stand.meshId = 14;
	public_stand.position = trackPosition + glm::vec3(0.0, 2.0, -20.0);
	public_stand.rotation = trackRotation;
	public_stand.scale = trackScale + glm::vec3(4.0f);

	public_stand.material.ambient = trackAmbient;
	public_stand.material.diffuse = trackDiffuse;
	public_stand.material.specular = trackSpecular;
	public_stand.material.shininess = trackShininess;

	app.objects.push_back(public_stand);

	app.person.position = glm::vec3(0.0f, 0.8f, 73.0f);

	//baloon object (yes balloon is two l's but I found out too late, not going to change it)
	Object baloon;
	GLuint baloonTexture = pgr::createTexture("data/textures/baloon_diffuse.png");
	std::cout << "baloon texture id: " << baloonTexture << std::endl;
	baloon.texture = baloonTexture;
	baloon.material.useTexture = true;

	MeshGeometry baloonMesh;
	initObjectMesh(baloonMesh, baloon_n_attribs_per_vertex, baloon_n_vertices, baloon_n_triangles, baloon_vertices, baloon_triangles);
	app.meshes.push_back(baloonMesh);

	baloon.meshId = 15;
	baloon.position = glm::vec3(-40.0f, 20.0f, 0.0f);
	baloon.rotation = glm::vec3(glm::radians(180.0f), 0.0f, 0.0f);;
	baloon.scale = glm::vec3(0.05f);

	baloon.material.ambient = trackAmbient;
	baloon.material.diffuse = trackDiffuse;
	baloon.material.specular = trackSpecular;
	baloon.material.shininess = trackShininess;

	app.objects.push_back(baloon);

	Object balloon3;
	balloon3.texture = baloonTexture;
	balloon3.material.useTexture = true;

	balloon3.meshId = 15;
	balloon3.position = glm::vec3(70.0f, 90.0f, 150.0f);
	balloon3.rotation = glm::vec3(glm::radians(180.0f), 0.0f, 0.0f);;
	balloon3.scale = glm::vec3(0.05f);

	balloon3.material.ambient = trackAmbient;
	balloon3.material.diffuse = trackDiffuse;
	balloon3.material.specular = trackSpecular;
	balloon3.material.shininess = trackShininess;

	app.objects.push_back(balloon3);

	Object balloon2;
	balloon2.texture = baloonTexture;
	balloon2.material.useTexture = true;

	balloon2.meshId = 15;
	balloon2.position = glm::vec3(100.0f, 120.0f, -150.0f);
	balloon2.rotation = glm::vec3(glm::radians(180.0f), 0.0f, 0.0f);;
	balloon2.scale = glm::vec3(0.05f);

	balloon2.material.ambient = trackAmbient;
	balloon2.material.diffuse = trackDiffuse;
	balloon2.material.specular = trackSpecular;
	balloon2.material.shininess = trackShininess;

	app.objects.push_back(balloon2);

	//omniwrench object
	Object omniwrench;
	omniwrench.texture = baloonTexture;
	omniwrench.material.useTexture = true;

	MeshGeometry omniwrenchMesh;
	initObjectMesh(omniwrenchMesh, omniwrench_n_attribs_per_vertex, omniwrench_n_vertices, omniwrench_n_triangles, omniwrench_vertices, omniwrench_triangles);
	app.meshes.push_back(omniwrenchMesh);

	omniwrench.meshId = 16;
	omniwrench.position = glm::vec3(8.0f, 0.0f, 70.0f);
	omniwrench.rotation = glm::vec3(glm::radians(90.0f), glm::radians(90.0f), glm::radians(90.0f));
	omniwrench.scale = glm::vec3(0.01f);

	omniwrench.material.ambient = trackAmbient;
	omniwrench.material.diffuse = trackDiffuse;
	omniwrench.material.specular = trackSpecular;
	omniwrench.material.shininess = 20.0f;

	app.objects.push_back(omniwrench);

	glm::vec3 cinemaPosition = glm::vec3(25.0f, 0.0f, 55.0f);
	glm::vec3 cinemaRotation = glm::vec3(glm::radians(-90.0f), 0.0f, glm::radians(-75.0f));
	glm::vec3 cinemaScale = glm::vec3(1.2f);

	//scafolding object
	Object scafolding;
	GLuint scafoldingTexture = pgr::createTexture("data/textures/black.jpg");
	std::cout << "scafolding texture id: " << scafoldingTexture << std::endl;
	scafolding.texture = scafoldingTexture;
	scafolding.material.useTexture = true;

	MeshGeometry scafoldingMesh;
	initObjectMesh(scafoldingMesh, scafolding_n_attribs_per_vertex, scafolding_n_vertices, scafolding_n_triangles, scafolding_vertices, scafolding_triangles);
	app.meshes.push_back(scafoldingMesh);

	scafolding.meshId = 17;
	scafolding.position = cinemaPosition;
	scafolding.rotation = cinemaRotation;
	scafolding.scale = cinemaScale;

	scafolding.material.ambient = trackAmbient;
	scafolding.material.diffuse = trackDiffuse;
	scafolding.material.specular = trackSpecular;
	scafolding.material.shininess = 20.0f;

	app.objects.push_back(scafolding);

	//object screen / film / spriteSheet
	Object screen;
	GLuint screenTexture = pgr::createTexture("data/textures/spritesheet.png");
	std::cout << "screen texture id: " << screenTexture << std::endl;
	screen.texture = screenTexture;
	screen.material.useTexture = true;

	MeshGeometry screenMesh;
	initObjectMesh(screenMesh, screen_n_attribs_per_vertex, screen_n_vertices, screen_n_triangles, screen_vertices, screen_triangles);
	app.meshes.push_back(screenMesh);

	screen.meshId = 18;
	screen.position = cinemaPosition;
	screen.rotation = cinemaRotation;
	screen.scale = cinemaScale;

	//4x2 grid, 12 fps, 8 frames
	screen.spriteSheet = glm::vec4(2.0f, 4.0f, 12.0f, 8.0f);

	screen.material.ambient = trackAmbient;
	screen.material.diffuse = trackDiffuse;
	screen.material.specular = trackSpecular;
	screen.material.shininess = 20.0f;

	app.objects.push_back(screen);

	//mcqueen object
	Object mcqueen;
	GLuint mcqueenTexture = pgr::createTexture("data/textures/lightning_mcqueen_baseColor.jpeg");
	std::cout << "mcqueen texture id: " << mcqueenTexture << std::endl;
	mcqueen.texture = mcqueenTexture;
	mcqueen.material.useTexture = true;

	MeshGeometry mcqueenMesh;
	initObjectMesh(mcqueenMesh, mcqueen_n_attribs_per_vertex, mcqueen_n_vertices, mcqueen_n_triangles, mcqueen_vertices, mcqueen_triangles);
	app.meshes.push_back(mcqueenMesh);

	mcqueen.meshId = 19;
	mcqueen.position = glm::vec3(-20.2f, 4.8f, 57.3f);
	mcqueen.rotation = glm::vec3(0.0f, glm::radians(-90.0f), glm::radians(180.0f));
	mcqueen.scale = glm::vec3(1.0f);

	mcqueen.material.ambient = trackAmbient;
	mcqueen.material.diffuse = trackDiffuse;
	mcqueen.material.specular = trackSpecular;
	mcqueen.material.shininess = 20.0f;

	app.objects.push_back(mcqueen);

	//mazda object
	Object mazda;
	GLuint mazdaTexture = pgr::createTexture("data/textures/material_0_baseColor.jpeg");
	std::cout << "mazda texture id: " << mazdaTexture << std::endl;
	mazda.texture = mazdaTexture;
	mazda.material.useTexture = true;

	MeshGeometry mazdaMesh;
	initObjectMesh(mazdaMesh, mazda_n_attribs_per_vertex, mazda_n_vertices, mazda_n_triangles, mazda_vertices, mazda_triangles);
	app.meshes.push_back(mazdaMesh);

	mazda.meshId = 20;
	mazda.position = glm::vec3(0.0f, 0.0f, 70.0f);
	mazda.rotation = glm::vec3(glm::radians(-90.0f), 0.0f, glm::radians(-90.0f));
	mazda.scale = glm::vec3(1.5f);

	mazda.material.ambient = trackAmbient;
	mazda.material.diffuse = trackDiffuse;
	mazda.material.specular = trackSpecular;
	mazda.material.shininess = 20.0f;

	app.objects.push_back(mazda);

	//dp3 object
	Object dp3;
	GLuint dp3Texture = pgr::createTexture("data/textures/Material.001_baseColor.png");
	std::cout << "dp3 texture id: " << dp3Texture << std::endl;
	dp3.texture = dp3Texture;
	dp3.material.useTexture = true;

	MeshGeometry dp3Mesh;
	initObjectMesh(dp3Mesh, dp3_n_attribs_per_vertex, dp3_n_vertices, dp3_n_triangles, dp3_vertices, dp3_triangles);
	app.meshes.push_back(dp3Mesh);

	dp3.meshId = 21;
	dp3.position = glm::vec3(0.0f, 0.1f, 70.0f);
	dp3.rotation = glm::vec3(glm::radians(-90.0f), 0.1f, glm::radians(-90.0f));
	dp3.scale = glm::vec3(1.5f);

	dp3.material.ambient = trackAmbient;
	dp3.material.diffuse = trackDiffuse;
	dp3.material.specular = trackSpecular;
	dp3.material.shininess = 20.0f;

	app.objects.push_back(dp3);

	//dunlop object
	Object dunlop;
	GLuint dunlopTexture = pgr::createTexture("data/textures/Material__32_baseColor.png");
	std::cout << "dunlop texture id: " << dunlopTexture << std::endl;
	dunlop.texture = dunlopTexture;
	dunlop.material.useTexture = true;

	MeshGeometry dunlopMesh;
	initObjectMesh(dunlopMesh, dunlop_n_attribs_per_vertex, dunlop_n_vertices, dunlop_n_triangles, dunlop_vertices, dunlop_triangles);
	app.meshes.push_back(dunlopMesh);

	dunlop.meshId = 22;
	dunlop.position = trackPosition;
	dunlop.rotation = trackRotation;
	dunlop.scale = trackScale;

	dunlop.material.ambient = trackAmbient;
	dunlop.material.diffuse = trackDiffuse;
	dunlop.material.specular = trackSpecular;
	dunlop.material.shininess = trackShininess;

	app.objects.push_back(dunlop);

	//dunlop_chicane object
	Object dunlop_chicane;
	dunlop_chicane.texture = planeTexture;
	dunlop_chicane.material.useTexture = true;

	MeshGeometry dunlop_chicaneMesh;
	initObjectMesh(dunlop_chicaneMesh, dunlop_chicane_n_attribs_per_vertex, dunlop_chicane_n_vertices, dunlop_chicane_n_triangles, dunlop_chicane_vertices, dunlop_chicane_triangles);
	app.meshes.push_back(dunlop_chicaneMesh);

	dunlop_chicane.meshId = 23;
	dunlop_chicane.position = trackPosition;
	dunlop_chicane.rotation = trackRotation;
	dunlop_chicane.scale = trackScale;

	dunlop_chicane.material.ambient = trackAmbient;
	dunlop_chicane.material.diffuse = trackDiffuse;
	dunlop_chicane.material.specular = trackSpecular;
	dunlop_chicane.material.shininess = trackShininess;

	app.objects.push_back(dunlop_chicane);


	//make AABB's
	//garage box
	app.colliders.push_back({ glm::vec3(11, -1, -100), glm::vec3(200, 8, 200) });

	//fence
	app.colliders.push_back({ glm::vec3(-11, -1, -200), glm::vec3(-11, 8, 200) });
		
	//2 barriers to not leave scene
	app.colliders.push_back({ glm::vec3(-100, -1, 123), glm::vec3(100, 8, 200) });
	app.colliders.push_back({ glm::vec3(-100, -1, -54), glm::vec3(100, 8, -54) });

	app.person.position = glm::vec3(0.0f, 0.8f, 73.0f);
}

void cleanUpScene() {
	for (MeshGeometry& mesh : app.meshes) {
		cleanUpModel(mesh);
	}

	for (int i = 0; i < 2; i++) {
		cleanUpModel(app.skyboxGeometries[i]);
		if (app.skyboxTextures[i]) glDeleteTextures(1, &app.skyboxTextures[i]);
	}

	app.meshes.clear();
	app.objects.clear();
}

void cleanUpModel(MeshGeometry& geometry) {
	if (geometry.vertexArrayObject != 0) glDeleteVertexArrays(1, &geometry.vertexArrayObject);
	if (geometry.vertexBufferObject != 0) glDeleteBuffers(1, &geometry.vertexBufferObject);
	if (geometry.elementBufferObject != 0) glDeleteBuffers(1, &geometry.elementBufferObject);

	geometry.vertexArrayObject = 0;
	geometry.vertexBufferObject = 0;
	geometry.elementBufferObject = 0;
	geometry.vertexCount = 0;
	geometry.indexCount = 0;
}
