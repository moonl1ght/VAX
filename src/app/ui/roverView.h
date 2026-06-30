#pragma once

#include "uiEngine.h"
#include "drawableScene.h"
#include "gridWorld.h"
#include "inputController.h"
#include "vkEngine.h"

namespace vax::ui {
class RoverView final {
  public:
    RoverView(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}
    ~RoverView() = default;

    RoverView(const RoverView& other) = delete;
    RoverView& operator=(const RoverView& other) = delete;
    RoverView(RoverView&& other) noexcept = delete;
    RoverView& operator=(RoverView&& other) noexcept = delete;

    void updateImGui();
    void load(vax::vk::Engine& engine, vax::InputController& inputController);

    DrawableScene* drawableScene() const { return _drawableScene.get(); }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;

    std::unique_ptr<DrawableScene> _drawableScene;
    std::unique_ptr<vax::rl::GridWorld> _gridWorld;
};
} // namespace vax::ui