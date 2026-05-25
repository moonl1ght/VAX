#pragma once

#include "luna.h"

namespace vax::input {
struct MouseMoveValue {
    glm::vec2 position;
    glm::vec2 delta;
};

class InputController final {
  public:
    class Observer {
      public:
        InputController* _inputController = nullptr;
    };

    InputController() { _observers.reserve(10); }

    void handleEvent(const SDL_Event& event);

    template <typename TObserver> void addObserver(TObserver* obj) {
        obj->_inputController = this;

        auto mouseMoveWrapper = [](void* inst, const vax::input::MouseMoveValue& value) {
            static_cast<TObserver*>(inst)->onMouseMove(value);
        };

        if (_observerCount < _observers.size()) {
            _observers[_observerCount] = {static_cast<void*>(obj), mouseMoveWrapper};
        } else {
            _observers.push_back({static_cast<void*>(obj), mouseMoveWrapper});
        }

        ++_observerCount;
    }

    void removeObserver(void* obj) {
        for (int i = 0; i < _observerCount; ++i) {
            if (_observers[i].instance == obj) {
                _observers[i] = _observers[_observerCount - 1];
                --_observerCount;
                return;
            }
        }
    }

  private:
    struct ObserverSlot {
        void* instance;
        void (*mouseMoveFunc)(void*, const vax::input::MouseMoveValue&);
    };
    std::vector<ObserverSlot> _observers;
    int _observerCount = 0;
    bool _isLeftButtonDown = false;
};
} // namespace vax::input