#include "app.h"
#include "notificationCenter.h"
#include "profiler.h"
#include "renderdoc.h"
#include <SDL3/SDL_events.h>
#include <cuda_runtime.h>

using namespace vax;

namespace {
std::string mib(size_t bytes) { return std::to_string(bytes / (1024 * 1024)) + " MiB"; }

std::string kib(size_t bytes) { return std::to_string(bytes / 1024) + " KiB"; }

std::string dims(const int (&v)[3]) {
    return std::to_string(v[0]) + " x " + std::to_string(v[1]) + " x " + std::to_string(v[2]);
}

std::string yesNo(int value) { return value ? "yes" : "no"; }

std::string version(int packed) { return std::to_string(packed / 1000) + "." + std::to_string((packed % 1000) / 10); }

// Cores per SM is not exposed by the runtime, it has to be looked up per architecture.
int cudaCoresPerSM(int major, int minor) {
    switch ((major << 4) + minor) {
    case 0x50:
    case 0x52:
    case 0x53:
        return 128; // Maxwell
    case 0x60:
        return 64; // Pascal GP100
    case 0x61:
    case 0x62:
        return 128; // Pascal
    case 0x70:
    case 0x72:
    case 0x75:
        return 64; // Volta / Turing
    case 0x80:
        return 64; // Ampere GA100
    case 0x86:
    case 0x87:
    case 0x89:
        return 128; // Ampere GA10x / Ada
    case 0x90:
        return 128; // Hopper
    case 0xa0:
    case 0xa1:
    case 0xc0:
    case 0xc1:
        return 128; // Blackwell
    default:
        return -1;
    }
}

int deviceAttribute(cudaDeviceAttr attr, int deviceIndex) {
    int value = 0;
    if (cudaDeviceGetAttribute(&value, attr, deviceIndex) != cudaSuccess) {
        return -1;
    }
    return value;
}
} // namespace

bool App::run() {
    if (!_setup()) {
        return false;
    }
    try {
        _mainLoop();
        _cleanup();
    } catch (const std::exception& e) {
        _logger.error("Failed to run app: {}", e.what());
    }
    return true;
}

bool App::_setup() {
    _printGpuInfo();
    if (!SDL_Vulkan_LoadLibrary(NULL)) {
        _logger.error("Failed to load Vulkan library: {}", SDL_GetError());
        return false;
    }
    RenderDoc::init();
    vax::NotificationCenter::getInstance().setup();
    _windowController = std::make_unique<WindowController>();
    _windowController->setupPrimaryWindow(vax::math::SizeUI{1920, 1080});
    if (!_windowController->getPrimaryWindow()->load(true, true)) {
        return false;
    }
    _engine = std::make_unique<vk::Engine>(*_windowController);
    _engine->setup();

    _uiEngine = std::make_unique<ui::UIEngine>(*_engine, *_windowController->getPrimaryWindow());
    _menuView = std::make_unique<ui::MenuView>(*_uiEngine);
    _roverView = std::make_unique<ui::RoverView>(*_uiEngine, *_windowController);
    _trainingView = std::make_unique<ui::TrainingView>(*_uiEngine);

    _renderer = std::make_unique<engine::Renderer>(*_engine, *_uiEngine);
    _renderer->setup();

    return true;
}

void App::_cleanup() {
    _logger.info("Cleaning up...");
    vkDeviceWaitIdle(_engine->device->vkDevice);
    _uiEngine->cleanup();

    _renderer = nullptr;
    _roverView = nullptr;
    _trainingView = nullptr;
    _menuView = nullptr;
    _uiEngine = nullptr;

    _engine->cleanup();

    _windowController->getPrimaryWindow()->destroyWindow();
    if (_windowController->isSecondaryWindowSetup()) {
        _windowController->getSecondaryWindow()->destroyWindow();
    }
    SDL_Quit();
    _logger.info("Cleanup complete!");
}

void App::_mainLoop() {
    if (!_windowController->isPrimaryWindowSetup()) {
        throw std::runtime_error("Window not initialized");
    }
    static bool running = true;
    int i = 0;
    SDL_WindowID primaryWindowID = _windowController->getPrimaryWindow()->getWindowID();

    while (running) {
        SDL_Event event;
        auto processEvent = [&](SDL_Event& event) {
            _inputController.handleEvent(event);
            _uiEngine->processEvents(event);
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                return false;
            } else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
                if (event.window.windowID == primaryWindowID) {
                    running = false;
                    return false;
                } else if (_windowController->isSecondaryWindowSetup()) {
                    if (event.window.windowID == _windowController->getSecondaryWindow()->getWindowID()) {
                        _windowController->getSecondaryWindow()->hide();
                        return true;
                    }
                }
            }
            return true;
        };
        if (_appMode == AppMode::Demo) {
            while (SDL_PollEvent(&event)) {
                if (!processEvent(event)) {
                    break;
                }
            }
            // TODO: fix this
            SDL_Delay(16);
            _loopContinuousUpdate();
        } else if (SDL_WaitEvent(&event)) {
            do {
                processEvent(event);
            } while (SDL_PollEvent(&event));
            _loopByEventUpdate();
        }
        FrameMark;
    }
}

