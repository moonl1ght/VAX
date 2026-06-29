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
    _globalInstanceCursor = 0;
    for (const auto& modelDescriptor : modelDescriptors) {
        totalModelsCount += modelDescriptor.instancesCount;
        if (totalModelsCount > _maxDrawableInstances) {
            _logger.error(
                "Total models count exceeds the maximum number of drawable instances: " +
                std::to_string(totalModelsCount)
            );
            break;
        }
        auto itModelInfo = _modelMap.find(modelDescriptor.id);
        if (itModelInfo != _modelMap.end()) {
            continue;
        }
        bool isURDF = false;
        switch (modelDescriptor.modelType) {
        case vax::objects::ModelDescriptor::ModelType::MODEL: {
            if (modelDescriptor.getModelExtension() == vax::objects::ModelDescriptor::ModelExtension::URDF) {
                auto sceneNode = _modelLoader.get().loadSceneModel(*this, modelDescriptor);
                if (sceneNode) {
                    _cachedSceneNodeMap.insert_or_assign(modelDescriptor.id, std::move(*sceneNode));
                    isURDF = true;
                }
            } else {
                auto model = _modelLoader.get().loadModel(modelDescriptor.path, modelDescriptor.instancesCount);
                if (model) {
                    _drawableModels.push_back(std::move(*model));
                }
            }
            break;
        }
        case vax::objects::ModelDescriptor::ModelType::PRIMITIVE_CUBE: {
            auto primitiveDescriptor = modelDescriptor.primitiveDescriptor;
            auto primitive = _primitivesBuilder.get().createCube(primitiveDescriptor.size, primitiveDescriptor.color);
            if (primitive) {
                _drawableModels.push_back(std::move(*primitive));
            }
            break;
        }
        case ModelDescriptor::ModelType::PRIMITIVE_PLANE: {
            auto primitive = _primitivesBuilder.get().createPlane();
            if (primitive) {
                _drawableModels.push_back(std::move(*primitive));
            }
            break;
        }
        }
        if (!isURDF) {
            auto instanceCount = modelDescriptor.instancesCount + 10;
            ModelInfo::SSBOChunkInfo ssboChunkInfo = {
                .instanceOffset = _globalInstanceCursor,
                .cursor = 0,
                .maxInstances = instanceCount,
            };
            ModelInfo modelInfo = {
                .modelDescriptor = std::make_optional(modelDescriptor),
                .modelIndex = _drawableModels.size() - 1,
                .ssboChunkInfos = {ssboChunkInfo},
            };
            _modelMap[modelDescriptor.id] = modelInfo;
            _globalInstanceCursor += instanceCount;
        }
    }
    commandBuffer.begin();
    for (auto& drawableModel : _drawableModels) {
        MeshPBR::LoadMeshBuffersContext context = {
            .commandBuffer = &commandBuffer, .maxFramesInFlight = vax::MAX_FRAMES_IN_FLIGHT
        };
        drawableModel.loadMesh(context);
    }
    commandBuffer.end();
    commandBuffer.submitAndWait(submitQueue);
}

std::vector<std::string> ModelsController::getModelIds() const {
    std::vector<std::string> modelIds;
    modelIds.reserve(_modelMap.size());
    for (const auto& [id, _] : _modelMap) {
        modelIds.push_back(id);
    }
    return modelIds;
}

std::optional<SceneNode> ModelsController::createSceneNodeById(const std::string& id, uint32_t instancesCount) {
    auto itModelInfo = _modelMap.find(id);
    if (itModelInfo != _modelMap.end()) {
        auto modelDescriptor = itModelInfo->second.modelDescriptor.value_or(ModelDescriptor());
        auto sceneNode = SceneNode(
            _resourceManager.get().ssboManager(),
            id,
            modelDescriptor.initialTransform,
            {modelDescriptor.initialTransform.getModelMatrix()},
            true
        );
        auto drawableModelPtr = &_drawableModels[itModelInfo->second.modelIndex];
        // sceneNode.addDrawableModel(drawableModelPtr);
        return std::optional<SceneNode>(std::in_place, std::move(sceneNode));
    }
    return std::nullopt;
}

std::optional<SceneNode> ModelsController::getPreloadedSceneNodeById(const std::string& id, uint32_t instancesCount) {
    auto itCachedSceneNode = _cachedSceneNodeMap.find(id);
    if (itCachedSceneNode != _cachedSceneNodeMap.end()) {
        return std::optional<SceneNode>(std::in_place, std::move(itCachedSceneNode->second));
    }
    return std::nullopt;
}

DrawableModelHandle
ModelsController::_addDrawableModel(std::string id, std::string path, DrawableModel&& drawableModel) {
    auto itModelInfo = _modelMap.find(id);
    if (itModelInfo != _modelMap.end()) {
        // TODO: handle multiple instances
        auto chunkIndex = itModelInfo->second.ssboChunkIndex;
        auto ssboChunkInfo = itModelInfo->second.ssboChunkInfos[chunkIndex];
        return DrawableModelHandle{&_drawableModels[itModelInfo->second.modelIndex], ssboChunkInfo.instanceOffset, 1};
    }
    size_t modelIndex = _drawableModels.size();
    _drawableModels.push_back(std::move(drawableModel));
    ModelDescriptor modelDescriptor = {
        .path = path,
        .id = id,
        .modelType = ModelDescriptor::ModelType::MODEL,
        .instancesCount = 1,
    };
    ModelInfo::SSBOChunkInfo ssboChunkInfo = {
        .instanceOffset = _globalInstanceCursor,
        .cursor = 0,
        .maxInstances = 1,
    };
    ModelInfo modelInfo = {
        .modelDescriptor = std::make_optional(modelDescriptor),
        .modelIndex = modelIndex,
        .ssboChunkInfos = {ssboChunkInfo},
    };
    _globalInstanceCursor += 1;
    _modelMap[id] = modelInfo;
    return DrawableModelHandle{&_drawableModels.back(), _globalInstanceCursor, 1};
}

DrawableModel* ModelsController::getDrawableModelById(const std::string& id) {
    auto itModelInfo = _modelMap.find(id);
    if (itModelInfo != _modelMap.end()) {
        return &_drawableModels[itModelInfo->second.modelIndex];
    }
    return nullptr;
}