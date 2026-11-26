#include "pgr.h"
#include "constants.h"
#include "data_models.h"
#include "initialize.h"
#include "render.h"
#include "callbacks.h"

#include <iostream>
#include <math.h>
 
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawSkybox();

    glUseProgram(app.shaderProgram.program);

    //draw each object
    for (size_t i = 0; i < app.objects.size(); i++) {
        drawObject(app.objects[i], static_cast<GLubyte>(i + 1));
    }

    glutSwapBuffers();
}

/**
 * @brief Updates scene logic for one frame (camera, car path, lights, lerp).
 * @param timeElapsed Seconds since program start.
 */
void update(float timeElapsed) {
    static float lastTime = timeElapsed;
    float deltaTime = timeElapsed - lastTime;
    lastTime = timeElapsed;
    app.gameState.elapsedTime = timeElapsed;
    //in seconds
    const float period = LAP_TIME_PERIOD;


    //parametr
    float t = fmod(timeElapsed / period, 1.0f);
    const int N = std::size(RACECAR_1_POINTS);
    float scaled = t * N;
    size_t segment = static_cast<size_t>(scaled);
    float u = scaled - segment;

    //porsche driving around
    glm::vec3 carPos1, carTangent1;
    const auto& CAR1_P0 = RACECAR_1_POINTS[(segment + N - 1) % N];
    const auto& CAR1_P1 = RACECAR_1_POINTS[segment % N];
    const auto& CAR1_P2 = RACECAR_1_POINTS[(segment + 1) % N];
    const auto& CAR1_P3 = RACECAR_1_POINTS[(segment + 2) % N];
    racingCurve(CAR1_P0, CAR1_P1, CAR1_P2, CAR1_P3, u, carPos1, carTangent1);
    float carYaw1 = atan2(-carTangent1.z, carTangent1.x) + glm::radians(90.0);

    app.objects[0].rotation.y = carYaw1;
    app.objects[0].position = carPos1;

    //mazda driving around
    glm::vec3 carPos2, carTangent2;
    const auto& CAR2_P0 = RACECAR_2_POINTS[(segment + N - 1) % N];
    const auto& CAR2_P1 = RACECAR_2_POINTS[segment % N];
    const auto& CAR2_P2 = RACECAR_2_POINTS[(segment + 1) % N];
    const auto& CAR2_P3 = RACECAR_2_POINTS[(segment + 2) % N];
    racingCurve(CAR2_P0, CAR2_P1, CAR2_P2, CAR2_P3, u, carPos2, carTangent2);
    float carYaw2 = atan2(-carTangent2.z, carTangent2.x) + glm::radians(180.0f);

    app.objects[22].rotation.z = carYaw2;
    app.objects[22].position = carPos2;

    //dp3 driving around
    glm::vec3 carPos3, carTangent3;
    const auto& CAR3_P0 = RACECAR_3_POINTS[(segment + N - 1) % N];
    const auto& CAR3_P1 = RACECAR_3_POINTS[segment % N];
    const auto& CAR3_P2 = RACECAR_3_POINTS[(segment + 1) % N];
    const auto& CAR3_P3 = RACECAR_3_POINTS[(segment + 2) % N];
    racingCurve(CAR3_P0, CAR3_P1, CAR3_P2, CAR3_P3, u, carPos3, carTangent3);
    float carYaw3 = atan2(-carTangent3.z, carTangent3.x) + glm::radians(180.0f);

    app.objects[23].rotation.z = carYaw3;
    app.objects[23].position = carPos3;

    glUseProgram(app.shaderProgram.program);
    glUniform1f(glGetUniformLocation(app.shaderProgram.program, "timeElapsed"), timeElapsed);
    glUniform3f(glGetUniformLocation(app.shaderProgram.program, "spotLightPosition"), carPos1.x, 3.0, carPos1.z);

    glm::vec3 carHeading = glm::normalize(glm::vec3(carTangent1.x, 0.0f, carTangent1.z));;

    glm::vec3 targetPos, targetFront;

    //if free walking, key 1
    if (app.camera.toggleFree) {
        targetPos = app.person.position;
        targetFront = app.camera.front;
    }
    //if dynamic2, key 2
    else if (app.camera.toggleDynamic2) {
        targetPos = DYNAMIC2_POS;
        targetFront = glm::normalize(app.objects[0].position - DYNAMIC2_POS);
    }
    //if static2, key 3
    else if (app.camera.toggleStatic2) {
        targetPos = STATIC2_POS;
        glm::vec3 baseFront = STATIC2_LOOK;

        glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), app.camera.staticYawOffset, glm::vec3(0, 1, 0));
        glm::vec3 right = glm::normalize(glm::cross(baseFront, glm::vec3(0, 1, 0)));
        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), app.camera.staticPitchOffset, right);

        targetFront = glm::vec3(yawRotation * pitchRotation * glm::vec4(baseFront, 0.0f));
    }
    //if dynamic1, key 4
    else if (app.camera.toggleDynamic1) {
        //follow the car
        targetPos = carPos1 - carHeading * 2.5f + glm::vec3(0.0f, 1.1f, 0.0f);
        targetFront = carHeading + glm::vec3(0.0, 0.0f, glm::radians(-30.0f));
    }
    //if static1, key 5
    else if (app.camera.toggleStatic1) {
        targetPos = STATIC1_POS;
        glm::vec3 baseFront = STATIC1_LOOK;

        glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), app.camera.staticYawOffset, glm::vec3(0, 1, 0));
        glm::vec3 right = glm::normalize(glm::cross(baseFront, glm::vec3(0, 1, 0)));
        glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), app.camera.staticPitchOffset, right);

        targetFront = glm::vec3(yawRotation * pitchRotation * glm::vec4(baseFront, 0.0f));
    }

    if (app.camera.transitioning) {
        //parametr t taken so this one is u
        float u = (timeElapsed - app.camera.transitionStart) / app.camera.transitionDuration;
        if (u >= 1.0f) {
            u = 1.0f;
            app.camera.transitioning = false;
            if (app.camera.toggleFree) {
                app.camera.front = glm::normalize(targetFront);
                app.camera.yaw = glm::degrees(atan2(app.camera.front.z, app.camera.front.x));
                app.camera.pitch = glm::degrees(asin(app.camera.front.y));
            }
        }
        app.camera.position = glm::mix(app.camera.fromPosition, targetPos, u);
        app.camera.front = glm::normalize(glm::mix(app.camera.fromFront, targetFront, u));
    }
    else {
        app.camera.position = targetPos;
        app.camera.front = targetFront;
    }

    std::cout << "pos = ("<< app.person.position.x << ", " << app.person.position.y << ", "<< app.person.position.z << ")\n";
    
    movePerson(deltaTime);
}

