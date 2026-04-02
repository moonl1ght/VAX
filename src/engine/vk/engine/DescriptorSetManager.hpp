#ifndef DescriptorSetManager_hpp
#define DescriptorSetManager_hpp

#include "luna.h"
#include "device.h"
#include "vkEngine.h"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "ShaderUniforms.h"

struct DescriptorLayoutBuilder {
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    void addBinding(uint32_t binding, VkDescriptorType type);
    void clear();
    std::optional<VkDescriptorSetLayout> build(
        VkDevice device,
        VkShaderStageFlags shaderStages,
        void* pNext = nullptr,
        VkDescriptorSetLayoutCreateFlags flags = 0
    );
};

struct DescriptorWriter {
public:
    DescriptorWriter() {}
    ~DescriptorWriter() {}

    void writeBuffer(Buffer* buffer, uint32_t binding, uint32_t offset = 0);
    void writeTexture(Texture* texture, uint32_t binding, uint32_t offset = 0);
    void writeStorageImage(VkImageView imageView, uint32_t binding);

    void updateSet(VkDevice device, VkDescriptorSet descriptorSet);
    void clear();

private:
    std::vector<VkWriteDescriptorSet> _writes;
    std::vector<VkDescriptorBufferInfo> _bufferInfos;
    std::vector<VkDescriptorImageInfo> _imageInfos;
};

class DescriptorSetManager {
public:
    DescriptorSetManager(vax::VkEngine* vkEngine) : _vkEngine(vkEngine) {};
    ~DescriptorSetManager();

    DescriptorSetManager(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;

    bool setup();
    std::optional<VkDescriptorSet> getGlobalDescriptorSet(
        uint32_t frameIndex, Buffer* uniformBuffer, Texture* texture
    );

    std::optional<VkDescriptorSet> getDrawBackgroundDescriptorSet(uint32_t frameIndex);
    // std::optional<VkDescriptorSet> getObjectDescriptorSet(uint32_t frameIndex);

    VkDescriptorSetLayout getGlobalDescriptorSetLayout() const { return _globalDescriptorSetLayout; }
    VkDescriptorSetLayout getDrawBackgroundDescriptorSetLayout() const {
        return _drawBackgroundDescriptorSetLayout;
    }
    // VkDescriptorSetLayout getObjectDescriptorSetLayout() const { return _objectDescriptorSetLayout; }

private:
    vax::VkEngine* _vkEngine;

    VkDescriptorSetLayout _drawBackgroundDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout _globalDescriptorSetLayout = VK_NULL_HANDLE;
    // VkDescriptorSetLayout _objectDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _drawBackgroundDescriptorSets;
    std::vector<VkDescriptorSet> _globalDescriptorSets;
    // std::vector<VkDescriptorSet> _objectDescriptorSets;

    bool createGlobalDescriptorSetLayout();
    bool createDrawBackgroundDescriptorSetLayout();
    // bool createObjectDescriptorSetLayout();
    bool createDescriptorPool();
};

#endif // DescriptorSetManager_hpp
