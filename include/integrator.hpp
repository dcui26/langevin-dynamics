#pragma once
#include <cmath>
#include <random>
#include <Eigen/Dense>
#include "particle.hpp"
#include "potential.hpp"

class Integrator {
public:
    virtual ~Integrator() = default;
    //pass a potential by ref is okay, no object slicing
    virtual void step(Particle &particle, const Potential &p, double h, double T) = 0;

protected:
    //use brace initialization
    std::mt19937 gen{std::random_device{}()};
    std::normal_distribution<double> dist{0.0, 1.0};
};

class EulerMaruyama : public Integrator {
public:
    void step(Particle &particle, const Potential &p, double h, double T) override {
        Eigen::Vector2d drift_inc = -p.gradient(particle.pos) * h;
        
        //use reparam trick for increment of brownian motion
        double coeff = std::sqrt(2 * T * h);
        Eigen::Vector2d stochastic_inc(coeff * dist(gen), coeff * dist(gen));

        particle.pos = particle.pos + drift_inc + stochastic_inc;
    }
};

class LeimkuhlerMatthews : public Integrator {
public:
    void step(Particle &particle, const Potential &p, double h, double T) override {
        Eigen::Vector2d drift_inc = -p.gradient(particle.pos) * h;
        double coeff = std::sqrt(2 * T * h) / std::sqrt(2);

        //noise handling
        Eigen::Vector2d curr_noise(dist(gen), dist(gen));
        //prev_noise initially 0
        Eigen::Vector2d stochastic_inc = coeff * (particle.prev_noise + curr_noise);
        particle.prev_noise = curr_noise;

        particle.pos = particle.pos + drift_inc + stochastic_inc;
    }
};

class StochasticHeun : public Integrator {
public:
    void step(Particle &particle, const Potential &p, double h, double T) override {
        double coeff = std::sqrt(2 * T * h);
        Eigen::Vector2d noise(coeff * dist(gen), coeff * dist(gen));

        Eigen::Vector2d drift1 = -p.gradient(particle.pos) * h;
        Eigen::Vector2d predicted = particle.pos + drift1 + noise;

        Eigen::Vector2d drift2 = -p.gradient(predicted) * h;

        particle.pos = particle.pos + 0.5 * (drift1 + drift2) + noise;
    }
};