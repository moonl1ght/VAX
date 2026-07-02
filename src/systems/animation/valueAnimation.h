#pragma once

#include "frameTime.h"

namespace vax {
enum class AnimationFunction {
    LINEAR,
};

class ValueAnimation final {
  public:
    ValueAnimation(float duration, float startValue, float endValue)
        : _duration(duration)
        , _startValue(startValue)
        , _endValue(endValue) {
        _isReversed = startValue > endValue;
    }

    bool update(const engine::FrameTime& frameTime);

    bool isStarted() const { return _isStarted; }

    bool isCompleted() const { return _isCompleted; }

    bool isInProgress() const { return _isStarted && !_isCompleted; }

    float currentValue() const { return _currentValue; }

    float duration() const { return _duration; }

    void setDuration(float duration) { _duration = duration; }

    void reset() {
        _isStarted = false;
        _isCompleted = false;
        _currentDuration = 0.0f;
        _currentValue = _startValue;
    }

  private:
    AnimationFunction _function = AnimationFunction::LINEAR;

    float _duration = 1.0f;
    float _currentDuration = 0.0f;
    bool _isStarted = false;
    bool _isCompleted = false;

    float _startValue = 0.0f;
    float _endValue = 1.0f;

    float _currentValue = _startValue;
    bool _isReversed;

    void _calculateInterpolatedValue();
};
} // namespace vax