//draw each for each object and define matrices for view
void drawObject(const Object& obj, GLubyte stencilId) {
    glStencilFunc(GL_ALWAYS, stencilId, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    MeshGeometry& mesh = app.meshes[obj.meshId];

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, obj.position);
    model = glm::rotate(model, obj.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, obj.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, obj.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, obj.scale);

    glm::mat4 view = glm::lookAt(
        app.camera.position,
        app.camera.position + app.camera.front,
        app.camera.up
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(WIN_WIDTH) / float(WIN_HEIGHT),
        0.1f,
        1000.0f
    );

    glUniformMatrix4fv(app.shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(app.shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(app.shaderProgram.PmatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(app.shaderProgram.ambientLocation, 1, glm::value_ptr(obj.material.ambient));
    glUniform3fv(app.shaderProgram.diffuseLocation, 1, glm::value_ptr(obj.material.diffuse));
    glUniform3fv(app.shaderProgram.specularLocation, 1, glm::value_ptr(obj.material.specular));
    glUniform1f(app.shaderProgram.shininessLocation, obj.material.shininess);

    glUniform1f(glGetUniformLocation(app.shaderProgram.program, "waterTex"), obj.waterTex);
    glUniform4fv(glGetUniformLocation(app.shaderProgram.program, "spriteSheet"), 1, glm::value_ptr(obj.spriteSheet));

    glUniform1i(app.shaderProgram.useTextureLocation, obj.material.useTexture ? 1 : 0);
    glActiveTexture(GL_TEXTURE0);



    if (obj.material.useTexture && obj.texture != 0) {
        glBindTexture(GL_TEXTURE_2D, obj.texture);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindVertexArray(mesh.vertexArrayObject);
    glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void drawSkybox() {
    glm::mat4 view = glm::lookAt(
        app.camera.position,
        app.camera.position + app.camera.front,
        app.camera.up
    );

    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f),
        float(WIN_WIDTH) / float(WIN_HEIGHT),
        0.1f,
        1000.0f
    );

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glStencilMask(0x00);

    glUseProgram(app.skyboxProgram.program);

    glUniformMatrix4fv(app.skyboxProgram.viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(app.skyboxProgram.projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));

    int id = app.skyboxProgram.curSkyboxId;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app.skyboxTextures[id]);

    glBindVertexArray(app.skyboxGeometries[id].vertexArrayObject);
    glDrawElements(GL_TRIANGLES, app.skyboxGeometries[id].indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glStencilMask(0xFF);
}

void racingCurve(const glm::vec3 P0, const glm::vec3 P1, const glm::vec3 P2, const glm::vec3 P3, float u, glm::vec3 &carPos, glm::vec3 & carTangent) {
    float u2 = u * u;
    float u3 = u2 * u;

    carPos = 0.5f * ((2.0f * P1) + (-P0 + P2) * u 
        + (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * u2
        + (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * u3);

    carTangent = 0.5f * ((-P0 + P2)
        + 2.0f * (2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * u
        + 3.0f * (-P0 + 3.0f * P1 - 3.0f * P2 + P3) * u2);
}
