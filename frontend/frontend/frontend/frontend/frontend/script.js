function startGame() {
    alert("🚨 CONTAINMENT BREACH INITIATED\n\nWelcome, Overseer Omega Black.\n\nGame starting...");
    // You can later change this to navigate to your actual game screen
}

document.addEventListener('keydown', e => {
    if (e.key === 'Enter' || e.key === ' ') startGame();
});
