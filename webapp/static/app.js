const canvas = document.getElementById("canvas");
const ctx = canvas.getContext("2d");

const startBtn = document.getElementById("startBtn");
const stopBtn = document.getElementById("stopBtn");
const tempSlider = document.getElementById("temperature");
const tempVal = document.getElementById("temperature_val");
const particlesSlider = document.getElementById("num_particles");
const particlesVal = document.getElementById("num_particles_val");
const potentialSelect = document.getElementById("potential");
const integratorSelect = document.getElementById("integrator");
const zoomInBtn = document.getElementById("zoomInBtn");
const zoomOutBtn = document.getElementById("zoomOutBtn");
const statusText = document.getElementById("status");

let zoom = 3.0;
let isRunning = false;

function worldToScreen(x, y) {
    const scale = (canvas.width / 2) / zoom;
    return {
        sx: canvas.width / 2 + x * scale,
        sy: canvas.height / 2 - y * scale
    };
}

function draw(positions) {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "white";
    for (const [x, y] of positions) {
        const { sx, sy } = worldToScreen(x, y);
        ctx.beginPath();
        ctx.arc(sx, sy, 2, 0, 2 * Math.PI);
        ctx.fill();
    }
}

startBtn.addEventListener("click", async () => {
    await fetch("/start", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
            temperature: parseFloat(tempSlider.value),
            num_particles: parseInt(particlesSlider.value),
            potential: potentialSelect.value,
            integrator: integratorSelect.value
        })
    });
    isRunning = true;
    statusText.textContent = "Status: running";
});

stopBtn.addEventListener("click", async () => {
    await fetch("/stop", { method: "POST" });
    isRunning = false;
    statusText.textContent = "Status: stopped";
});

// run this every 33 ms for 30 fps
async function tick() {
    //avoid wasteful http requests
    if (!isRunning) return;
    const res = await fetch("/step");
    const data = await res.json();
    draw(data.positions);
}
setInterval(tick, 33);

//"input" means listens until user lets go
tempSlider.addEventListener("input", async () => {
    const value = parseFloat(tempSlider.value);
    tempVal.textContent = value;
    await fetch("/update_params", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ temperature: value })
    });
});

particlesSlider.addEventListener("input", async () => {
    const value = parseInt(particlesSlider.value);
    particlesVal.textContent = value;
    await fetch("/update_params", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ num_particles: value})
    });
});

// "change" for dropdowns
potentialSelect.addEventListener("change", async () => {
    await fetch("/update_params", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ potential: potentialSelect.value })
    });
});

integratorSelect.addEventListener("change", async () => {
    await fetch("/update_params", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ "integrator": integratorSelect.value })
    });
});

zoomInBtn.addEventListener("click", () => {
    zoom = Math.max(0.5, zoom - 0.5);
});
zoomOutBtn.addEventListener("click", () => {
    zoom = Math.min(10, zoom + 0.5);
});