void App::_updateTimestamp() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static uint32_t frameCount = 0;
    static auto previousTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    _frameTime.timestamp = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    _frameTime.deltaTime =
        std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
    _frameTime.fps = 1.0f / _frameTime.deltaTime;
    ++frameCount;
    _frameTime.frameCount = frameCount;
    previousTime = currentTime;
}

void App::_loopByEventUpdate() {
    ZoneScoped;

    _uiEngine->updateUiStart();
    _menuView->updateImGui();
    _trainingView->updateImGui();
    _uiEngine->updateUiEnd();

    _checkActions();

    _updateTimestamp();

    bool renderResult = false;
    renderResult = _renderer->render(nullptr, _frameTime);

    if (!renderResult) {
        _logger.error("Failed to render scene!");
    }
}

void App::_loopContinuousUpdate() {
    static bool firstTime = true;
    ZoneScoped;

    _roverView->updateImGui();

    _checkActions();

    _updateTimestamp();

    bool renderResult = false;
    vax::engine::SceneUpdateContext sceneUpdateContext{.frameTime = _frameTime};
    if (firstTime) {
        _renderer->prepare(_roverView->drawableScene());
        firstTime = false;
    }
    _roverView->drawableScene()->update(sceneUpdateContext);

    renderResult = _renderer->render(_roverView->drawableScene(), _frameTime);

    if (!renderResult) {
        _logger.error("Failed to render scene!");
    }
}

void App::_checkActions() {
    auto menuViewAction = _menuView->popPendingAction();

    if (menuViewAction) {
        switch (menuViewAction.value()) {
        case ui::MenuView::Action::SHOW_ROVER_DEMO:
            _appMode = AppMode::Demo;
            _roverView->load(*_engine.get(), _inputController);
            break;
        case ui::MenuView::Action::TRAIN_Q_LEARNING:
            _trainingView->startTraining();
            _appMode = AppMode::Training;
            break;
        }
    }
}

