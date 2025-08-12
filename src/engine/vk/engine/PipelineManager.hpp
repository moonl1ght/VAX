#ifndef PipelineManager_hpp
#define PipelineManager_hpp

#include "luna.h"
#include "VKObject.hpp"
#include "Vertex.h"
#include "DescriptorSetManager.hpp"

struct ShaderModuleBuilder {
    std::vector<char> code;

    ShaderModuleBuilder() = default;
    ~ShaderModuleBuilder() = default;

    void readFile(const std::string& filename);

    std::optional<VkShaderModule> build(VkDevice device);
};

struct PipelineBuilder {
    enum class PipelineType {
        RENDER,
        COMPUTE
    };

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    PipelineBuilder(VkPipelineLayout pipelineLayout)
        : pipelineLayout(pipelineLayout) {
    };

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    PipelineBuilder() = default;
    ~PipelineBuilder() = default;

    void addShaderStage(VkShaderStageFlagBits stage, VkShaderModule module, const char* name);
    std::optional<VkPipeline> build(VkDevice device, PipelineType pipelineType);
};

class PipelineManager final : public VKObject {
public:
    PipelineManager(VKEngine* engine, DescriptorSetManager* descriptorSetManager)
        : VKObject(engine)
        , _descriptorSetManager(descriptorSetManager) {
    };

    ~PipelineManager() {
        vkDestroyPipelineLayout(vkEngine->device->vkDevice, _pipelineLayout, nullptr);
        vkDestroyPipeline(vkEngine->device->vkDevice, _pipeline, nullptr);
        vkDestroyPipelineLayout(vkEngine->device->vkDevice, _pipelineDrawBackgroundLayout, nullptr);
        vkDestroyPipeline(vkEngine->device->vkDevice, _pipelineDrawBackground, nullptr);
    };

    bool setup();
    VkPipeline getPipeline() const { return _pipeline; }
    VkPipelineLayout getPipelineLayout() const { return _pipelineLayout; }
    VkPipeline getPipelineDrawBackground() const { return _pipelineDrawBackground; }
    VkPipelineLayout getPipelineDrawBackgroundLayout() const { return _pipelineDrawBackgroundLayout; }

private:
    DescriptorSetManager* _descriptorSetManager;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
    VkPipeline _pipeline = VK_NULL_HANDLE;

    VkPipelineLayout _pipelineDrawBackgroundLayout = VK_NULL_HANDLE;
    VkPipeline _pipelineDrawBackground = VK_NULL_HANDLE;
};

#endif