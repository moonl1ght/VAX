#pragma once

namespace vax::rl::ql {
struct QLearningConfig {
    double learningRate = 0.1;
    double gamma = 0.9;
    double epsilon = 0.3;
    int episodes = 1000;
};
} // namespace vax::rl::ql