#include <time.h>
#include <list>
#include <cmath>
#include <iostream>

#include "pgr.h"
#include "initialize.h"
#include "callbacks.h"
#include "render.h"

//Snapshot the current camera state as the starting point for a mode transition
//ai helped
auto beginCameraTransition = []() {
	app.camera.fromPosition = app.camera.position;
	app.camera.fromFront = app.camera.front;
	app.camera.transitionStart = app.gameState.elapsedTime;
	app.camera.transitioning = true;
};

/**
 *@brief Test whether a point (with player radius r) intersects any collider in app.colliders
 *@param p Proposed player position.
 *@param r Player radius for the swept test; default 0.4 units
 *@return true if any AABB overlaps the inflated point
 */
static bool collidesWithAny(const glm::vec3& p, float r = 0.4f) {
	for (const AABB& b : app.colliders) {
		if (p.x + r > b.min.x && p.x - r < b.max.x &&
			p.y + r > b.min.y && p.y - r < b.max.y &&
			p.z + r > b.min.z && p.z - r < b.max.z) {
			return true;
		}
	}
	return false;
}

void keyboardCallBack(unsigned char keyPressed, int mouseX, int mouseY) {

	int mods = glutGetModifiers();
	if (mods & GLUT_ACTIVE_SHIFT)app.gameState.keyMap[SHIFT] = true;
	if (mods & GLUT_ACTIVE_CTRL) app.gameState.keyMap[CTRL] = true;
	if (mods & GLUT_ACTIVE_ALT) app.gameState.keyMap[ALT] = true;
	
	switch (keyPressed) {
	case 27:
		glutLeaveMainLoop();
		break;
	case 'W':
	case 'w':
		//move forward
		if (!app.gameState.sceneOver) {
			app.gameState.keyMap[W] = true;
		}
		break;
	case 'S':
	case 's':
		//move backwords
		if (!app.gameState.sceneOver) {
			app.gameState.keyMap[S] = true;
		}
		break;
	case 'A':
	case 'a':
		//move left
		if (!app.gameState.sceneOver) {
			app.gameState.keyMap[A] = true;
		}
		break;
	case 'D':
	case 'd':
		//move right
		if (!app.gameState.sceneOver) {
			app.gameState.keyMap[D] = true;
		}
		break;
		//switch to flight mode
	case 'C':
	case 'c':
		if (!app.gameState.sceneOver) {
			app.gameState.toggle_flight = !app.gameState.toggle_flight;
		}
		break;
		//switch skybox and fog color
	case ' ':
		if (!app.gameState.sceneOver) {
			if (app.skyboxProgram.curSkyboxId == 0) {
				glUniform3f(app.shaderProgram.fogColorLocation, 0.05f, 0.0f, 0.05f);
				glUniform1f(app.shaderProgram.sunParamLocation, 1.0f);
				app.skyboxProgram.curSkyboxId = 1;
			}
			else {
				glUniform3f(app.shaderProgram.fogColorLocation, 0.5f, 0.5f, 0.5f);
				glUniform1f(app.shaderProgram.sunParamLocation, 1.5f);
				app.skyboxProgram.curSkyboxId = 0;
			}
		}
		break;
		//reversing the stencil red effects
	case 'R':
	case 'r':
		if (!app.gameState.sceneOver) {
			app.gameState.keyMap[R] = true;
		}
		break;
		//free, dynamic2, static2, dynamic1, static1 cameras
	case '1':
		if (!app.gameState.sceneOver) {
			if (!app.camera.toggleFree) beginCameraTransition();
			app.camera.toggleStatic1 = false;
			app.camera.toggleStatic2 = false;
			app.camera.toggleDynamic1 = false;
			app.camera.toggleDynamic2 = false;
			app.camera.toggleFree = true;
		}
		break;
	case '2':
		if (!app.gameState.sceneOver) {
			if (!app.camera.toggleDynamic2) beginCameraTransition();
			app.camera.toggleStatic1 = false;
			app.camera.toggleStatic2 = false;
			app.camera.toggleDynamic1 = false;
			app.camera.toggleDynamic2 = true;
			app.camera.toggleFree = false;
			app.camera.staticYawOffset = 0.0f;
			app.camera.staticPitchOffset = 0.0f;
		}
		break;
	case '3':
		if (!app.gameState.sceneOver) {
			if (!app.camera.toggleStatic2) beginCameraTransition();
			app.camera.toggleStatic1 = false;
			app.camera.toggleStatic2 = true;
			app.camera.toggleDynamic1 = false;
			app.camera.toggleDynamic2 = false;
			app.camera.toggleFree = false;
		}
		break;
	case '4':
		if (!app.gameState.sceneOver) {
			if (!app.camera.toggleDynamic1) beginCameraTransition();
			app.camera.toggleStatic1 = false;
			app.camera.toggleStatic2 = false;
			app.camera.toggleDynamic1 = true;
			app.camera.toggleDynamic2 = false;
			app.camera.toggleFree = false;
		}
		break;
	case '5':
		if (!app.gameState.sceneOver) {
			if (!app.camera.toggleStatic1) beginCameraTransition();
			app.camera.toggleStatic1 = true;
			app.camera.toggleStatic2 = false;
			app.camera.toggleDynamic1 = false;
			app.camera.toggleDynamic2 = false;
			app.camera.toggleFree = false;
			app.camera.staticYawOffset = 0.0f;
			app.camera.staticPitchOffset = 0.0f;
		}
	//reload changed config file
	case 'L':
	case 'l':
		if (!app.gameState.sceneOver) {
			initConstants();
		}
		break;
	default:
		//do nothin
		break;

	
	}
}

