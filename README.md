install deps on macos: conan install . --output-folder=build_conan --build=missing --profile=profile
vulkan: conan install . --output-folder=build --build=missing --profile=profile

win test git

win conan: conan install . --output-folder=build --build=missing --profile=winprofile

need to add sdl3 dep if on window download sdl3 and add set(CMAKE_PREFIX_PATH "E:/SDL3") if on mac use conan