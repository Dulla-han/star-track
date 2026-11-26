#include "pgr.h"
#include "constants.h"
#include "initialize.h"
#include "render.h"
#include "callbacks.h"

int main(int argc, char** argv) {

    //basic initiliazation
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
    glutCreateWindow(WIN_TITLE);

    glutTimerFunc(1000 / FPS, timerCallBack, 0);

    if (!pgr::initialize(pgr::OGL_VER_MAJOR, pgr::OGL_VER_MINOR))
        pgr::dieWithError("pgr init failed, required OpenGL not supported?");

    //make background blue for now
    glClearColor(0.45f, 0.65f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    //custom made initiliazition functions
    initConstants();
    initializeShaderPrograms();
    initalizeSkyboxShaderProgram();
    initScene();
    initSkybox();
    initCallBacks(app.gameState);

    //register callbacks
    glutKeyboardFunc(keyboardCallBack);
    glutKeyboardUpFunc(keyboardCallBackUp);
    glutMouseFunc(mouseCallBack);
    glutMotionFunc(motionMouseCallBack);
    glutPassiveMotionFunc(passiveMouseMotionCallback);
    glutWarpPointer(WIN_WIDTH / 2, WIN_HEIGHT / 2);

    glutDisplayFunc(display);
    glutMainLoop();

    cleanUpScene();

    return EXIT_SUCCESS;
}
