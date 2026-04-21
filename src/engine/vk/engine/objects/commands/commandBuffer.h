#include "luna.h"
#include "device.h"

namespace vax::vk {
    class CommandBuffer {
    public:
        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;

        explicit CommandBuffer(
            const vax::vk::Device& device,
            const VkCommandPool commandPool,
            VkCommandBuffer vkCommandBuffer
        ) : _commandPool(commandPool), _device(device), vkCommandBuffer(vkCommandBuffer) {
        };

        ~CommandBuffer() {
            if (_isBegun) {
                end();
            }
            vkFreeCommandBuffers(_device.get().vkDevice, _commandPool, 1, &vkCommandBuffer);
        }

        CommandBuffer(const CommandBuffer& other) = delete;
        CommandBuffer(CommandBuffer&& other) noexcept = delete;
        CommandBuffer& operator=(const CommandBuffer& other) = delete;
        CommandBuffer& operator=(CommandBuffer&& other) noexcept = delete;

        bool begin();
        bool end();
        void submitAndWait(VkQueue queue);

    private:
        const VkCommandPool _commandPool;
        std::reference_wrapper<const vax::vk::Device> _device;
        bool _isBegun = false;
    };
}