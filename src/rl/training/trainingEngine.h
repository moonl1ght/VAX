#pragma once

#include "logger.h"
#include "rlenv.h"

namespace vax::rl::training {
 class TrainingEngine final {
  public:
    TrainingEngine() {};
    ~TrainingEngine() = default;

    template <typename Environment, typename Agent, typename State, typename Action>
    void train(Environment& environment, Agent& agent, int episodes) {
        _logger.info("Start training session ...");
        _logger.info("Environment: ", environment.name());

        for (int episode = 0; episode < episodes; ++episode) {
            _logger.info("Episode ", episode + 1, "/", episodes);
            environment.reset();
            bool done = false;
            // while (!done) {
            //     State beginState = environment.getState();
            //     Action action = agent.chooseAction(environment.getState());
            //     vax::rl::StepResult stepResult = environment.step(action);
            //     State endState = environment.getState();
            //     agent.update(beginState, action, stepResult.reward, endState, stepResult.done);
            // }
        }
    }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("TrainingEngine");
};
} // namespace vax::rl::training