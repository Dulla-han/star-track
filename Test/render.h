#pragma once

#include "initialize.h"

//----------------------------------------------------------------------------------------
/**
 * \file    render.h
 * \author  Jakub Zimmer
 * \date    2026
 * \brief   Display, update and draw object functions
 */
 //----------------------------------------------------------------------------------------

//GLUT display callback. Clears the framebuffer, draws the skybox, then every object
void display();

/**
 *@brief Per-frame logic: advance the car along its curve, push live uniforms
 *        (timeElapsed, spotLight position), compute the target camera state
 *        per mode and lerp during transitions, then move the free-cam person
 *@param timeElapsed Absolute seconds since program start
 */
void update(float timeElapsed);

/**
 *@brief Draw one Object: build its model matrix, push per-object uniforms
 *        (material, water, sprite-sheet), bind its texture and mesh, draw
 *        Writes (index+1) into the stencil buffer for picking
 *@param obj       Object to draw
 *@param stencilId Value written into the stencil buffer for this object
 */
void drawObject(const Object& obj, GLubyte stencilId);

//Draw the current skybox cube with depth/stencil writes disabled
void drawSkybox();

/**
 *@brief Evaluate a Catmull-Rom spline segment at parameter u, producing both
 *       a position and a tangent vector. Used to drive race-car animation
 *       along the closed loop of track waypoints
 *@param P0 Control point preceding the segment start (index-1, wrapped)
 *@param P1 Segment start point; curve passes through this at u=0
 *@param P2 Segment end point; curve passes through this at u=1
 *@param P3 Control point following the segment end (index+2, wrapped)
 *@param u  Local parameter in [0,1]; 0 is at P1, 1 is at P2
 *@param carPos     [out] Interpolated position on the spline
 *@param carTangent [out] Tangent vector at u (unnormalized; direction of motion)
 */
void racingCurve(const glm::vec3 P0, const glm::vec3 P1, const glm::vec3 P2, const glm::vec3 P3, float u, glm::vec3& carPos, glm::vec3& carTangent);