#pragma once

#include "luna.h"
#include "resourceHandle.h"
#include "sampler.h"
#include "vaxMath.h"

namespace vax::vk {
class TextureManager;
class TextureFactory;
} // namespace vax::vk

namespace vax::vk {
class Texture final {
  public:
    friend class vax::vk::TextureManager;
    friend class vax::vk::TextureFactory;

    Texture(const vax::vk::Device& device, VmaAllocator allocator)
        : _device(device)
        , _allocator(allocator) {};

    Texture(
        const vax::vk::Device& device,
        VmaAllocator allocator,
        std::string name,
        VkImage image,
        VmaAllocation allocation,
        vax::math::SizeUI size,
        VkFormat format,
        VkImageAspectFlags aspectMask
    )
        : _device(device)
        , _allocator(allocator)
        , _name(name)
        , _image(image)
        , _allocation(allocation)
        , _size(size)
        , _format(format)
        , _aspectMask(aspectMask) {}

    Texture(const Texture& other) = delete;
    Texture& operator=(Texture& other) = delete;

    Texture(Texture&& other) noexcept
        : _device(other._device)
        , _allocator(other._allocator)
        , _name(other._name)
        , _image(other._image)
        , _allocation(other._allocation)
        , _size(other._size)
        , _imageView(other._imageView)
        , _format(other._format)
        , _aspectMask(other._aspectMask)
        , _isDetached(other._isDetached)
        , _id(other._id) {
        other._name.clear();
        other._image = VK_NULL_HANDLE;
        other._allocation = VK_NULL_HANDLE;
        other._imageView = VK_NULL_HANDLE;
        other._size = vax::math::SizeUI::zero();
        other._format = VK_FORMAT_UNDEFINED;
        other._aspectMask = VK_IMAGE_ASPECT_NONE;
        other._isDetached = true;
        other._id = NullTextureId;
    }

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            cleanup();
            _device = other._device;
            _allocator = other._allocator;
            _name = other._name;
            _size = other._size;
            _image = other._image;
            _allocation = other._allocation;
            _imageView = other._imageView;
            _format = other._format;
            _aspectMask = other._aspectMask;
            _isDetached = other._isDetached;
            _id = other._id;

            other._name.clear();
            other._image = VK_NULL_HANDLE;
            other._allocation = VK_NULL_HANDLE;
            other._imageView = VK_NULL_HANDLE;
            other._size = vax::math::SizeUI::zero();
            other._format = VK_FORMAT_UNDEFINED;
            other._aspectMask = VK_IMAGE_ASPECT_NONE;
            other._isDetached = true;
            other._id = NullTextureId;
        }
        return *this;
    }

    ~Texture() { cleanup(); }

    void cleanup();

    bool isValid() const;

    void loadImageView(VkImageViewType viewType, uint32_t layerCount, uint32_t levelCount);

    const std::string& name() const { return _name; }

    bool isDetached() const { return _isDetached; }

    TextureId id() const { return _id; }

    VkImage image() const { return _image; }

    VkImageView imageView() const { return _imageView; }

    std::optional<VkDescriptorImageInfo> descriptorImageInfoNoSampler() const;

    std::optional<VkDescriptorImageInfo> descriptorImageInfo(const Sampler& sampler) const;

    vax::math::SizeUI size() const { return _size; }

    uint32_t width() const { return _size.width; }

    uint32_t height() const { return _size.height; }

    void createSampler();

  private:
    vax::Logger _logger = vax::Logger("Texture");
    vax::math::SizeUI _size = vax::math::SizeUI::zero();
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags _aspectMask = VK_IMAGE_ASPECT_NONE;
    std::string _name;
    TextureId _id = NullTextureId;
    VkImage _image = VK_NULL_HANDLE;
    VkImageView _imageView = VK_NULL_HANDLE;
    VmaAllocation _allocation = VK_NULL_HANDLE;
    VmaAllocator _allocator = VK_NULL_HANDLE;
    std::reference_wrapper<const vax::vk::Device> _device;
    bool _isDetached = true;

    void _destroy();
};
} // namespace vax::vk