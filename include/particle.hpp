#pragma once

#include <Eigen/Dense>

struct Particle {
    Eigen::Vector2d pos = Eigen::Vector2d::Zero();
    Eigen::Vector2d prev_noise = Eigen::Vector2d::Zero();
};