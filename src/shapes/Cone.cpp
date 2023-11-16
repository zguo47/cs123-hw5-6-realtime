#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Cone::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) {

    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!
    glm::vec3 nb_left = glm::normalize(glm::vec3(2 * bottomLeft.x, (0.5f - bottomLeft.y) / 2.0f, 2 * bottomLeft.z));
    glm::vec3 nb_right = glm::normalize(glm::vec3(2 * bottomRight.x, (0.5f - bottomRight.y) / 2.0f, 2 * bottomRight.z));
    glm::vec3 nt_left = glm::normalize(glm::vec3(2 * topLeft.x, (0.5f - topLeft.y) / 2.0f, 2 * topLeft.z));
    glm::vec3 nt_right = glm::normalize(glm::vec3(2 * topRight.x, (0.5f - topRight.y) / 2.0f, 2 * topRight.z));

    insertVec3(m_vertexData, topLeft);
    if ((topLeft.x == 0 && topLeft.z == 0) || (topRight.x == 0 && topRight.z == 0)){
        insertVec3(m_vertexData, glm::normalize(nb_left + nb_right));
    }else{
        insertVec3(m_vertexData, nt_left);
    }
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, nb_left);
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, nb_right);

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, nb_right);
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, nt_right);
        insertVec3(m_vertexData, topLeft);
    if ((topLeft.x == 0 && topLeft.z == 0) || (topRight.x == 0 && topRight.z == 0)){
        insertVec3(m_vertexData, glm::normalize(nb_left + nb_right));
    }else{
        insertVec3(m_vertexData, nt_left);
    }

}

void Cone::makeCap(float currentTheta, float nextTheta, float height) {
    // Note: think about how param 1 comes into play here!
    for (int i = 0; i < m_param1; i++){
        float k = (1.0f / m_param1);
        glm::vec3 topLeft(0.5 * k * i * glm::sin(currentTheta), height, 0.5 * k * i * glm::cos(currentTheta));
        glm::vec3 topRight(0.5 * k * i * glm::sin(nextTheta), height, 0.5 * k * i * glm::cos(nextTheta));
        glm::vec3 bottomLeft(0.5 * k * (i + 1) * glm::sin(currentTheta), height, 0.5 * k * (i + 1) * glm::cos(currentTheta));
        glm::vec3 bottomRight(0.5 * k * (i + 1) * glm::sin(nextTheta), height, 0.5 * k * (i + 1) * glm::cos(nextTheta));

        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - bottomLeft)));
        insertVec3(m_vertexData, bottomLeft);
        insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - bottomLeft)));
        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - bottomLeft)));

        insertVec3(m_vertexData, bottomRight);
        insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, topLeft - topRight)));
        insertVec3(m_vertexData, topRight);
        insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, topLeft - topRight)));
        insertVec3(m_vertexData, topLeft);
        insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, topLeft - topRight)));

    }
}

void Cone::makeSide(float currentTheta, float nextTheta) {
    // Note: think about how param 1 comes into play here!
    float k = (1.0f / m_param1);
    for (int i = 0; i < m_param1; i++){
        glm::vec3 topLeft(0.5 * (i * k) * glm::sin(currentTheta), (0.5f - i * k), 0.5 * (i * k) * glm::cos(currentTheta));
        glm::vec3 topRight(0.5 * (i * k) * glm::sin(nextTheta), (0.5f - i * k), 0.5 * (i * k) * glm::cos(nextTheta));
        glm::vec3 bottomLeft(0.5 * ((i + 1) * k) * glm::sin(currentTheta), (0.5f - (i + 1) * k), 0.5 * ((i + 1) * k) * glm::cos(currentTheta));
        glm::vec3 bottomRight(0.5 * ((i + 1) * k) * glm::sin(nextTheta), (0.5f - (i + 1) * k), 0.5 * ((i + 1) * k) * glm::cos(nextTheta));
        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Cone::makeCone() {
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++){
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeCap(nextTheta, currentTheta, -0.5f);
        makeSide(currentTheta, nextTheta);
    }
}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeCone();
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
