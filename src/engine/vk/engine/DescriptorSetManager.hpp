#ifndef DescriptorSetManager_hpp
#define DescriptorSetManager_hpp

#include "luna.h"
#include "Device.hpp"
#include "VKEngine.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "ShaderUniforms.h"

class DescriptorWriter {
public:
    DescriptorWriter() {}
    ~DescriptorWriter() {}

    void writeBuffer(Buffer* buffer, uint32_t binding, uint32_t offset = 0);
    void writeTexture(Texture* texture, uint32_t binding, uint32_t offset = 0);

    void updateSet(VkDevice device, VkDescriptorSet descriptorSet);
    void clear();

private:
    std::vector<VkWriteDescriptorSet> _writes;
    std::vector<VkDescriptorBufferInfo> _bufferInfos;
    std::vector<VkDescriptorImageInfo> _imageInfos;
};

class DescriptorSetManager {
public:
    DescriptorSetManager(VKEngine* vkEngine): _vkEngine(vkEngine) {};
    ~DescriptorSetManager();

    DescriptorSetManager(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;

    bool setup();
    std::optional<VkDescriptorSet> getGlobalDescriptorSet(
        uint32_t frameIndex, Buffer* uniformBuffer, Texture* texture
    );
    // std::optional<VkDescriptorSet> getObjectDescriptorSet(uint32_t frameIndex);

    VkDescriptorSetLayout getGlobalDescriptorSetLayout() const { return _globalDescriptorSetLayout; }
    // VkDescriptorSetLayout getObjectDescriptorSetLayout() const { return _objectDescriptorSetLayout; }

private:
    VKEngine* _vkEngine;

    VkDescriptorSetLayout _globalDescriptorSetLayout = VK_NULL_HANDLE;
    // VkDescriptorSetLayout _objectDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _globalDescriptorSets;
    // std::vector<VkDescriptorSet> _objectDescriptorSets;

    bool createGlobalDescriptorSetLayout();
    // bool createObjectDescriptorSetLayout();
    bool createDescriptorPool();
};

#endif // DescriptorSetManager_hpp
