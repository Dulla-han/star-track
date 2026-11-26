#pragma once
#include <vector>


//----------------------------------------------------------------------------------------
/**
 * \file    constants.h
 * \author  Jakub Zimmer
 * \date    2026
 * \brief   Constants and other non-changing variables
 */
 //----------------------------------------------------------------------------------------

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

enum { W, S, A, D, SPACE, R, SHIFT, CTRL, ALT,KEYS_COUNT };

inline float MOVE_SPEED = 25.0f;
inline float SPRINT_SPEED = 30.0f;
inline float SENSITIVITY = 0.1f;

inline float CAMERA_ELEVATION_MAX = 45.0f;

const char WIN_TITLE[] = "STAR*TRACK";

inline float LAP_TIME_PERIOD = 53.0f;

const int FPS = 60;

//static camera set ups
const glm::vec3 STATIC1_POS = glm::vec3(-19.0033, 5.16693, 60.0223);
const glm::vec3 STATIC1_LOOK = glm::normalize(glm::vec3(1.0f, 0.0f, -2.0f));
const glm::vec3 STATIC2_POS = glm::vec3(2.0f, 0.0f, 90.0f);
const glm::vec3 STATIC2_LOOK = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 DYNAMIC2_POS = glm::vec3(15.0f, 15.0f, 10.0f);
const glm::vec3 DYNAMIC2_LOOK = glm::vec3(0.0f, 0.0f, 0.0f);

const glm::vec3 RACECAR_1_POINTS[] = {
	glm::vec3(0.0f, -0.2f, 45.0f), //start line
	glm::vec3(-3.0f, -0.1f, -55.0f), //turn 1
	glm::vec3(16.0f, 1.0f, -96.0f), //turn 1
	glm::vec3(41.0f, 0.1f, -70.0f), //turn 1 apex
	glm::vec3(40.0f, -0.4f, 12.0f), //turn 2
	glm::vec3(52.0f, -0.3f, 72.0f), //turn 3
	glm::vec3(46.0f, -0.2f, 117.0f), //turn 4
	glm::vec3(67.0f, 1.0f, 133.0f), //turn 4
	glm::vec3(84.5f, -0.85f, 105.0f), //turn 4 apex
	glm::vec3(75.0f, -0.5f, 38.0f), //turn 5
	glm::vec3(87.0f, 0.8f, 11.0f), //turn 5 apex
	glm::vec3(142.0f, -0.2f, -6.3f), //turn 6
	glm::vec3(172.6f, -0.3f, -32.3f), //turn 6 apex
	glm::vec3(196.0f, -0.1f, -103.5f), //dunlop straight - turn 7
	glm::vec3(197.3f, 0.45f, -149.5f), //turn 8
	glm::vec3(217.5f, 1.0f, -172.6f), //turn 8
	glm::vec3(229.7f, 0.5f, -144.4f), //turn 8 apex
	glm::vec3(181.0f, -0.2f, -11.0f), //back-straight
	glm::vec3(129.0f, -0.25f, 117.6f), //turn 9
	glm::vec3(73.0f, 0.7f, 174.6f), //turn 9
	glm::vec3(8.8f, 0.1f, 146.7f) //turn 9 apex
}; //21 

const glm::vec3 RACECAR_2_POINTS[] = {
	glm::vec3(197.3f, 0.9f, -149.5f), //turn 8
	glm::vec3(217.5f, 1.6f, -172.6f), //turn 8
	glm::vec3(229.7f, 0.85f, -144.4f), //turn 8 apex
	glm::vec3(181.0f, -0.1f, -11.0f), //back-straight
	glm::vec3(129.0f, 0.2f, 117.6f), //turn 9
	glm::vec3(73.0f, 1.0f, 174.6f), //turn 9
	glm::vec3(8.8f, 0.5f, 146.7f), //turn 9 apex
	glm::vec3(0.0f, 0.0f, 45.0f), //start line
	glm::vec3(-3.0f, 0.3f, -55.0f), //turn 1
	glm::vec3(16.0f, 1.3f, -96.0f), //turn 1
	glm::vec3(41.0f, 0.55f, -70.0f), //turn 1 apex
	glm::vec3(40.0f, -0.1f, 12.0f), //turn 2
	glm::vec3(52.0f, 0.0f, 72.0f), //turn 3
	glm::vec3(46.0f, 0.3f, 117.0f), //turn 4
	glm::vec3(67.0f, 1.5f, 133.0f), //turn 4
	glm::vec3(84.5f, -0.45f, 105.0f), //turn 4 apex
	glm::vec3(75.0f, 0.1f, 38.0f), //turn 5
	glm::vec3(87.0f, 1.1f, 11.0f), //turn 5 apex
	glm::vec3(142.0f, 0.1f, -6.3f), //turn 6
	glm::vec3(172.6f, 0.1f, -32.3f), //turn 6 apex
	glm::vec3(196.0f, 0.1f, -103.5f) //dunlop straight - turn 7
}; //21 

const glm::vec3 RACECAR_3_POINTS[] = {
	glm::vec3(46.0f, 0.3f, 117.0f), //turn 4
	glm::vec3(67.0f, 1.5f, 133.0f), //turn 4
	glm::vec3(84.5f, -0.55f, 105.0f), //turn 4 apex
	glm::vec3(75.0f, -0.1f, 38.0f), //turn 5
	glm::vec3(87.0f, 1.2f, 11.0f), //turn 5 apex
	glm::vec3(142.0f, 0.0f, -6.3f), //turn 6
	glm::vec3(172.6f, 0.1f, -32.3f), //turn 6 apex
	glm::vec3(196.0f, 0.2f, -103.5f), //dunlop straight - turn 7
	glm::vec3(197.3f, 0.9f, -149.5f), //turn 8
	glm::vec3(217.5f, 1.5f, -172.6f), //turn 8
	glm::vec3(229.7f, 0.85f, -144.4f), //turn 8 apex
	glm::vec3(181.0f, 0.0f, -11.0f), //back-straight
	glm::vec3(129.0f, 0.4f, 117.6f), //turn 9
	glm::vec3(73.0f, 1.0f, 174.6f), //turn 9
	glm::vec3(8.8f, 0.4f, 146.7f), //turn 9 apex
	glm::vec3(0.0f, 0.1f, 45.0f), //start line
	glm::vec3(-3.0f, 0.3f, -55.0f), //turn 1
	glm::vec3(16.0f, 1.3f, -96.0f), //turn 1
	glm::vec3(41.0f, 0.45f, -70.0f), //turn 1 apex
	glm::vec3(40.0f, -0.1f, 12.0f), //turn 2
	glm::vec3(52.0f, 0.0f, 72.0f) //turn 3
}; //21 
