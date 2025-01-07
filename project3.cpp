#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <deque>
#include <iostream>

const int WindowWidth = 800;
const int WindowHeight = 600;
const int BlockSize = 20;  // Size of each block (snake part, food, etc.)

// Directions for snake movement
enum Direction { Up, Down, Left, Right };

class SnakeGame {
public:
    SnakeGame();
    void run();

private:
    void processInput();
    void update();
    void render();
    void resetGame();
    bool checkCollisions();

    sf::RenderWindow window;
    sf::RectangleShape snakeBlock;
    sf::RectangleShape food;
    std::deque<sf::Vector2i> snake;  // Snake body (head is at the back)
    Direction direction;
    bool gameOver;
    sf::Clock clock;
    sf::Time moveTime;
    int score;
};

SnakeGame::SnakeGame()
    : window(sf::VideoMode(WindowWidth, WindowHeight), "Snake Game"),
      snakeBlock(sf::Vector2f(BlockSize, BlockSize)),
      food(sf::Vector2f(BlockSize, BlockSize)),
      direction(Right), gameOver(false), score(0) {
    snakeBlock.setFillColor(sf::Color::Green);
    food.setFillColor(sf::Color::Red);

    // Initial position of the snake (3 blocks long)
    snake.push_back(sf::Vector2i(40, 40));
    snake.push_back(sf::Vector2i(20, 40));
    snake.push_back(sf::Vector2i(0, 40));

    // Position food randomly
    food.setPosition(sf::Vector2f(rand() % (WindowWidth / BlockSize) * BlockSize, 
                                  rand() % (WindowHeight / BlockSize) * BlockSize));

    moveTime = sf::seconds(0.1f);  // Snake move interval
}

void SnakeGame::run() {
    while (window.isOpen()) {
        processInput();
        update();
        render();
    }
}

void SnakeGame::processInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up && direction != Down)
                direction = Up;
            else if (event.key.code == sf::Keyboard::Down && direction != Up)
                direction = Down;
            else if (event.key.code == sf::Keyboard::Left && direction != Right)
                direction = Left;
            else if (event.key.code == sf::Keyboard::Right && direction != Left)
                direction = Right;
        }
    }
}

void SnakeGame::update() {
    if (gameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
            resetGame();
        }
        return;
    }

    if (clock.getElapsedTime() >= moveTime) {
        // Move the snake
        sf::Vector2i newHead = snake.front();

        if (direction == Up) newHead.y -= BlockSize;
        if (direction == Down) newHead.y += BlockSize;
        if (direction == Left) newHead.x -= BlockSize;
        if (direction == Right) newHead.x += BlockSize;

        // Check for collision with itself or walls
        if (checkCollisions()) {
            gameOver = true;
            return;
        }

        snake.push_front(newHead);

        // Check if the snake eats the food
        if (newHead == sf::Vector2i(food.getPosition().x, food.getPosition().y)) {
            score++;
            food.setPosition(sf::Vector2f(rand() % (WindowWidth / BlockSize) * BlockSize, 
                                          rand() % (WindowHeight / BlockSize) * BlockSize));
        } else {
            snake.pop_back();  // Remove the last block if no food eaten
        }

        clock.restart();
    }
}

void SnakeGame::render() {
    window.clear();

    // Draw the snake
    for (const auto& block : snake) {
        snakeBlock.setPosition(block.x, block.y);
        window.draw(snakeBlock);
    }

    // Draw the food
    window.draw(food);

    // Draw the score
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font!" << std::endl;
        return;
    }

    sf::Text scoreText("Score: " + std::to_string(score), font, 24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, 10.f);
    window.draw(scoreText);

    // Game Over screen
    if (gameOver) {
        sf::Text gameOverText("GAME OVER! Press Enter to Restart", font, 30);
        gameOverText.setFillColor(sf::Color::White);
        gameOverText.setPosition(WindowWidth / 4, WindowHeight / 2);
        window.draw(gameOverText);
    }

    window.display();
}

void SnakeGame::resetGame() {
    snake.clear();
    snake.push_back(sf::Vector2i(40, 40));
    snake.push_back(sf::Vector2i(20, 40));
    snake.push_back(sf::Vector2i(0, 40));
    direction = Right;
    score = 0;
    gameOver = false;
    food.setPosition(sf::Vector2f(rand() % (WindowWidth / BlockSize) * BlockSize, 
                                  rand() % (WindowHeight / BlockSize) * BlockSize));
}

bool SnakeGame::checkCollisions() {
    sf::Vector2i head = snake.front();

    // Check if the snake collides with walls
    if (head.x < 0 || head.x >= WindowWidth || head.y < 0 || head.y >= WindowHeight)
        return true;

    // Check if the snake collides with itself
    for (size_t i = 1; i < snake.size(); ++i) {
        if (head == snake[i]) {
            return true;
        }
    }

    return false;
}

int main() {
    SnakeGame game;
    game.run();
    return 0;
}
