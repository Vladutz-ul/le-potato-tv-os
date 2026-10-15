const apps = document.querySelectorAll('.app');
let currentIndex = 0;

function setActiveApp(index) {
    apps[currentIndex].classList.remove('active');
    currentIndex = index;
    apps[currentIndex].classList.add('active');
}

// "Wire" the backend/frontend logic for launching apps
function launchApp() {
    const appName = apps[currentIndex].innerText;
    if (appName === "YouTube") {
        window.location.href = "https://www.youtube.com";
    } else {
        console.log(appName + " selected, but not wired yet.");
    }
}

// Mouse support
apps.forEach((app, index) => {
    app.addEventListener('mouseenter', () => setActiveApp(index));
    app.addEventListener('click', () => {
        setActiveApp(index);
        launchApp();
    });
});

// Keyboard support
window.addEventListener('keydown', (e) => {
    if (e.key === 'ArrowRight') {
        setActiveApp((currentIndex + 1) % apps.length);
    } else if (e.key === 'ArrowLeft') {
        setActiveApp((currentIndex - 1 + apps.length) % apps.length);
    } else if (e.key === 'Enter') {
        launchApp();
    }
});

// Real-time clock update
setInterval(() => {
    const now = new Date();
    document.getElementById('clock').innerText = 
        now.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', hour12: false });
}, 1000);

// Fetch hardware temperature from C++ API
function updateTemp() {
    fetch('/api/temp')
        .then(res => res.json())
        .then(data => {
            document.getElementById('board-temp').innerText = `${data.temp}°C`;
        })
        .catch(err => console.error("Temp error:", err));
}

updateTemp();
setInterval(updateTemp, 5000);