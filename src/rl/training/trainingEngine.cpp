#include "trainingEngine.h"
#include "rlenv.h"

using namespace vax::rl::training;
using namespace vax::rl;
using namespace vax;

template <typename Environment, typename Agent, typename State, typename Action>
void TrainingEngine::train(Environment& environment, Agent& agent, int episodes) {
    _logger.info("Start training session ...");
    _logger.info("Environment: {}", environment.name());

    for (int episode = 0; episode < episodes; ++episode) {
        _logger.info("Episode {}/{}", episode + 1, episodes);
        environment.reset();
        bool done = false;
        while (!done) {
            State beginState = environment.getState();
            Action action = agent.chooseAction(environment.state());
            StepResult stepResult = environment.step(action);
            State endState = environment.getState();
            agent.update(beginState, action, stepResult.reward, endState, stepResult.done);
        }
    }
}