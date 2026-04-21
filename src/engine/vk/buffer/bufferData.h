#pragma once

#include <functional>

namespace vax::vk {
    struct BufferData {
    public:
        using DataDeleter = std::function<void(const void*)>;

        const void* data;

        BufferData(const void* data, DataDeleter dataDeleter) : data(data), _dataDeleter(dataDeleter) {};

        ~BufferData() {
            _dataDeleter(data);
        }

        BufferData(const BufferData& other) = delete;

        BufferData& operator=(const BufferData& other) = delete;

        BufferData(BufferData&& other) noexcept : data(other.data), _dataDeleter(std::move(other._dataDeleter)) {
            other.data = nullptr;
        }

        BufferData& operator=(BufferData&& other) noexcept {
            if (this != &other) {
                data = other.data;
                _dataDeleter = std::move(other._dataDeleter);
                other.data = nullptr;
            }
            return *this;
        }

    private:
        DataDeleter _dataDeleter;
    };
}