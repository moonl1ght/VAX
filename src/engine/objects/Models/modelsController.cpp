#include "modelsController.h"

using namespace vax::objects;
using namespace vax::vk;
using namespace vax;

void ModelsController::preload(
    const std::vector<vax::objects::ModelDescriptor>& modelDescriptors,
    vax::vk::CommandBuffer& commandBuffer,
    VkQueue submitQueue
) {
    uint32_t totalModelsCount = 0;
    for (const auto& modelDescriptor : modelDescriptors) {
        totalModelsCount += modelDescriptor.instancesCount;
        if (totalModelsCount > _maxDrawableInstances) {
            _logger.error(
                "Total models count exceeds the maximum number of drawable instances: " +
                std::to_string(totalModelsCount)
            );
            break;
        }
        size_t currentModelIndex = _drawableModels.size();
        switch (modelDescriptor.modelType) {
        case vax::objects::ModelDescriptor::ModelType::MODEL: {
            if (modelDescriptor.getModelExtension() == vax::objects::ModelDescriptor::ModelExtension::URDF) {
                auto sceneNode = _modelLoader.get().loadSceneModel(*this, modelDescriptor);
                if (sceneNode) {
                    _cachedSceneNodeMap.insert_or_assign(modelDescriptor.name, std::move(*sceneNode));
                }
            } else {
                auto model = _modelLoader.get().loadModel(modelDescriptor.path, modelDescriptor.instancesCount);
                if (model) {
                    model->updateSSBOHandle(currentModelIndex);
                    _drawableModels.push_back(std::move(*model));
                    _modelMap[modelDescriptor.name] = {
                        .modelDescriptor = std::make_optional(modelDescriptor),
                        .modelIndex = _drawableModels.size() - 1,
                    };
                }
            }
            break;
        }
        case vax::objects::ModelDescriptor::ModelType::PRIMITIVE_CUBE: {
            auto primitiveDescriptor = modelDescriptor.primitiveDescriptor;
            auto primitive = _primitivesBuilder.get().createCube(primitiveDescriptor.size, primitiveDescriptor.color);
            if (primitive) {
                primitive->updateSSBOHandle(currentModelIndex);
                _drawableModels.push_back(std::move(*primitive));
                _modelMap[modelDescriptor.name] = {
                    .modelDescriptor = std::make_optional(modelDescriptor),
                    .modelIndex = _drawableModels.size() - 1,
                };
            }
            break;
        }
        case vax::objects::ModelDescriptor::ModelType::PRIMITIVE_PLANE: {
            auto primitive = _primitivesBuilder.get().createPlane();
            if (primitive) {
                primitive->updateSSBOHandle(currentModelIndex);
                _drawableModels.push_back(std::move(*primitive));
                _modelMap[modelDescriptor.name] = {
                    .modelDescriptor = std::make_optional(modelDescriptor),
                    .modelIndex = _drawableModels.size() - 1,
                };
            }
            break;
        }
        }
    }
    commandBuffer.begin();
    for (auto& drawableModel : _drawableModels) {
        vax::objects::MeshPBR::LoadMeshBuffersContext context = {
            .commandBuffer = &commandBuffer, .maxFramesInFlight = vax::MAX_FRAMES_IN_FLIGHT
        };
        drawableModel.loadMesh(context);
    }
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
}

std::vector<std::string> ModelsController::getModelNames() const {
    std::vector<std::string> modelNames;
    modelNames.reserve(_modelMap.size());
    for (const auto& [name, _] : _modelMap) {
        modelNames.push_back(name);
    }
    return modelNames;
}

std::optional<vax::objects::SceneNode> ModelsController::createSceneNodeByModelName(const std::string& name) {
    auto itModelInfo = _modelMap.find(name);
    if (itModelInfo != _modelMap.end()) {
        auto modelDescriptor = itModelInfo->second.modelDescriptor.value_or(vax::objects::ModelDescriptor());
        auto sceneNode = vax::objects::SceneNode(
            name, modelDescriptor.initialTransform, {modelDescriptor.initialTransform.getModelMatrix()}, true
        );
        auto drawableModelPtr = &_drawableModels[itModelInfo->second.modelIndex];
        sceneNode.addDrawableModel(drawableModelPtr);
        return std::optional<vax::objects::SceneNode>(std::in_place, std::move(sceneNode));
    }
    return std::nullopt;
}

std::optional<vax::objects::SceneNode> ModelsController::getPreloadedSceneNodeByName(const std::string& name) {
    auto itCachedSceneNode = _cachedSceneNodeMap.find(name);
    if (itCachedSceneNode != _cachedSceneNodeMap.end()) {
        return std::optional<vax::objects::SceneNode>(std::in_place, std::move(itCachedSceneNode->second));
    }
    return std::nullopt;
}

DrawableModel* ModelsController::addDrawableModel(std::string name, vax::objects::DrawableModel&& drawableModel) {
    size_t modelIndex = _drawableModels.size();
    drawableModel.updateSSBOHandle(modelIndex);
    _drawableModels.push_back(std::move(drawableModel));
    _modelMap[name] = {
        .modelDescriptor = std::nullopt,
        .modelIndex = _drawableModels.size() - 1,
    };
    return &_drawableModels.back();
}

DrawableModel* ModelsController::getDrawableModelByName(const std::string& name) {
    auto itModelInfo = _modelMap.find(name);
    if (itModelInfo != _modelMap.end()) {
        return &_drawableModels[itModelInfo->second.modelIndex];
    }
    return nullptr;
}