void App::_printGpuInfo() const {
    int deviceCount = 0;
    auto countResult = cudaGetDeviceCount(&deviceCount);
    if (countResult != cudaSuccess) {
        _logger.error("cudaGetDeviceCount failed: ", cudaGetErrorString(countResult));
        return;
    }
    if (deviceCount == 0) {
        _logger.warning("No CUDA capable devices found");
        return;
    }

    int driverVersion = 0;
    int runtimeVersion = 0;
    cudaDriverGetVersion(&driverVersion);
    cudaRuntimeGetVersion(&runtimeVersion);

    _logger.info("===== CUDA =====");
    _logger.info("Driver version:               ", version(driverVersion));
    _logger.info("Runtime version:              ", version(runtimeVersion));
    _logger.info("Device count:                 ", deviceCount);

    int previousDevice = 0;
    cudaGetDevice(&previousDevice);

    for (int i = 0; i < deviceCount; ++i) {
        cudaDeviceProp prop{};
        auto propResult = cudaGetDeviceProperties(&prop, i);
        if (propResult != cudaSuccess) {
            _logger.error("cudaGetDeviceProperties failed for device ", i, ": ", cudaGetErrorString(propResult));
            continue;
        }

        int coresPerSM = cudaCoresPerSM(prop.major, prop.minor);
        int clockKHz = deviceAttribute(cudaDevAttrClockRate, i);
        int memoryClockKHz = deviceAttribute(cudaDevAttrMemoryClockRate, i);
        long long residentThreads = static_cast<long long>(prop.multiProcessorCount) * prop.maxThreadsPerMultiProcessor;

        _logger.info("----- Device ", i, ": ", prop.name, " -----");
        _logger.info("Compute capability:           ", prop.major, ".", prop.minor);
        _logger.info("PCI (domain:bus:device):      ", prop.pciDomainID, ":", prop.pciBusID, ":", prop.pciDeviceID);
        _logger.info("Integrated:                   ", yesNo(prop.integrated));
        _logger.info(
            "Multi-GPU board:              ", yesNo(prop.isMultiGpuBoard), " (group ", prop.multiGpuBoardGroupID, ")"
        );

        _logger.info("-- Compute --");
        _logger.info("Multiprocessors (SM):         ", prop.multiProcessorCount);
        _logger.info("Warp size:                    ", prop.warpSize, " threads");
        _logger.info("Max threads per block:        ", prop.maxThreadsPerBlock);
        _logger.info("Max threads per SM:           ", prop.maxThreadsPerMultiProcessor);
        _logger.info("Max blocks per SM:            ", prop.maxBlocksPerMultiProcessor);
        _logger.info("Max resident threads:         ", residentThreads);
        _logger.info("Max resident warps per SM:    ", prop.maxThreadsPerMultiProcessor / prop.warpSize);
        if (coresPerSM > 0) {
            _logger.info("CUDA cores per SM:            ", coresPerSM);
            _logger.info("CUDA cores total:             ", coresPerSM * prop.multiProcessorCount);
        } else {
            _logger.info("CUDA cores per SM:            unknown for this architecture");
        }
        _logger.info("Max block dimensions:         ", dims(prop.maxThreadsDim));
        _logger.info("Max grid dimensions:          ", dims(prop.maxGridSize));
        _logger.info(
            "GPU clock rate:               ", clockKHz < 0 ? "unavailable" : std::to_string(clockKHz / 1000) + " MHz"
        );

        _logger.info("-- Memory --");
        _logger.info("Total global memory:          ", mib(prop.totalGlobalMem));
        size_t freeMemory = 0;
        size_t totalMemory = 0;
        if (cudaSetDevice(i) == cudaSuccess && cudaMemGetInfo(&freeMemory, &totalMemory) == cudaSuccess) {
            _logger.info("Free / total global memory:   ", mib(freeMemory), " / ", mib(totalMemory));
        }
        _logger.info("Memory bus width:             ", prop.memoryBusWidth, " bit");
        if (memoryClockKHz > 0) {
            double bandwidth = memoryClockKHz * 1000.0 * (prop.memoryBusWidth / 8.0) * 2.0 / 1.0e9;
            _logger.info("Memory clock rate:            ", memoryClockKHz / 1000, " MHz");
            _logger.info("Peak memory bandwidth:        ", static_cast<int>(bandwidth), " GB/s");
        }
        _logger.info("L2 cache size:                ", kib(static_cast<size_t>(prop.l2CacheSize)));
        _logger.info("Persisting L2 max size:       ", kib(static_cast<size_t>(prop.persistingL2CacheMaxSize)));
        _logger.info("Shared memory per block:      ", kib(prop.sharedMemPerBlock));
        _logger.info("Shared mem per block (optin): ", kib(prop.sharedMemPerBlockOptin));
        _logger.info("Shared memory per SM:         ", kib(prop.sharedMemPerMultiprocessor));
        _logger.info("Reserved shared mem/block:    ", prop.reservedSharedMemPerBlock, " B");
        _logger.info("Constant memory:              ", kib(prop.totalConstMem));
        _logger.info("Registers per block:          ", prop.regsPerBlock);
        _logger.info("Registers per SM:             ", prop.regsPerMultiprocessor);
        _logger.info("Max memcpy pitch:             ", mib(prop.memPitch));
        _logger.info("Texture alignment:            ", prop.textureAlignment, " B");
        _logger.info("ECC enabled:                  ", yesNo(prop.ECCEnabled));

        _logger.info("-- Capabilities --");
        _logger.info("Concurrent kernels:           ", yesNo(prop.concurrentKernels));
        _logger.info("Async engines (copy):         ", prop.asyncEngineCount);
        _logger.info("Cooperative launch:           ", yesNo(prop.cooperativeLaunch));
        _logger.info("Cluster launch:               ", yesNo(prop.clusterLaunch));
        _logger.info("Stream priorities:            ", yesNo(prop.streamPrioritiesSupported));
        _logger.info("Unified addressing:           ", yesNo(prop.unifiedAddressing));
        _logger.info("Managed memory:               ", yesNo(prop.managedMemory));
        _logger.info("Concurrent managed access:    ", yesNo(prop.concurrentManagedAccess));
        _logger.info("Pageable memory access:       ", yesNo(prop.pageableMemoryAccess));
        _logger.info("Can map host memory:          ", yesNo(prop.canMapHostMemory));
        _logger.info("Host register supported:      ", yesNo(prop.hostRegisterSupported));
        _logger.info("Memory pools (mallocAsync):   ", yesNo(prop.memoryPoolsSupported));
        _logger.info("Global L1 cache:              ", yesNo(prop.globalL1CacheSupported));
        _logger.info("Local L1 cache:               ", yesNo(prop.localL1CacheSupported));
        _logger.info("Compute preemption:           ", yesNo(prop.computePreemptionSupported));
        _logger.info("GPUDirect RDMA:               ", yesNo(prop.gpuDirectRDMASupported));
        _logger.info("Timeline semaphore interop:   ", yesNo(prop.timelineSemaphoreInteropSupported));
        _logger.info("Unified function pointers:    ", yesNo(prop.unifiedFunctionPointers));
    }

    cudaSetDevice(previousDevice);
    _logger.info("================");
}