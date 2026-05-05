#pragma once

#include "luna.h"
#include "texture.h"
#include "device.h"
#include "commandBuffer.h"
#include "commandManager.h"

namespace vax::textures {
    class TextureWorkerInline final {
    public:
        explicit TextureWorkerInline(
            const vax::vk::Device& device,
            vax::vk::CommandBuffer& commandBuffer
        )
            : _device(device)
            , _commandBuffer(commandBuffer) {
        }

        ~TextureWorkerInline() {}

        TextureWorkerInline(const TextureWorkerInline& other) = delete;
        TextureWorkerInline& operator=(const TextureWorkerInline& other) = delete;
        TextureWorkerInline(TextureWorkerInline&& other) noexcept = delete;
        TextureWorkerInline& operator=(TextureWorkerInline&& other) noexcept = delete;

        void transitionTextureLayout(
            Texture& texture,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageAspectFlags aspectMask
        );

    private:
        std::reference_wrapper<const vax::vk::Device> _device;
        std::reference_wrapper<vax::vk::CommandBuffer> _commandBuffer;
    };

    class TextureWorker final {
    public:
        explicit TextureWorker(
            const vax::vk::Device& device,
            vax::vk::CommandManager& commandManager
        )
            : _device(device)
            , _commandManager(commandManager) {
        }

        ~TextureWorker() {}

        TextureWorker(const TextureWorker& other) = delete;
        TextureWorker& operator=(const TextureWorker& other) = delete;
        TextureWorker(TextureWorker&& other) noexcept = delete;
        TextureWorker& operator=(TextureWorker&& other) noexcept = delete;

        void transitionTextureLayout(
            Texture& texture,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageAspectFlags aspectMask
        );

        void transitionTextureLayoutAndSubmit(
            VkQueue submitQueue,
            Texture& texture,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            VkImageAspectFlags aspectMask
        );

    private:
        std::reference_wrapper<const vax::vk::Device> _device;
        std::reference_wrapper<vax::vk::CommandManager> _commandManager;
    };
};