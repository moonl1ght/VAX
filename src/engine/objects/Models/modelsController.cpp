#include "modelsController.h"

using namespace vax::objects;
using namespace vax::vk;
using namespace vax;

void ModelsController::preloadModels(
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
        switch (modelDescriptor.modelType) {
        case vax::objects::ModelDescriptor::ModelType::MODEL: {
            auto model = _modelLoader.get().loadModel(modelDescriptor.path, modelDescriptor.instancesCount);
            if (model) {
                _drawableModels.push_back(std::move(*model));
                _modelInfos[modelDescriptor.name] = {modelDescriptor, _drawableModels.size() - 1};
            }
            break;
        }
        case vax::objects::ModelDescriptor::ModelType::PRIMITIVE_CUBE: {
            auto primitiveDescriptor = modelDescriptor.primitiveDescriptor;
            auto primitive = _primitivesBuilder.get().createCube(primitiveDescriptor.size, primitiveDescriptor.color);
            if (primitive) {
                _drawableModels.push_back(std::move(*primitive));
                _modelInfos[modelDescriptor.name] = {modelDescriptor, _drawableModels.size() - 1};
            }
            break;
        }
        case vax::objects::ModelDescriptor::ModelType::PRIMITIVE_PLANE: {
            auto primitive = _primitivesBuilder.get().createPlane();
            if (primitive) {
                _drawableModels.push_back(std::move(*primitive));
                _modelInfos[modelDescriptor.name] = {modelDescriptor, _drawableModels.size() - 1};
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
    modelNames.reserve(_modelInfos.size());
    for (const auto& [name, _] : _modelInfos) {
        modelNames.push_back(name);
    }
    return modelNames;
}

std::optional<vax::objects::SceneNode> ModelsController::getSceneNode(const std::string& name) {
    if (auto it = _modelInfos.find(name); it != _modelInfos.end()) {
        auto sceneNode = vax::objects::SceneNode(
            name,
            it->second.modelDescriptor.initialTransform,
            {it->second.modelDescriptor.initialTransform.getModelMatrix()},
            true
        );
        auto drawableModelPtr = &_drawableModels[it->second.modelIndex];
        sceneNode.addDrawableModel(drawableModelPtr);
        return std::optional<vax::objects::SceneNode>(std::in_place, std::move(sceneNode));
    }
    return std::nullopt;
}