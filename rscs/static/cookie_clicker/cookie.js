document.addEventListener('DOMContentLoaded', () => {
    let score = 0;

    const cookie = document.getElementById('cookie');
    const scoreDisplay = document.getElementById('score');
    const rstBtn = document.getElementById('rst-btn');

    function getCookie(name) {
        const value = `; ${document.cookie}`;
        const parts = value.split(`; ${name}=`);
        if (parts.length === 2) return parts.pop().split(';').shift();
        return "0";
    }

    function updateScore(newScore) {
        score = newScore;
        scoreDisplay.textContent = `Score: ${score}`;
        document.cookie = `score=${score}; path=/;`;
    }

    score = parseInt(getCookie('score'), 10);
    updateScore(score);

    cookie.addEventListener('click', () => {
        updateScore(score + 1);
    });
    
    rstBtn.addEventListener('click', () => {
        updateScore(0);
    });
    

});