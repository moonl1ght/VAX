#include "windowController.h"

using namespace vax;
using namespace vax::vk;

void WindowController::setupPrimaryWindow(vax::math::SizeUI size) {
    if (_primaryWindow != nullptr) {
        _logger.error("Primary window already setup");
        return;
    }
    _primaryWindow = std::make_unique<vax::vk::Window>(size.width, size.height);
}

void WindowController::setupSecondaryWindow(vax::math::SizeUI size, const std::string& name) {
    if (_secondaryWindow != nullptr) {
        _logger.error("Secondary window already setup");
        return;
    }
    _secondaryWindow = std::make_unique<vax::vk::Window>(size.width, size.height);
    _secondaryWindow->setWindowName(name);
}