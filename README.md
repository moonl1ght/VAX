# VAX

Vulkan renderer using C++20.

## Dependencies

Managed via Conan:
- assimp/5.4.3
- nlohmann_json/3.11.3
- glfw/3.4
- glm/1.0.1
- tinyobjloader/2.0.0-rc10
- vk-bootstrap/1.3.296
- imgui/1.92.0

SDL3 is sourced separately (see platform notes below).

## Build

### Windows

SDL3 must be installed via MSYS2:
```bash
pacman -S mingw-w64-ucrt-x86_64-SDL3
```

Install Conan dependencies and configure:
```bash
conan install . --output-folder=build --build=missing --profile=winprofile
```

Then configure and build via CMake (VSCode: **CMake: Configure** then **CMake: Build**), or manually:
```bash
cmake --preset luna
cmake --build build
```

### macOS

```bash
conan install . --output-folder=build --build=missing --profile=profile
cmake --preset luna
cmake --build build
```

## Requirements

- CMake 3.10+
- Conan 2.x
- Vulkan SDK (set `VULKAN_SDK` environment variable)
- GCC 14+ (Windows: MSYS2 ucrt64) or Clang (macOS)
