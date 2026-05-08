#pragma once

#include "luna.h"
#include "drawableModel.h"
#include "shaderUniforms.h"
#include "modelLoader.h"
#include "primitivesBuilder.h"
#include "vkEngine.h"
#include "textureLoader.h"
#include "resourceManager.h"
#include "descriptorSetWriter.h"
#include "renderContext.h"

namespace vax {
    struct SceneUpdateContext {
        float deltaTime;
    };

    class DrawableScene final {
    public:
        DrawableScene(vax::vk::Engine& vkEngine)
            : _vkEngine(vkEngine)
            , _resourceManager(vax::ResourceManager(*vkEngine.device, vkEngine.allocator))
            , _textureLoader(vax::textures::TextureLoader(
                *vkEngine.device, _resourceManager.textureManager(), *vkEngine.commandManager
            ))
            , _modelLoader(vax::objects::ModelLoader(_resourceManager, _textureLoader))
            , _primitivesBuilder(vax::objects::PrimitivesBuilder(_resourceManager.meshManager())) {
        };

        ~DrawableScene() {
            _drawableModels.clear();
            _resourceManager.cleanup();
        };

        DrawableScene(const DrawableScene& other) = delete;
        DrawableScene& operator=(const DrawableScene& other) = delete;
        DrawableScene(DrawableScene&& other) noexcept = delete;
        DrawableScene& operator=(DrawableScene&& other) noexcept = delete;

        void load(VkQueue submitQueue);

        void prepareForDraw(vax::renderer::RenderCallContext renderCallContext);

        void update(vax::SceneUpdateContext sceneUpdateContext);

        bool writeGlobalDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter);

        bool writeFrameDescriptorSet(vax::vk::DescriptorSetWriter& descriptorSetWriter);

        void draw(VkCommandBuffer commandBuffer);

    private:
        vax::utils::Logger _logger = vax::utils::Logger("DrawableScene");
        std::vector<vax::vk::Buffer*> _sceneUniformBuffers;
        std::reference_wrapper<vax::vk::Engine> _vkEngine;
        vax::ResourceManager _resourceManager;
        vax::textures::TextureLoader _textureLoader;
        vax::objects::ModelLoader _modelLoader;
        vax::objects::PrimitivesBuilder _primitivesBuilder;
        UniformBufferObject _ubo;
        std::vector<vax::objects::DrawableModel> _drawableModels;

        bool _needsUpdateMaterialsSSBO = true;
        bool _needsUpdateTexturesSSBO = true;

        vax::renderer::RenderCallContext _renderCallContext;
        vax::SceneUpdateContext _sceneUpdateContext;
    };
}