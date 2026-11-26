#pragma once
//----------------------------------------------------------------------------------------
/**
 * \file    callbacks.h
 * \author  Jakub Zimmer
 * \date    2026
 * \brief   Basic callback functions for handling inputs 
 */
 //----------------------------------------------------------------------------------------

/**
 *@brief GLUT key-down callback. Updates the key map and handles single-shot
 *        toggles (camera switching, flight mode, skybox/fog cycle, exit)
 *@param keyPressed The pressed character code
 */
void keyboardCallBack(unsigned char keyPressed, int mouseX, int mouseY);

//GLUT key-up callback. Clears the corresponding key-map entry
void keyboardCallBackUp(unsigned char keyReleased, int mouseX, int mouseY);

/**
 *@brief Main timer tick. Updates the global elapsed time, calls update()
 *        and re-arms itself at 1000/FPS milliseconds
 */
void timerCallBack(int);

/**
 *@brief Apply WASD input to app.person.position with axis-separated AABB
 *        collision (free-cam walks; flight mode bypasses collision)
 *@param deltaTime Time since the previous frame, in seconds
 */
void movePerson(float deltaTime);

/**
 *@brief Mouse motion handler for free-camera yaw/pitch. Disabled in
 *        non-free modes and during transitions. Re-centers the cursor
 */
void passiveMouseMotionCallback(int mouseX, int mouseY);
/**
 *@brief Left-click picking: reads the stencil buffer at the cursor position
 *        and reacts to the clicked object's ID (1 + index in app.objects)
 */
void mouseCallBack(int buttonPressed, int buttonState, int mouseX, int mouseY);

/**
 *@brief GLUT mouse drag callback. While the right mouse button is held in
 *       a static camera mode, updates the camera's yaw/pitch offsets so the
 *       user can pan the look direction. Active only when dragLooking is set
 *@param mouseX Current mouse x coordinate, in window pixels
 *@param mouseY Current mouse y coordinate, in window pixels
 */
void motionMouseCallBack(int mouseX, int mouseY);