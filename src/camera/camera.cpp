#include <stdexcept>
#include "camera.h"
#include "settings.h"
#include "utils/scenedata.h"

SceneCameraData Camera::getUpdatedRotation(SceneCameraData &camera, int deltaX, int deltaY) const {
    float xRadians = glm::radians(deltaX * 0.1f);
    float yRadians = glm::radians(deltaY * 0.1f);

    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(camera.look), glm::vec3(camera.up)));

    glm::vec3 look = glm::normalize(glm::vec3(camera.look));

    glm::mat4 yawRotation = glm::mat4(
        glm::vec4(cos(xRadians), 0, sin(xRadians), 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(-sin(xRadians), 0, cos(xRadians), 0),
        glm::vec4(0, 0, 0, 1)
        );

    look = glm::vec3(yawRotation * glm::vec4(look, 0.0f));
    glm::vec3 up = glm::mat3(yawRotation) * glm::vec3(camera.up);
    right = glm::normalize(glm::cross(look, up));

    glm::mat4 pitchRotation = glm::mat4(
        glm::vec4(cos(yRadians) + right.x * right.x * (1 - cos(yRadians)), right.x * right.y * (1 - cos(yRadians)) - right.z * sin(yRadians), right.x * right.z * (1 - cos(yRadians)) + right.y * sin(yRadians), 0),
        glm::vec4(right.y * right.x * (1 - cos(yRadians)) + right.z * sin(yRadians), cos(yRadians) + right.y * right.y * (1 - cos(yRadians)), right.y * right.z * (1 - cos(yRadians)) - right.x * sin(yRadians), 0),
        glm::vec4(right.z * right.x * (1 - cos(yRadians)) - right.y * sin(yRadians), right.z * right.y * (1 - cos(yRadians)) + right.x * sin(yRadians), cos(yRadians) + right.z * right.z * (1 - cos(yRadians)), 0),
        glm::vec4(0, 0, 0, 1)
        );

    look = glm::vec3(pitchRotation * glm::vec4(look, 0.0f));
    up = glm::vec3(pitchRotation * glm::vec4(up, 0.0f));

    camera.look = glm::vec4(look, 0.0f);
    camera.up = glm::vec4(up, 0.0f);
    return camera;

}

SceneCameraData Camera::getUpdatedCameraData(SceneCameraData &camera, std::unordered_map<Qt::Key, bool> m_keyMap, float speed, float deltaTime) const {
    if (m_keyMap[Qt::Key_W]){
        glm::vec4 translation = glm::normalize(camera.look) * speed * deltaTime;
        camera.pos += translation;
    }
    if (m_keyMap[Qt::Key_D]){
        glm::vec4 translateDir = glm::vec4(glm::normalize(glm::cross(glm::vec3(camera.look), glm::vec3(camera.up))), 0.0f);
        glm::vec4 translation = translateDir * speed * deltaTime;
        camera.pos += translation;
    }
    if (m_keyMap[Qt::Key_S]){
        glm::vec4 translation = glm::normalize(-camera.look) * speed * deltaTime;
        camera.pos += translation;
    }
    if (m_keyMap[Qt::Key_A]){
        glm::vec4 translateDir = glm::vec4(glm::normalize(glm::cross(glm::vec3(camera.look), glm::vec3(camera.up))), 0.0f);
        glm::vec4 translation = -translateDir * speed * deltaTime;
        camera.pos += translation;
    }
    if (m_keyMap[Qt::Key_Control]){
        glm::vec4 translateDir = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
        glm::vec4 translation = translateDir * speed * deltaTime;
        camera.pos += translation;
    }
    if (m_keyMap[Qt::Key_Space]){
        glm::vec4 translateDir = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        glm::vec4 translation = translateDir * speed * deltaTime;
        camera.pos += translation;
    }
    return camera;
}

glm::mat4 Camera::getViewMatrix(SceneCameraData &camera) const {
    // Optional TODO: implement the getter or make your own design
    glm::vec3 pos(camera.pos);
    glm::vec3 look(camera.look);
    glm::vec3 up(camera.up);
    glm::mat4 translate = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -pos[0], -pos[1], -pos[2], 1);
    glm::vec3 w = - glm::normalize(look);
    glm::vec3 u = glm::normalize(glm::cross(up, w));
    glm::vec3 v = glm::cross(w, u);
    glm::mat4 rotate = glm::mat4(u[0], v[0], w[0], 0, u[1], v[1], w[1], 0, u[2], v[2], w[2], 0, 0, 0, 0, 1);

    return rotate * translate;

}

glm::mat4 Camera::getProjectionMatrix(SceneCameraData &camera, float aspectRatio, float farPlane, float nearPlane) const {
    const float scale = 1.0f / tan(camera.heightAngle / 2.0f);
    glm::mat4 proj = glm::mat4(0.0f);

    proj[0][0] = scale / aspectRatio;
    proj[1][1] = scale;
    proj[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    proj[2][3] = -1.0f;
    proj[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    return proj;
}

float Camera::getAspectRatio(int width, int height) const {
    // Optional TODO: implement the getter or make your own design
    float w = (float) width;
    float h = (float) height;
    return (float) w / h;
}

float Camera::getHeightAngle(SceneCameraData &camera) const {
    // Optional TODO: implement the getter or make your own design
    return camera.heightAngle;
}

float Camera::getFocalLength(SceneCameraData &camera) const {
    // Optional TODO: implement the getter or make your own design
    return camera.focalLength;
}

float Camera::getAperture(SceneCameraData &camera) const {
    // Optional TODO: implement the getter or make your own design
    return camera.aperture;
}
