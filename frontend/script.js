function startGame() {
    const btn = document.getElementById('startBtn');
    
    btn.style.transition = "all 0.4s";
    btn.innerHTML = `⚠️ BREACH PROTOCOL INITIATED...`;
    btn.disabled = true;

    setTimeout(() => {
        alert(`🚨 CONTAINMENT BREACH IN PROGRESS\n\n` +
              `Welcome, Overseer Omega Black (Level 5)\n\n` +
              `You are now in command of Site-20.\n\n` +
              `To play the full game:\n` +
              `1. Clone this repo\n` +
              `2. Run: python play.py\n\n` +
              `Your save data will be stored locally.`);

        // Reset button
        btn.innerHTML = `
            <span class="icon">⚙︎</span>
            START GAME
            <span class="arrow">→</span>
        `;
        btn.disabled = false;
    }, 900);
}

// Allow pressing Enter or Space
document.addEventListener('keydown', function(e) {
    if (e.key === "Enter" || e.key === " ") {
        startGame();
    }
});
