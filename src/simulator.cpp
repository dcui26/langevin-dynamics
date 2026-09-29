#include "simulator.hpp"
#include <stdexcept>
#include <utility>
#include <vector>

Simulator::Simulator(std::shared_ptr<Potential> p, std::shared_ptr<Integrator> i, double h, double T, int num_particles)
    : potential(std::move(p)), integrator(std::move(i)), h(h), T(T) {
        particles.resize(num_particles);
    }

void Simulator::step() {
    for (Particle &particle : particles) {
        //update particle via step
        integrator->step(particle, *potential, h, T);
    }
}

//come back to this, if multithreading
std::vector<Eigen::Vector2d> Simulator::currentPositions() const {
    std::vector<Eigen::Vector2d> currpos;
    currpos.reserve(particles.size());
    for (const auto &particle : particles) {
        currpos.push_back(particle.pos);
    }
    return currpos;
}

void Simulator::setParticleCount(int n) {
    //either new particles are added in at origin
    //or some particles are deleted off the back of the vector
    if (n < 0) {
        throw std::invalid_argument("Particle count must be non-negative");
    }
    particles.resize(n);
}

void Simulator::setTemperature(double newT) {
    T = newT;
}

// Note: if switching integrators like LM -> EM -> LM, particle.prev_noise
// may be stale by several steps. This should be statistically negligible since prev_noise
// is a valid N(0,1) sample regardless of when it was drawn, and the process self-heals
// after one step. Not worth resetting on integrator switch.
void Simulator::setIntegrator(std::shared_ptr<Integrator> newIntegrator) {
    integrator = std::move(newIntegrator);
}

void Simulator::setPotential(std::shared_ptr<Potential> newPotential) {
    potential = std::move(newPotential);
}

