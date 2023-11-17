#include "realtime.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "shapes/Cone.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Sphere.h"
#include "utils/shaderloader.h"
#include "utils/sceneparser.h"
#include "camera/camera.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    // Delete the VBO
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0; // Set to 0 to mark it as 'deleted'
    }

    // Delete the VAO
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0; // Set to 0 to mark it as 'deleted'
    }

    // If you have shaders or shader programs, you should also delete them
    if (m_shader) {
        glDeleteProgram(m_shader);
        m_shader = 0; // Set to 0 to mark it as 'deleted'
    }

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glClearColor(0, 0, 0, 255);

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    initialized = true;
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);

    bindBuffer();

}

void Realtime::bindBuffer(){
    // ================== Vertex Buffer Objects

    // Bind the VBO you created here
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    myCone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    myCube.updateParams(settings.shapeParameter1);
    mySphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    myCylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);

    conepoints = myCone.generateShape();
    spherepoints = mySphere.generateShape();
    cubepoints = myCube.generateShape();
    cylinderpoints = myCylinder.generateShape();

    size_t totalSize = conepoints.size() + spherepoints.size() +
                       cubepoints.size() + cylinderpoints.size();

    std::vector<GLfloat> allVertices(totalSize);

    std::copy(conepoints.begin(), conepoints.end(), allVertices.begin());
    std::copy(spherepoints.begin(), spherepoints.end(), allVertices.begin() + conepoints.size());
    std::copy(cubepoints.begin(), cubepoints.end(), allVertices.begin() + conepoints.size() + spherepoints.size());
    std::copy(cylinderpoints.begin(), cylinderpoints.end(), allVertices.begin() + conepoints.size() + spherepoints.size() + cubepoints.size());


    glBufferData(GL_ARRAY_BUFFER, allVertices.size() * sizeof(GLfloat), allVertices.data(), GL_STATIC_DRAW);

    // ================== Vertex Array Objects

    // Task 12: Bind the VAO you created here
    glBindVertexArray(m_vao);

    // Task 13: Add position and color attributes to your VAO here
    GLsizei stride = 6 * sizeof(GLfloat);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    // ================== Returning to Default State

    // Task 14: Unbind your VBO and VAO here
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    // Task 15: Clear the screen here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind the shader
    glUseProgram(m_shader);

    viewMatrix = camera.getViewMatrix(metaData.cameraData);
    float aspectRatio = camera.getAspectRatio(size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    projectionMatrix = camera.getProjectionMatrix(metaData.cameraData, aspectRatio, settings.farPlane, settings.nearPlane);

//    projectionMatrix = glm::perspective(metaData.cameraData.heightAngle, aspectRatio, settings.nearPlane, settings.farPlane);

    // Pass in necessary paramets
    GLint v_location = glGetUniformLocation(m_shader, "view_matrix");
    glUniformMatrix4fv(v_location, 1, GL_FALSE, &viewMatrix[0][0]);

    GLint p_location = glGetUniformLocation(m_shader, "projection_matrix");
    glUniformMatrix4fv(p_location, 1, GL_FALSE, &projectionMatrix[0][0]);

    GLint m_location = glGetUniformLocation(m_shader, "model_matrix");

    GLint mka_location = glGetUniformLocation(m_shader, "ambient");

    GLint mkd_location = glGetUniformLocation(m_shader, "diffuse");

    GLint mks_location = glGetUniformLocation(m_shader, "specular");

    GLint shi_location = glGetUniformLocation(m_shader, "m_shininess");

    GLint numLightsLocation = glGetUniformLocation(m_shader, "num_lights");
    glUniform1i(numLightsLocation, metaData.lights.size());

    for (size_t i = 0; i < metaData.lights.size(); ++i) {
        std::string lightBase = "lights[" + std::to_string(i) + "]";

        GLint lightTypeLocation = glGetUniformLocation(m_shader, (lightBase + ".type").c_str());

        switch(metaData.lights[i].type){
        case LightType::LIGHT_DIRECTIONAL:
            glUniform1i(lightTypeLocation, 0);
            break;
        case LightType::LIGHT_SPOT:
            glUniform1i(lightTypeLocation, 1);
            break;
        case LightType::LIGHT_POINT:
            glUniform1i(lightTypeLocation, 2);
            break;
        }

        // Set light position
        GLint lightPosLocation = glGetUniformLocation(m_shader, (lightBase + ".position").c_str());
        glUniform4fv(lightPosLocation, 1, &metaData.lights[i].pos[0]);

        // Set light direction
        GLint lightDirLocation = glGetUniformLocation(m_shader, (lightBase + ".direction").c_str());
        glUniform4fv(lightDirLocation, 1, &metaData.lights[i].dir[0]);

        // Set light intensity/color
        GLint lightIntensityLocation = glGetUniformLocation(m_shader, (lightBase + ".lightColor").c_str());
        glUniform4fv(lightIntensityLocation, 1, &metaData.lights[i].color[0]);

        GLint lightFunctionLocation = glGetUniformLocation(m_shader, (lightBase + ".function").c_str());
        glUniform3fv(lightFunctionLocation, 1, &metaData.lights[i].function[0]);

        GLint lightPenumbraLocation = glGetUniformLocation(m_shader, (lightBase + ".penumbra").c_str());
        glUniform1f(lightPenumbraLocation, metaData.lights[i].penumbra);

        GLint lightAngleLocation = glGetUniformLocation(m_shader, (lightBase + ".angle").c_str());
        glUniform1f(lightAngleLocation, metaData.lights[i].angle);

    }

    GLint c_location = glGetUniformLocation(m_shader, "camera_pos");
    glm::vec4 cam_pos = glm::inverse(viewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0);
    glUniform4fv(c_location, 1, &cam_pos[0]);

    glBindVertexArray(m_vao);

    GLint coneOffset = 0;
    GLint sphereOffset = conepoints.size() / 6;
    GLint cubeOffset = conepoints.size() / 6 + spherepoints.size() / 6;
    GLint cylinderOffset = conepoints.size() / 6 + spherepoints.size() / 6 + cubepoints.size() / 6;


    for (const RenderShapeData& shapeData : metaData.shapes) {
        glm::mat4 modelMatrix = shapeData.ctm; // The model matrix for the shape
        glUniformMatrix4fv(m_location, 1, GL_FALSE, &modelMatrix[0][0]);

        SceneMaterial material = shapeData.primitive.material;
        glm::vec4 ambient = material.cAmbient * metaData.globalData.ka;
        glUniform4fv(mka_location, 1, &ambient[0]);

        glm::vec4 diffuse = material.cDiffuse * metaData.globalData.kd;
        glUniform4fv(mkd_location, 1, &diffuse[0]);

        glm::vec4 specular = material.cSpecular * metaData.globalData.ks;
        glUniform4fv(mks_location, 1, &specular[0]);

        float shininess = material.shininess;
        glUniform1f(shi_location, shininess);

        GLint offset = 0;
        GLsizei count = 0;

        switch(shapeData.primitive.type){
            case PrimitiveType::PRIMITIVE_CONE:
                offset = coneOffset;
                count = conepoints.size() / 6;
                break;

            case PrimitiveType::PRIMITIVE_SPHERE:
                offset = sphereOffset;
                count = spherepoints.size() / 6;
                break;

            case PrimitiveType::PRIMITIVE_CUBE:
                offset = cubeOffset;
                count = cubepoints.size() / 6;
                break;

            case PrimitiveType::PRIMITIVE_CYLINDER:
                offset = cylinderOffset;
                count = cylinderpoints.size() / 6;
                break;

            default:
                break;
        }

        // Draw the shape
        glDrawArrays(GL_TRIANGLES, offset, count);

    }

    glBindVertexArray(0);

    // Unbind the shader
    glUseProgram(0);

    finished_drawing = true;
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {
    RenderData temp;
    metaData = temp;
    bool success = SceneParser::parse(settings.sceneFilePath, metaData);
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    if (initialized){
        bindBuffer();
    }
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
