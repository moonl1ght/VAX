#ifndef DescriptorSetManager_hpp
#define DescriptorSetManager_hpp

#include "luna.h"
#include "Device.hpp"
#include "VKStack.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "VertexUniforms.h"


class DescriptorSetManager {
public:
    DescriptorSetManager(VKStack* vkStack): _vkStack(vkStack) {};
    ~DescriptorSetManager();

    DescriptorSetManager(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;

    bool initialize();
    std::optional<VkDescriptorSet> getGlobalDescriptorSet(
        uint32_t frameIndex, Buffer* uniformBuffer, Texture* texture
    );

    VkDescriptorSetLayout getGlobalDescriptorSetLayout() const { return _globalDescriptorSetLayout; }
    VkDescriptorSetLayout getObjectDescriptorSetLayout() const { return _objectDescriptorSetLayout; }

private:
    VKStack* _vkStack;

    VkDescriptorSetLayout _globalDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout _objectDescriptorSetLayout = VK_NULL_HANDLE;

    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;

    std::vector<VkDescriptorSet> _globalDescriptorSets;

    bool createGlobalDescriptorSetLayout();
    bool createObjectDescriptorSetLayout();
};

#endif // DescriptorSetManager_hpp
