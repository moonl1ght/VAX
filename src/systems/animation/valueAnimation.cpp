#include "valueAnimation.h"
#include <algorithm>

using namespace vax;

bool ValueAnimation::update(const engine::FrameTime& frameTime) {
    if (_isCompleted)
        return true;
    _isStarted = true;
    _currentDuration += frameTime._deltaTime;
    _calculateInterpolatedValue();
    if (_currentDuration >= _duration) {
        _isCompleted = true;
        return true;
    }
    return false;
}

void ValueAnimation::_calculateInterpolatedValue() {
    switch (_function) {
    case AnimationFunction::LINEAR:
        if (_isReversed) {
            auto interpolatedValue = _startValue - ((_currentDuration / _duration) * (_startValue - _endValue));
            interpolatedValue = std::max(interpolatedValue, _endValue);
            _currentValue = interpolatedValue;
        } else {
            auto interpolatedValue = _startValue + (_currentDuration / _duration) * (_endValue - _startValue);
            interpolatedValue = std::min(interpolatedValue, _endValue);
            _currentValue = interpolatedValue;
        }
    }
}