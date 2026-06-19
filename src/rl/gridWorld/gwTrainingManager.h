#pragma once

#include "gridWorld.h"
#include "threadRunner.h"
#include "trainingEngine.h"
#include <thread>

namespace vax::rl::gw {
struct TrainingStatus {
    std::string message = "";
    bool isCompleted = false;
};

using TrainingCallback = void(TrainingStatus trainingStatus);

class GWTrainingManager final {
  public:
    GWTrainingManager() = default;
    ~GWTrainingManager() = default;

    GWTrainingManager(const GWTrainingManager& other) = delete;
    GWTrainingManager& operator=(const GWTrainingManager& other) = delete;
    GWTrainingManager(GWTrainingManager&& other) noexcept = delete;
    GWTrainingManager& operator=(GWTrainingManager&& other) noexcept = delete;

    void startTraining(vax::core::concurrency::ThreadRunner& threadRunner, std::function<TrainingCallback> callback);

  private:
    vax::utils::Logger _logger = vax::utils::Logger("GWTrainingManager");
    std::unique_ptr<vax::rl::training::TrainingEngine> _trainingEngine;
    std::unique_ptr<vax::rl::gw::env::GridWorld> _gridWorld;

    std::jthread _trainingThread;
    std::mutex _trainingMutex;
    bool _isTraining = false;

    void _setupTraining(vax::core::concurrency::ThreadRunner& threadRunner, std::function<TrainingCallback>& callback);
    void _train(vax::core::concurrency::ThreadRunner& threadRunner, std::function<TrainingCallback>& callback);
};
} // namespace vax::rl::gw