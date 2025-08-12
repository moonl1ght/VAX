#ifndef VAXSize_hpp
#define VAXSize_hpp

#include <iostream>

template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

namespace vax {
    template<typename T>
        requires arithmetic<T>
    struct Size_t {
        T width;
        T height;

        static Size_t zero() { return Size_t(0, 0); }

        Size_t(T width, T height) : width(width), height(height) {}

        double whRatio() const noexcept { return static_cast<double>(width) / static_cast<double>(height); }
        double hwRatio() const noexcept { return static_cast<double>(height) / static_cast<double>(width); }

        VkExtent2D toExtent2D() const noexcept { 
            return VkExtent2D{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; 
        }
    };

    typedef Size_t<uint32_t> SizeUI;
    typedef Size_t<double> SizeD;
    typedef Size_t<float> SizeF;
    typedef SizeD Size;
}

#endif