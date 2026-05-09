#include "inputController.h"

using namespace vax::input;
using namespace vax;

void InputController::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            _isLeftButtonDown = true;
        }
    }
    if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            _isLeftButtonDown = false;
        }
    }
    if (_isLeftButtonDown) {
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            std::cout << "Mouse motion: " << event.motion.x << ", " << event.motion.y << std::endl;
            for (int i = 0; i < _observerCount; ++i) {
                _observers[i].mouseMoveFunc(_observers[i].instance, glm::vec2(event.motion.x, event.motion.y));
            }
        }
    }
}