void keyboardCallBackUp(unsigned char keyReleased, int mouseX, int mouseY) {
	int mods = glutGetModifiers();
	app.gameState.keyMap[SHIFT] = (mods & GLUT_ACTIVE_SHIFT) != 0;

	//handle the release of keys
	switch (keyReleased) {
	case ' ':
		app.gameState.keyMap[SPACE] = false;
		break;
	case 'W':
	case 'w':
		app.gameState.keyMap[W] = false;
		break;
	case 'S':
	case 's':
		app.gameState.keyMap[S] = false;
		break;
	case 'A':
	case 'a':
		app.gameState.keyMap[A] = false;
		break;
	case 'D':
	case 'd':
		app.gameState.keyMap[D] = false;
		break;
	case 'R':
	case 'r':
		app.gameState.keyMap[R] = false;
		break;
	default:
		break; //nothin
	}
}

void timerCallBack(int) {
	//keep track of time
	app.gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME);

	//update scene clock
	update(app.gameState.elapsedTime);
	glutPostRedisplay();
	glutTimerFunc(1000 / FPS, timerCallBack, 0);
}

void movePerson(float deltaTime) {
	glm::vec3 forward;
	forward.x = cos(glm::radians(app.camera.yaw));
	forward.y = 0.0f;
	forward.z = sin(glm::radians(app.camera.yaw));
	forward = glm::normalize(forward);

	float actual_move_speed = MOVE_SPEED;

	glm::vec3 right = glm::normalize(glm::cross(forward, app.camera.up));

	glm::vec3 delta(0.0f);
	if (app.gameState.keyMap[W]) {
		if (app.gameState.toggle_flight) delta += app.camera.front;
		else delta += forward;
	}

	if (app.gameState.keyMap[S]) {
		if (app.gameState.toggle_flight) delta -= app.camera.front;
		else delta -= forward;
	}

	if (app.gameState.keyMap[D]) {
		if (app.gameState.toggle_flight) delta += glm::normalize(glm::cross(app.camera.front, app.camera.up));
		else delta += right;
	}

	if (app.gameState.keyMap[A]) {
		if (app.gameState.toggle_flight) delta -= glm::normalize(glm::cross(app.camera.front, app.camera.up));
		else delta -= right;
	}
	if (app.gameState.keyMap[SHIFT]) actual_move_speed = SPRINT_SPEED;

	if (glm::length(delta) < 1e-6f) return;
	delta = glm::normalize(delta) * actual_move_speed * deltaTime;

	//flight bypasses collision entirely
	if (app.gameState.toggle_flight) {
		app.person.position += delta;
		return;
	}

	glm::vec3 tryX = app.person.position + glm::vec3(delta.x, 0.0f, 0.0f);
	if (!collidesWithAny(tryX)) app.person.position = tryX;

	glm::vec3 tryZ = app.person.position + glm::vec3(0.0f, 0.0f, delta.z);
	if (!collidesWithAny(tryZ)) app.person.position = tryZ;
}


