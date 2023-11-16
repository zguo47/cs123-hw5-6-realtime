#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>

void helper(SceneNode* node, glm::mat4 ctm, RenderData &renderData){
    std::vector<SceneTransformation *> transformation = node->transformations;

    for (int i = 0; i < transformation.size(); i++) {
        SceneTransformation trans = *transformation[i];

        switch (trans.type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            ctm = glm::translate(ctm, glm::vec3(trans.translate.x, trans.translate.y, trans.translate.z));
            break;
        case TransformationType::TRANSFORMATION_SCALE:
            ctm = glm::scale(ctm, glm::vec3(trans.scale.x, trans.scale.y, trans.scale.z));
            break;
        case TransformationType::TRANSFORMATION_ROTATE:
            ctm = glm::rotate(ctm, trans.angle, glm::vec3(trans.rotate.x, trans.rotate.y, trans.rotate.z));
            break;
        case TransformationType::TRANSFORMATION_MATRIX:
            ctm = ctm * trans.matrix;
            break;
        }
    }


    std::vector<ScenePrimitive *> primtype = node->primitives;

    for (int i = 0; i < primtype.size(); i++) {
        ScenePrimitive primitive = *primtype[i];
        RenderShapeData object;
        object.primitive = primitive;
        object.ctm = ctm;
        renderData.shapes.push_back(object);

    }

    std::vector<SceneLight *> lightdata = node->lights;

    for (int i = 0; i < lightdata.size(); i++) {
        SceneLight light = *lightdata[i];

        SceneLightData transformedLightData;
        transformedLightData.id = light.id;
        transformedLightData.type = light.type;
        transformedLightData.color = light.color;
        transformedLightData.function = light.function;
        transformedLightData.penumbra = light.penumbra;
        transformedLightData.angle = light.angle;
        transformedLightData.width = light.width;
        transformedLightData.height = light.height;

        switch(light.type) {
        case LightType::LIGHT_POINT:
            transformedLightData.pos = ctm * glm::vec4(0, 0, 0, 1.0f);
            break;
        case LightType::LIGHT_DIRECTIONAL:
            transformedLightData.dir = glm::mat4(ctm) * glm::vec4(light.dir);
            break;
        case LightType::LIGHT_SPOT:
            transformedLightData.pos = ctm * glm::vec4(0, 0, 0, 1.0f);
            transformedLightData.dir = glm::mat4(ctm) * glm::vec4(light.dir);
            break;
        }
        renderData.lights.push_back(transformedLightData);

    }
    for (SceneNode* child : node->children) {
        helper(child, ctm, renderData);
    }

}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readJSON();
    if (!success) {
        return false;
    }

    // TODO: Use your Lab 5 code here
    // Task 5: populate renderData with global data, and camera data;
    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData = fileReader.getCameraData();

    // Task 6: populate renderData's list of primitives and their transforms.
    //         This will involve traversing the scene graph, and we recommend you
    //         create a helper function to do so!
    SceneNode* root = fileReader.getRootNode();
    renderData.shapes.clear();

    glm::mat4 ctm = glm::mat4(1.0f);
    helper(root, ctm, renderData);

    return true;
}
