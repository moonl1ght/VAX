#include "gwTrainingManager.h"
#include "fileUtils.h"
#include "nlohmann/json.hpp"
#include <filesystem>

using namespace vax::rl::gw;
using namespace vax::rl::gw::env;
using namespace vax::core::concurrency;
using namespace vax::core::utils;

GWTrainingManager::GWTrainingManager() {
    _trainDirectory = RELATIVE_PATH("output/qlearning/train_" + getCurrentDatetimeString());
    std::filesystem::create_directories(_trainDirectory);
}

void GWTrainingManager::startTraining(ThreadRunner& threadRunner, std::function<TrainingCallback> callback) {
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
    _gridWorld = std::make_unique<vax::rl::gw::env::GridWorld>(_qlConfig);
    _gridWorld->setEvalModeImpl(vax::rl::EvalMode::TRAINING);
    _gridWorld->createRandomGrid();
}

void GWTrainingManager::_train(ThreadRunner& threadRunner, std::function<TrainingCallback>& callback) {
    threadRunner.runOnThread([callback]() { callback(TrainingStatus{"Training in progress...", false}); });
    _trainingEngine->train<GridWorld, Agent, State, MoveAction>(
        *_gridWorld, _gridWorld->getAgent(), _qlConfig.episodes
    );
    {
        std::lock_guard<std::mutex> lock(_trainingMutex);
        _isTraining = false;
    }
    _saveTrainingData();
    threadRunner.runOnThread([callback]() { callback(TrainingStatus{"Training completed!", true}); });
}

void GWTrainingManager::_saveTrainingData() {
    auto path = _trainDirectory + "/config.json";
    nlohmann::json config;
    config["learningRate"] = _qlConfig.learningRate;
    config["gamma"] = _qlConfig.gamma;
    config["epsilon"] = _qlConfig.epsilon;
    config["episodes"] = _qlConfig.episodes;
    std::ofstream outFile(path);
    outFile << config.dump(4);
    outFile.close();

    _gridWorld->save(_trainDirectory);
    _gridWorld->load(_trainDirectory);
}