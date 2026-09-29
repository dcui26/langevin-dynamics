#pragma once
#include <Eigen/Dense>

class Potential {
public:
    virtual ~Potential() = default;
    virtual double energy(const Eigen::Vector2d &pos) const = 0;
    virtual Eigen::Vector2d gradient(const Eigen::Vector2d &pos) const = 0;
};

class HarmonicPotential : public Potential {
public:
    double energy(const Eigen::Vector2d &pos) const override {
        return 0.5*(pos.x() * pos.x() + pos.y() * pos.y());
    }
    Eigen::Vector2d gradient(const Eigen::Vector2d &pos) const override {
        return pos;
    }
};

class DoubleWellPotential : public Potential {
private:
    //wells at +a, -a
    static constexpr double a = 2.0;

public:
    double energy(const Eigen::Vector2d &pos) const override {
        double s = pos.x() * pos.x() - a * a;
        s *= s;
        double t = pos.y() * pos.y() / 2;
        return s + t;
    }
    Eigen::Vector2d gradient(const Eigen::Vector2d &pos) const override {
        double del_x = 4 *pos.x() * (pos.x() * pos.x() - a * a);
        double del_y = pos.y();
        return Eigen::Vector2d(del_x, del_y);
    }
};

class QuarticPotential : public Potential {
public:
    double energy(const Eigen::Vector2d &pos) const override {
        return (std::pow(pos.x(), 4) + std::pow(pos.y(), 4)) / 4.0;
    }
    Eigen::Vector2d gradient(const Eigen::Vector2d &pos) const override {
        return Eigen::Vector2d(std::pow(pos.x(), 3), std::pow(pos.y(), 3));
    }
};