#pragma once

#include "logger.h"
#include "rlenv.h"

namespace vax::rl::training {
class TrainingEngine final {
  public:
    TrainingEngine() = default;
    ~TrainingEngine() = default;

    template <typename Environment, typename Agent, typename State, typename Action>
    void train(Environment& environment, Agent& agent, int episodes) {
        _logger.info("Start training session ...");
        _logger.info("Environment: ", environment.name());

        for (int episode = 0; episode < episodes; ++episode) {
            _logger.info("Episode ", episode + 1, "/", episodes);
            environment.setEvalMode(vax::rl::EvalMode::TRAINING);
            environment.reset();
            bool done = false;
            while (!done) {
                State beginState = environment.getState();
                Action action = agent.chooseAction(beginState);
                vax::rl::StepResult stepResult = environment.step(action);
                if (stepResult.finishedWithError) {
                    _logger.error("Episode ", episode + 1, "/", episodes, " finished with error");
                    break;
                }
                State endState = environment.getState();
                agent.update(beginState, action, stepResult.reward, endState, stepResult.done);
                done = stepResult.done;
            }
        }
        environment.reset();
        environment.setEvalMode(vax::rl::EvalMode::EVALUATION);
    }

  private:
    vax::utils::Logger _logger = vax::utils::Logger("TrainingEngine");
};
} // namespace vax::rl::training