# Langevin Dynamics Simulator

A 2D overdamped Langevin dynamics particle simulator, with a C++ physics
core exposed to Python via pybind11, and an interactive Flask + JavaScript/
HTML5 Canvas frontend for real-time visualization.

Simulates `dx = -∇U(x) dt + √(2T) dW` for an ensemble of particles under a
choice of potential energy landscape and numerical integrator, with live
control over temperature, particle count, potential, and integrator.

## Features

- **Potentials**: Harmonic, Double Well (tunable barrier separation),
  Quartic
- **Integrators**: Euler-Maruyama, Leimkuhler-Matthews, Stochastic Heun
  (predictor-corrector)
- **Interactive frontend**: live sliders for temperature/particle count,
  dropdowns for potential/integrator, start/stop controls, zoom
- Polymorphic C++ core — `Potential` and `Integrator` are abstract base
  classes, so new physics can be added without touching simulation logic
- Server-side input validation and state guards (safe against out-of-order
  requests, malformed input, and unstarted-simulation edge cases)

## Architecture

```
src/, include/, bindings/    — C++ simulation core + pybind11 module
CMakeLists.txt, build/       — build system (build/ is gitignored)
webapp/                      — Flask backend + HTML/CSS/JS frontend
python_streamlit/            — earlier Streamlit prototype, used to
                                validate simulation/bindings logic before
                                building the full Flask frontend
```

The C++ core (`Particle`, `Potential`, `Integrator`, `Simulator`) is
independent of Python. `bindings/bindings.cpp` exposes it via pybind11,
using `shared_ptr` (rather than `unique_ptr`) for `Potential`/`Integrator`
ownership, since pybind11 only supports `shared_ptr` as both an argument
and return type across the Python/C++ boundary.

The Flask backend (`webapp/app.py`) holds a single `Simulator` instance and
exposes it over a small set of HTTP endpoints; the frontend
(`webapp/static/app.js`) polls for updated particle positions and renders
them on an HTML canvas.

## Build (C++ core + Python bindings)

Requires: a C++17 compiler, CMake ≥ 3.18, Eigen3 (e.g. `libeigen3-dev` via
apt), and `pybind11` installed in your active Python environment.

```bash
pip install pybind11
mkdir build && cd build
cmake ..
make
```

This builds `langevin_sim.cpython-*.so` and copies it into `webapp/`
automatically (see the `POST_BUILD` step in `CMakeLists.txt`).

## Run (web app)

```bash
cd webapp
pip install -r requirements.txt
python3 app.py
```

Then open `http://127.0.0.1:5000` in a browser.

## Notes

- Simulation stepping is currently driven by client polling (each
  `/step` request both advances and reads the simulation), rather than an
  independent background thread — a reasonable future extension for
  fully decoupling simulation rate from client/network timing.
- Particle count is capped at 1000 and temperature is clamped
  server-side; both are tuned for a 2D interactive visualization, not
  large-scale simulation.
