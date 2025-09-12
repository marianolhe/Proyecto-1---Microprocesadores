#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "pong_render.h"

class PongGame {
private:
    PongRenderer renderer;
    int scoreP1;
    int scoreP2;
    int paddle1Y;
    int paddle2Y;
    int ballX;
    int ballY;
    int ballSpeedX;
    int ballSpeedY;
    bool gameRunning;
    
public:
    PongGame();
    void initializeGame();
    void runDemo();
    void startGame(int gameMode);
    void updatePhysics();
    void handleInput();
    void checkCollisions();
    void checkScoring();
    void resetBall();
};

#endif