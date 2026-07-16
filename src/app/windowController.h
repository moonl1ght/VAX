#pragma once

#include "logger.h"
#include "size.h"
#include "window.h"
#include "size.h"

namespace vax {
class WindowController final {
  public:
    WindowController() = default;
    ~WindowController() = default;

    WindowController(const WindowController&) = delete;
    WindowController& operator=(const WindowController&) = delete;
    WindowController(WindowController&&) = delete;
    WindowController& operator=(WindowController&&) = delete;

    void setupPrimaryWindow(vax::math::SizeUI size);

    void setupSecondaryWindow(vax::math::SizeUI size, const std::string& name);

    const vax::vk::Window* getPrimaryWindow() const { return _primaryWindow.get(); }

    vax::vk::Window* getPrimaryWindow() { return _primaryWindow.get(); }

    const vax::vk::Window* getSecondaryWindow() const { return _secondaryWindow.get(); }

    vax::vk::Window* getSecondaryWindow() { return _secondaryWindow.get(); }

    bool isPrimaryWindowSetup() const { return _primaryWindow != nullptr; }

    bool isSecondaryWindowSetup() const { return _secondaryWindow != nullptr; }

  private:
    Logger _logger = Logger("WindowController");

    std::unique_ptr<vax::vk::Window> _primaryWindow;
    std::unique_ptr<vax::vk::Window> _secondaryWindow;
};
}