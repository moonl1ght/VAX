#include "viewBuilder.h"

using namespace vax::ui;

std::unique_ptr<RoverView> ViewBuilder::buildRoverView() {
    auto roverView =
        std::make_unique<RoverView>(_uiEngine.get(), _windowController.get(), _renderer.get());
    roverView->load(_engine.get(), _inputController.get());
    return roverView;
}