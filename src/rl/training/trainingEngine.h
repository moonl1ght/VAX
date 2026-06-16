#pragma once

#include "logger.h"

namespace vax::rl::training {
class TrainingEngine final {
  public:
    TrainingEngine() {};
    ~TrainingEngine() = default;

    template <typename Environment, typename Agent, typename State, typename Action>
    void train(Environment& environment, Agent& agent, int episodes);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("TrainingEngine");
};
} // namespace vax::rl::training