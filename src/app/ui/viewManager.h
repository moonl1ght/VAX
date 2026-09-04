#pragma once

#include "appMode.h"
#include "frameTime.h"
#include "uiEngine.h"
#include "view.h"

namespace vax::ui {
class ViewManager final {
  public:
    ViewManager(UIEngine& uiEngine)
        : _uiEngine(uiEngine) {}

    ~ViewManager() = default;

    ViewManager(const ViewManager& other) = delete;
    ViewManager& operator=(const ViewManager& other) = delete;
    ViewManager(ViewManager&& other) noexcept = delete;
    ViewManager& operator=(ViewManager&& other) noexcept = delete;

    void update(const vax::engine::FrameTime& frameTime);

    void setRootView(std::unique_ptr<View> view);

    vax::AppMode getAppMode() const;

    View& rootView() { return *_rootView; }

    const View& rootView() const { return *_rootView; }

  private:
    std::reference_wrapper<UIEngine> _uiEngine;
    std::unique_ptr<View> _rootView = nullptr;
    std::unique_ptr<View> _pendingRootView = nullptr;
    bool _isUpdating = false;

    void _swapRootView(std::unique_ptr<View> view);
};
} // namespace vax::ui