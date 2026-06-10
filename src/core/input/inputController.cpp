#include "inputController.h"

using namespace vax::input;
using namespace vax;

inline vax::input::KeyCode keyCodeFromScancode(SDL_Scancode scancode) {
    switch (scancode) {
    case SDL_SCANCODE_A:
        return vax::input::KeyCode::A;
    case SDL_SCANCODE_B:
        return vax::input::KeyCode::B;
    case SDL_SCANCODE_C:
        return vax::input::KeyCode::C;
    case SDL_SCANCODE_D:
        return vax::input::KeyCode::D;
    case SDL_SCANCODE_E:
        return vax::input::KeyCode::E;
    case SDL_SCANCODE_F:
        return vax::input::KeyCode::F;
    case SDL_SCANCODE_G:
        return vax::input::KeyCode::G;
    case SDL_SCANCODE_H:
        return vax::input::KeyCode::H;
    case SDL_SCANCODE_I:
        return vax::input::KeyCode::I;
    case SDL_SCANCODE_J:
        return vax::input::KeyCode::J;
    case SDL_SCANCODE_K:
        return vax::input::KeyCode::K;
    case SDL_SCANCODE_L:
        return vax::input::KeyCode::L;
    case SDL_SCANCODE_M:
        return vax::input::KeyCode::M;
    case SDL_SCANCODE_N:
        return vax::input::KeyCode::N;
    case SDL_SCANCODE_O:
        return vax::input::KeyCode::O;
    case SDL_SCANCODE_P:
        return vax::input::KeyCode::P;
    case SDL_SCANCODE_Q:
        return vax::input::KeyCode::Q;
    case SDL_SCANCODE_R:
        return vax::input::KeyCode::R;
    case SDL_SCANCODE_S:
        return vax::input::KeyCode::S;
    case SDL_SCANCODE_T:
        return vax::input::KeyCode::T;
    case SDL_SCANCODE_U:
        return vax::input::KeyCode::U;
    case SDL_SCANCODE_V:
        return vax::input::KeyCode::V;
    case SDL_SCANCODE_W:
        return vax::input::KeyCode::W;
    case SDL_SCANCODE_X:
        return vax::input::KeyCode::X;
    case SDL_SCANCODE_Y:
        return vax::input::KeyCode::Y;
    case SDL_SCANCODE_Z:
        return vax::input::KeyCode::Z;
    case SDL_SCANCODE_0:
        return vax::input::KeyCode::_0;
    case SDL_SCANCODE_1:
        return vax::input::KeyCode::_1;
    case SDL_SCANCODE_2:
        return vax::input::KeyCode::_2;
    case SDL_SCANCODE_3:
        return vax::input::KeyCode::_3;
    case SDL_SCANCODE_4:
        return vax::input::KeyCode::_4;
    case SDL_SCANCODE_5:
        return vax::input::KeyCode::_5;
    case SDL_SCANCODE_6:
        return vax::input::KeyCode::_6;
    case SDL_SCANCODE_7:
        return vax::input::KeyCode::_7;
    case SDL_SCANCODE_8:
        return vax::input::KeyCode::_8;
    case SDL_SCANCODE_9:
        return vax::input::KeyCode::_9;
    case SDL_SCANCODE_LEFT:
        return vax::input::KeyCode::arrow_left;
    case SDL_SCANCODE_RIGHT:
        return vax::input::KeyCode::arrow_right;
    case SDL_SCANCODE_UP:
        return vax::input::KeyCode::arrow_up;
    case SDL_SCANCODE_DOWN:
        return vax::input::KeyCode::arrow_down;
    default:
        return vax::input::KeyCode::UNKNOWN;
    }
    return vax::input::KeyCode::UNKNOWN;
}

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
    if (event.type == SDL_EVENT_KEY_DOWN) {
        KeyEvent keyEvent;
        auto key = keyCodeFromScancode(event.key.scancode);
        if (key == KeyCode::UNKNOWN) {
            return;
        }
        uint32_t keyValue = static_cast<uint32_t>(key);
        if (_heldKeys.find(keyValue) == _heldKeys.end()) {
            _heldKeys.insert(keyValue);
            keyEvent = {.state = KeyEvent::State::DOWN, .key = key};
        } else {
            keyEvent = {.state = KeyEvent::State::HOLD, .key = key};
        }
        for (int i = 0; i < _observerCount; ++i) {
            _observers[i].keyEventFunc(_observers[i].instance, keyEvent);
        }
    }
    if (event.type == SDL_EVENT_KEY_UP) {
        auto key = keyCodeFromScancode(event.key.scancode);
        if (key == KeyCode::UNKNOWN) {
            return;
        }
        uint32_t keyValue = static_cast<uint32_t>(key);
        if (_heldKeys.find(keyValue) == _heldKeys.end()) {
            return;
        }
        _heldKeys.erase(keyValue);
        KeyEvent keyEvent = {.state = KeyEvent::State::UP, .key = key};
        for (int i = 0; i < _observerCount; ++i) {
            _observers[i].keyEventFunc(_observers[i].instance, keyEvent);
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