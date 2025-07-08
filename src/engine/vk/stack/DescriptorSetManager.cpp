#include "DescriptorSetManager.hpp"

DescriptorSetManager::~DescriptorSetManager() {
    vkDestroyDescriptorPool(_vkStack->device->vkDevice, _descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(_vkStack->device->vkDevice, _globalDescriptorSetLayout, nullptr);
    // vkDestroyDescriptorSetLayout(_vkStack->device->vkDevice, _objectDescriptorSetLayout, nullptr);
}

bool DescriptorSetManager::initialize() {
    if (!createGlobalDescriptorSetLayout()) {
        return false;
    }

    // result = createObjectDescriptorSetLayout();
    return createDescriptorPool();
}

bool DescriptorSetManager::createDescriptorPool() {
    uint32_t uniformBufferCount = 2;
    uint32_t imageSamplerCount = 1;
    uint32_t maxUniformBufferSets = static_cast<uint32_t>(_vkStack->MAX_FRAMES_IN_FLIGHT) * uniformBufferCount;
    uint32_t maxImageSamplerSets = static_cast<uint32_t>(_vkStack->MAX_FRAMES_IN_FLIGHT) * imageSamplerCount;

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxUniformBufferSets },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxImageSamplerSets }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxUniformBufferSets + maxImageSamplerSets;

    if (vkCreateDescriptorPool(_vkStack->device->vkDevice, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
        Logger::getInstance().error("failed to create descriptor pool!");
        return false;
    }

    return true;
}

// std::optional<VkDescriptorSet> DescriptorSetManager::getObjectDescriptorSet(uint32_t frameIndex) {
//     if (_objectDescriptorSets.size() == _vkStack->MAX_FRAMES_IN_FLIGHT) {
//         // std::cout << "Returning existing descriptor set" << std::endl;
//         return std::make_optional(_objectDescriptorSets[frameIndex]);
//     }

//     // std::cout << "Allocating new descriptor set" << std::endl;

//     std::vector<VkDescriptorSetLayout> layouts(_vkStack->MAX_FRAMES_IN_FLIGHT, _objectDescriptorSetLayout);
//     VkDescriptorSetAllocateInfo allocInfo{};
//     allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//     allocInfo.descriptorPool = _descriptorPool;
//     allocInfo.descriptorSetCount = static_cast<uint32_t>(_vkStack->MAX_FRAMES_IN_FLIGHT);
//     allocInfo.pSetLayouts = layouts.data();

//     _objectDescriptorSets.resize(_vkStack->MAX_FRAMES_IN_FLIGHT);
//     auto result = vkAllocateDescriptorSets(
//         _vkStack->device->vkDevice, &allocInfo, _objectDescriptorSets.data()
//     );
//     if (result != VK_SUCCESS) {
//         Logger::getInstance().error("Failed to allocate descriptor set!");
//         return std::nullopt;
//     }

//     return std::make_optional(_objectDescriptorSets[frameIndex]);
// }

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

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
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
        Logger::getInstance().error("Failed to create global descriptor set layout!");
        return false;
    }

    return true;
}

// bool DescriptorSetManager::createObjectDescriptorSetLayout() {
//     VkDescriptorSetLayoutBinding objectDataBinding{};
//     objectDataBinding.binding = 0;
//     objectDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//     objectDataBinding.descriptorCount = 1;
//     objectDataBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//     objectDataBinding.pImmutableSamplers = nullptr;

//     std::vector<VkDescriptorSetLayoutBinding> bindings = { objectDataBinding };

//     VkDescriptorSetLayoutCreateInfo layoutInfo{};
//     layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//     layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
//     layoutInfo.pBindings = bindings.data();

//     auto result = vkCreateDescriptorSetLayout(
//         _vkStack->device->vkDevice, &layoutInfo, nullptr, &_objectDescriptorSetLayout
//     );
//     if (result != VK_SUCCESS) {
//         Logger::getInstance().error("Failed to create object descriptor set layout!");
//         return false;
//     }

//     return true;
// }

// MARK: - DescriptorWriter

void DescriptorWriter::writeBuffer(Buffer* buffer, uint32_t binding, uint32_t offset) {
    VkDescriptorBufferInfo& bufferInfo = _bufferInfos.emplace_back(
        VkDescriptorBufferInfo {
            .buffer = buffer->vkBuffer,
            .offset = offset,
            .range = buffer->size
        }
    );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .pBufferInfo = &bufferInfo
    };

    _writes.push_back(write);
}

void DescriptorWriter::writeTexture(Texture* texture, uint32_t binding, uint32_t offset) {
    VkDescriptorImageInfo& imageInfo = _imageInfos.emplace_back(
        VkDescriptorImageInfo {
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .imageView = texture->textureImageView,
            .sampler = texture->sampler->vkSampler
        }
    );

    VkWriteDescriptorSet write{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = VK_NULL_HANDLE,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .pImageInfo = &imageInfo
    };

    _writes.push_back(write);
}

void DescriptorWriter::updateSet(VkDevice device, VkDescriptorSet descriptorSet) {
    for (auto& write : _writes) {
        write.dstSet = descriptorSet;
    }

    vkUpdateDescriptorSets(device, static_cast<uint32_t>(_writes.size()), _writes.data(), 0, nullptr);
}

void DescriptorWriter::clear() {
    _writes.clear();
    _imageInfos.clear();
    _bufferInfos.clear();
}