#include "gwTrainingManager.h"

using namespace vax::rl::gw;
using namespace vax::rl::gw::env;
using namespace vax::core::concurrency;

void GWTrainingManager::startTraining(
    ThreadRunner& threadRunner, std::function<TrainingCallback> callback
) {
    std::lock_guard<std::mutex> lock(_trainingMutex);
    if (_isTraining)
        return;

    _isTraining = true;

    std::cout << "Training started on thread: " << std::this_thread::get_id() << std::endl;
    _trainingThread = std::jthread([this, &threadRunner, callback = std::move(callback)](std::stop_token st) mutable {
        std::cout << "Training setup started on thread: " << std::this_thread::get_id() << std::endl;
        _setupTraining(threadRunner, callback);
        _train(threadRunner, callback);
    });
}

void GWTrainingManager::_setupTraining(ThreadRunner& threadRunner, std::function<TrainingCallback>& callback) {
    _trainingEngine = std::make_unique<vax::rl::training::TrainingEngine>();
    _gridWorld = std::make_unique<vax::rl::gw::env::GridWorld>();
    _gridWorld->setEvalModeImpl(vax::rl::EvalMode::TRAINING);
    _gridWorld->load();
}

void GWTrainingManager::_train(ThreadRunner& threadRunner, std::function<TrainingCallback>& callback) {
    threadRunner.runOnThread([callback]() {
        callback(TrainingStatus{ "Training in progress...", false });
    });
    _trainingEngine->train<GridWorld, Agent, State, MoveAction>(*_gridWorld, _gridWorld->getAgent(), 100);
    {
        std::lock_guard<std::mutex> lock(_trainingMutex);
        _isTraining = false;
    }
    threadRunner.runOnThread([callback]() {
        callback(TrainingStatus{ "Training completed!", true });
    });
}