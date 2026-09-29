#include <pybind11/pybind11.h>
#include <pybind11/eigen.h> //for numpy conversion
#include <pybind11/stl.h> //for vector
#include "potential.hpp"
#include "integrator.hpp"
#include "simulator.hpp"
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(langevin_sim, m) {
    m.doc() = "Langevin dynamics simulator";

    py::class_<Potential, std::shared_ptr<Potential>>(m, "Potential");
    py::class_<HarmonicPotential, Potential, std::shared_ptr<HarmonicPotential>>(m, "HarmonicPotential")
        .def(py::init<>());
    py::class_<DoubleWellPotential, Potential, std::shared_ptr<DoubleWellPotential>>(m, "DoubleWellPotential")
        .def(py::init<>());
    py::class_<QuarticPotential, Potential, std::shared_ptr<QuarticPotential>>(m, "QuarticPotential")
        .def(py::init<>());

    py::class_<Integrator, std::shared_ptr<Integrator>>(m, "Integrator"); //no init because its abstract class
    py::class_<EulerMaruyama, Integrator, std::shared_ptr<EulerMaruyama>>(m, "EulerMaruyama")
        .def(py::init<>());
    py::class_<LeimkuhlerMatthews, Integrator, std::shared_ptr<LeimkuhlerMatthews>>(m, "LeimkuhlerMatthews")
        .def(py::init<>());
    py::class_<StochasticHeun, Integrator, std::shared_ptr<StochasticHeun>>(m, "StochasticHeun")
        .def(py::init<>());

    py::class_<Simulator>(m, "Simulator")
        .def(py::init<std::shared_ptr<Potential>, std::shared_ptr<Integrator>, double, double, int>()) //ctor
        .def("step", &Simulator::step)
        .def("currentPositions", &Simulator::currentPositions)
        .def("setParticleCount", &Simulator::setParticleCount)
        .def("setTemperature", &Simulator::setTemperature)
        .def("setIntegrator", &Simulator::setIntegrator) //smart pointer? check documentation
        .def("setPotential", &Simulator::setPotential);
}