#include "DescriptorSetManager.hpp"

DescriptorSetManager::~DescriptorSetManager() {
    vkDestroyDescriptorPool(_vkStack->device->vkDevice, _descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(_vkStack->device->vkDevice, _globalDescriptorSetLayout, nullptr);
    vkDestroyDescriptorSetLayout(_vkStack->device->vkDevice, _objectDescriptorSetLayout, nullptr);
}

bool DescriptorSetManager::initialize() {
    auto result = createGlobalDescriptorSetLayout();
    if (!result) {
        return false;
    }

    // result = createObjectDescriptorSetLayout();
    return true;
}

std::optional<VkDescriptorSet> DescriptorSetManager::getGlobalDescriptorSet(
    uint32_t frameIndex, Buffer* uniformBuffer, Texture* texture
) {
    if (_globalDescriptorSets.size() == _vkStack->MAX_FRAMES_IN_FLIGHT) {
        return std::make_optional(_globalDescriptorSets[frameIndex]);
    }

    std::vector<VkDescriptorSetLayout> layouts(_vkStack->MAX_FRAMES_IN_FLIGHT, _globalDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(_vkStack->MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    _globalDescriptorSets.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
    auto result = vkAllocateDescriptorSets(
        _vkStack->device->vkDevice, &allocInfo, _globalDescriptorSets.data()
    );
    if (result != VK_SUCCESS) {
        Logger::getInstance().error("Failed to allocate descriptor set!");
        return std::nullopt;
    }

    for (size_t i = 0; i < _vkStack->MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffer->vkBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->textureImageView;
        imageInfo.sampler = texture->sampler->vkSampler;

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = _globalDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = _globalDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(
            _vkStack->device->vkDevice,
            static_cast<uint32_t>(descriptorWrites.size()),
            descriptorWrites.data(),
            0,
            nullptr
        );
    }

    return std::make_optional(_globalDescriptorSets[frameIndex]);
}

bool DescriptorSetManager::createGlobalDescriptorSetLayout() {
    // Binding 0: Uniform Buffer Object (UBO)
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    // Binding 1: Combined Image Sampler
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerLayoutBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        uboLayoutBinding, samplerLayoutBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    auto result = vkCreateDescriptorSetLayout(
        _vkStack->device->vkDevice, &layoutInfo, nullptr, &_globalDescriptorSetLayout
    );
    if (result != VK_SUCCESS) {
        return false;
    }

    return true;
}

bool DescriptorSetManager::createObjectDescriptorSetLayout() {
    // Binding 0: Object Data Buffer (per-object data)
    VkDescriptorSetLayoutBinding objectDataBinding{};
    objectDataBinding.binding = 0;
    objectDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    objectDataBinding.descriptorCount = 1;
    objectDataBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    objectDataBinding.pImmutableSamplers = nullptr;

    std::vector<VkDescriptorSetLayoutBinding> bindings = { objectDataBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    auto result = vkCreateDescriptorSetLayout(
        _vkStack->device->vkDevice, &layoutInfo, nullptr, &_objectDescriptorSetLayout
    );
    if (result != VK_SUCCESS) {
        return false;
    }

    return true;
}
