#pragma once

namespace vax::rl {
template <typename Derived, typename State, typename Action> class Environment {
  public:
    struct StepResult final {
        State state = 0;
        double reward = 0.0;
        bool done = false;
        bool finishedWithError = false;
    };

    Environment() {};
    ~Environment() = default;

    Environment(const Environment& other) = delete;
    Environment& operator=(const Environment& other) = delete;
    Environment(Environment&& other) noexcept = delete;
    Environment& operator=(Environment&& other) noexcept = delete;

    State reset() { return static_cast<Derived*>(this)->resetImpl(); }

    StepResult step(Action action) { return static_cast<Derived*>(this)->stepImpl(action); }
};
} // namespace vax::rl