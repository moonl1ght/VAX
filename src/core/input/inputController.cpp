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
    if (event.type == SDL_EVENT_MOUSE_WHEEL) {
        for (int i = 0; i < _observerCount; ++i) {
            _observers[i].mouseWheelFunc(_observers[i].instance, event.wheel.y);
        }
    }
    if (_isLeftButtonDown) {
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            for (int i = 0; i < _observerCount; ++i) {
                MouseMoveValue value = {
                    .position = glm::vec2(event.motion.x, event.motion.y),
                    .delta = glm::vec2(event.motion.xrel, event.motion.yrel)
                };
                _observers[i].mouseMoveFunc(_observers[i].instance, value);
            }
        }
    }
}