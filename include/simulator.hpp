#pragma once
#include <vector>
#include <memory>
#include "particle.hpp"
#include "potential.hpp"
#include "integrator.hpp"

class Simulator {
private:
    //use unique pointers, prevent copying, and automatic free mem
    std::shared_ptr<Potential> potential;
    std::shared_ptr<Integrator> integrator;
    std::vector<Particle> particles;
    double h;
    double T;

public:
    Simulator(std::shared_ptr<Potential> p, std::shared_ptr<Integrator> i, double h, double T, int num_particles);

    void step();

    std::vector<Eigen::Vector2d> currentPositions() const;

    //The following are for dynamic sliders for actual visual simulator
    void setParticleCount(int n);

    void setTemperature(double newT);

    void setIntegrator(std::shared_ptr<Integrator> newIntegrator);

    void setPotential(std::shared_ptr<Potential> newPotential);
};