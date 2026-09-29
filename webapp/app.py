from flask import Flask, render_template, jsonify, request
import langevin_sim as ls
import threading

app = Flask(__name__)

POTENTIAL_MAP = {
    "harmonic" : ls.HarmonicPotential,
    "double_well" : ls.DoubleWellPotential,
    "quartic" : ls.QuarticPotential
}

INTEGRATOR_MAP = {
    "euler_maruyama" : ls.EulerMaruyama,
    "leimkuhler_matthews" : ls.LeimkuhlerMatthews,
    "stochastic_heun": ls.StochasticHeun
}

sim = None
running = False
sim_lock = threading.Lock()

@app.route("/")
def index():
    #assumes a templates/ directory
    return render_template("index.html")

#method is what server side receives
#POST means that someone is sending info to us
@app.route("/start", methods=["POST"])
def start():
    global sim, running
    data = request.get_json(silent=True) or {}

    potential_constructor = POTENTIAL_MAP.get(data.get("potential"), ls.HarmonicPotential)
    integrator_constructor = INTEGRATOR_MAP.get(data.get("integrator"), ls.EulerMaruyama)

    try:
        t = float(data.get("temperature", 1.0))
        n = int(data.get("num_particles", 100))
    except (TypeError, ValueError):
        return jsonify(error="temperature/num_particles must be numbers"), 400

    sim = ls.Simulator(potential_constructor(), integrator_constructor(), 0.01, t, n)
    running = True
    return jsonify(status="started")

@app.route("/stop", methods=["POST"])
def stop():
    global running
    running = False
    #freeze positions? happens on client side/frontend?
    return jsonify(status="stopped")

#receiving parameter update data
@app.route("/update_params", methods=["POST"])
def update_params():
    global sim
    data = request.get_json() #python dict now

    #validate input
    if data is None:
        return jsonify(error="invalid or missing json body"), 400

    with sim_lock:
        if sim is None:
            return jsonify(error="simulation not started"), 400
        if "temperature" in data:
            try:
                t = float(data["temperature"])
            except (TypeError, ValueError):
                return jsonify(error="temperature must be a number"), 400

            t = max(0.1, min(10.0, t)) #clamp to allowed range
            sim.setTemperature(t)
        if "potential" in data:
            potential_constructor = POTENTIAL_MAP.get(data["potential"])
            if potential_constructor is None:
                return jsonify(error=f"unknown potential: {data['potential']}"), 400
            sim.setPotential(potential_constructor())
        if "integrator" in data:
            integrator_constructor = INTEGRATOR_MAP.get(data["integrator"])
            if integrator_constructor is None:
                return jsonify(error=f"unknown integrator: {data['integrator']}"), 400
            sim.setIntegrator(integrator_constructor())
        if "num_particles" in data:
            try:
                n = int(data["num_particles"])
            except (TypeError, ValueError):
                return jsonify(error="num_particles must be a number"), 400

            n = max(1, min(1000, n))
            sim.setParticleCount(n)

    return jsonify(status="updated parameters")

#no method specified, default GET
@app.route("/step")
def step():
    global sim
    with sim_lock:
        if sim is None:
            return jsonify(positions=[])
        if running:
            sim.step()
        pos = sim.currentPositions()
    
    pos_list = [[float(p[0]), float(p[1])] for p in pos] #format it
    return jsonify(positions=pos_list)

if __name__ == "__main__":
    app.run()
    #app.run(debug=True) remove when finalizing