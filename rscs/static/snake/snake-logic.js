const canvas = document.getElementById('game');
const ctx = canvas.getContext('2d');

canvas.width = 400;
canvas.height = 400;

const gridSize = 20;
let snake = [{ x: 10, y: 10 }];
let direction = { x: 0, y: 0 };
let food = { x: Math.floor(Math.random() * canvas.width / gridSize), y: Math.floor(Math.random() * canvas.height / gridSize) };
let gameInterval;
let started = false;

function drawRect(x, y, color) {
    ctx.fillStyle = color;
    ctx.fillRect(x * gridSize, y * gridSize, gridSize, gridSize);
}

function spawnFood() {
    food.x = Math.floor(Math.random() * canvas.width / gridSize);
    food.y = Math.floor(Math.random() * canvas.height / gridSize);
}

function update() {
    if (!started) return;

    const head = { x: snake[0].x + direction.x, y: snake[0].y + direction.y };

    head.x = (head.x + canvas.width / gridSize) % (canvas.width / gridSize);
    head.y = (head.y + canvas.height / gridSize) % (canvas.height / gridSize);

    if (snake.some(segment => segment.x === head.x && segment.y === head.y)) {
        clearInterval(gameInterval);
        document.getElementById('game-over').style.display = 'block';
        return;
    }

    snake.unshift(head);

    if (head.x === food.x && head.y === food.y) {
        spawnFood();
    } else {
        snake.pop();
    }
}

function render() {
    ctx.fillStyle = '#000';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    drawRect(food.x, food.y, 'red');
    snake.forEach(segment => drawRect(segment.x, segment.y, 'lime'));
}

function gameLoop() {
    update();
    render();
}

function playAgain() {
    snake = [{ x: 10, y: 10 }];
    direction = { x: 0, y: 0 };
    started = false;
    spawnFood();
    document.getElementById('game-over').style.display = 'none';
    gameInterval = setInterval(gameLoop, 100);
}

document.addEventListener('keydown', (event) => {
    const key = event.key;
    if (key === 'ArrowUp' && direction.y === 0) {
        direction = { x: 0, y: -1 };
    } else if (key === 'ArrowDown' && direction.y === 0) {
        direction = { x: 0, y: 1 };
    } else if (key === 'ArrowLeft' && direction.x === 0) {
        direction = { x: -1, y: 0 };
    } else if (key === 'ArrowRight' && direction.x === 0) {
        direction = { x: 1, y: 0 };
    }
    if (key.startsWith('Arrow')) {
        started = true;
    }
});

gameInterval = setInterval(gameLoop, 100);