void passiveMouseMotionCallback(int mouseX, int mouseY) {
	int mods = glutGetModifiers();
	app.gameState.keyMap[SHIFT] = (mods & GLUT_ACTIVE_SHIFT) != 0;

	float dx = float(mouseX - WIN_WIDTH / 2);
	float dy = float(mouseY - WIN_HEIGHT / 2);
	//unless in free mode, no looking around
	if (!app.camera.toggleFree || app.camera.transitioning) return;

	app.camera.yaw += dx * SENSITIVITY;
	app.camera.pitch -= dy * SENSITIVITY;

	if (app.camera.pitch > CAMERA_ELEVATION_MAX)
		app.camera.pitch = CAMERA_ELEVATION_MAX;
	if (app.camera.pitch < -CAMERA_ELEVATION_MAX)
		app.camera.pitch = -CAMERA_ELEVATION_MAX;

	glm::vec3 direction;
	direction.x = cos(glm::radians(app.camera.yaw)) * cos(glm::radians(app.camera.pitch));
	direction.y = sin(glm::radians(app.camera.pitch));
	direction.z = sin(glm::radians(app.camera.yaw)) * cos(glm::radians(app.camera.pitch));
	app.camera.front = glm::normalize(direction);

	glutWarpPointer(WIN_WIDTH / 2, WIN_HEIGHT / 2);
}

void mouseCallBack(int buttonPressed, int buttonState, int mouseX, int mouseY) {
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {
		unsigned char objectID = 0;
		glReadPixels(mouseX, WIN_HEIGHT - mouseY, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &objectID);

		std::cout << "clicked stencil id = " << (int)objectID << std::endl;

		if (objectID == 0) return;
		size_t idx = objectID - 1;
		if (idx >= app.objects.size()) return;


		if (app.gameState.keyMap[R]) {
			//restore — only if it was actually picked
			if (app.objects[idx].isPicked) {
				app.objects[idx].material.diffuse = app.objects[idx].originalDiffuse;
				app.objects[idx].isPicked = false;
			}
		}
		else {
			//highlight — only if not already picked, so we don't save the red as the "original"
			if (!app.objects[idx].isPicked) {
				app.objects[idx].originalDiffuse = app.objects[idx].material.diffuse;
				app.objects[idx].material.diffuse = glm::vec3(2.0f, 0.0f, 0.0f);
				app.objects[idx].isPicked = true;
			}
		}
	}

	if (buttonPressed == GLUT_RIGHT_BUTTON) {
		if (buttonState == GLUT_DOWN) {
			if (app.camera.toggleStatic1 || app.camera.toggleStatic2) {
				app.gameState.dragLooking = true;
				app.gameState.dragStartX = mouseX;
				app.gameState.dragStartY = mouseY;
				app.gameState.dragStartYaw = app.camera.staticYawOffset;
				app.gameState.dragStartPitch = app.camera.staticPitchOffset;
			}
			else
			{
				app.gameState.dragLooking = false;
			}
		}
	}
}

void motionMouseCallBack(int mouseX, int mouseY) {
	if (!app.gameState.dragLooking) return;

	float dx = float(mouseX - app.gameState.dragStartX);
	float dy = float(mouseY - app.gameState.dragStartY);

	app.camera.staticYawOffset = app.gameState.dragStartYaw - dx * 0.005f;
	app.camera.staticPitchOffset = app.gameState.dragStartPitch - dy * 0.005f;

	const float maxPitch = glm::radians(80.0f);
	app.camera.staticPitchOffset = glm::clamp(app.camera.staticPitchOffset, -maxPitch, maxPitch);
}
