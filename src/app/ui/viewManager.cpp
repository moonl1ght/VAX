#include "viewManager.h"

using namespace vax::ui;

void ViewManager::update(const vax::engine::FrameTime& frameTime) {
    _uiEngine.get().updateUiStart();

    _rootView->update(frameTime);

    _uiEngine.get().updateUiEnd();
}

vax::AppMode ViewManager::getAppMode() const {
    return _rootView->getAppMode();
}