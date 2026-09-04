#include "viewManager.h"

using namespace vax::ui;

void ViewManager::setRootView(std::unique_ptr<View> view) {
    if (_isUpdating) {
        _pendingRootView = std::move(view);
        return;
    }
    _swapRootView(std::move(view));
}

void ViewManager::update(const vax::engine::FrameTime& frameTime) {
    _uiEngine.get().updateUiStart();

    _isUpdating = true;
    _rootView->update(frameTime);
    _isUpdating = false;

    _uiEngine.get().updateUiEnd();

    _rootView->render(frameTime);

    if (_pendingRootView) {
        _swapRootView(std::move(_pendingRootView));
    }
}

vax::AppMode ViewManager::getAppMode() const { return _rootView->getAppMode(); }

void ViewManager::_swapRootView(std::unique_ptr<View> view) {
    _rootView = std::move(view);
    _rootView->linkViewManager(this);
}