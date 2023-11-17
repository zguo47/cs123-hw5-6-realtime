#include <stdexcept>
#include "camera.h"
#include "settings.h"
#include "utils/scenedata.h"

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

    proj[0][0] = scale / aspectRatio; // scale the x coordinates of the projection
    proj[1][1] = scale;               // scale the y coordinates of the projection
    proj[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane); // used to remap z to [-1,1]
    proj[2][3] = -1.0f;               // set w = -z
    proj[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane); // calculate z
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
