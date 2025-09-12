#include "pong_game.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

PongGame::PongGame() {
    srand(time(0));
    initializeGame();
}

void PongGame::initializeGame() {
    scoreP1 = 0;
    scoreP2 = 0;
    paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
    ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
    gameRunning = true;
}

void PongGame::runDemo() {
    initializeGame();
    
    for (int i = 0; i < 100 && gameRunning; i++) {
        ballX += ballSpeedX;
        ballY += ballSpeedY;
        
        if (ballY <= 1 || ballY >= HEIGHT - 2) {
            ballSpeedY = -ballSpeedY;
        }
        
        if (ballX <= 3) {
            if (ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
                ballSpeedX = 1;
            } else {
                scoreP2++;
                resetBall();
            }
        }
        
        if (ballX >= WIDTH - 4) {
            if (ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
                ballSpeedX = -1;
            } else {
                scoreP1++;
                resetBall();
            }
        }
        
        if (ballY < paddle1Y + PADDLE_HEIGHT / 2 && paddle1Y > 1) {
            paddle1Y--;
        } else if (ballY > paddle1Y + PADDLE_HEIGHT / 2 && paddle1Y < HEIGHT - PADDLE_HEIGHT - 1) {
            paddle1Y++;
        }
        
        if (ballY < paddle2Y + PADDLE_HEIGHT / 2 && paddle2Y > 1) {
            paddle2Y--;
        } else if (ballY > paddle2Y + PADDLE_HEIGHT / 2 && paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) {
            paddle2Y++;
        }
        
        renderer.updateScores(scoreP1, scoreP2);
        renderer.updatePaddles(paddle1Y, paddle2Y);
        renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        renderer.renderGame();
        
        sleep_for(milliseconds(100)); 
        
        if (kbhit()) {
            int key = getch();
            if (key == 'q' || key == 'Q') {
                gameRunning = false;
            }
        }
    }
    
    cout << "Demo finalizada. Presiona cualquier tecla para continuar...";
    getch();
}

void PongGame::resetBall() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
    ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
}

void PongGame::startGame(int gameMode) {
    // Por implementar en la siguiente fase
}

void PongGame::updatePhysics() {
    // Por implementar
}

void PongGame::handleInput() {
    // Por implementar
}