#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "pong_render.h"
#include "highscores.h"
#include <string>

class PongGame {
private:
    PongRenderer renderer;
    HighScoreManager scoreManager;
    int scoreP1;
    int scoreP2;
    int paddle1Y;
    int paddle2Y;
    int ballX;
    int ballY;
    int ballSpeedX;
    int ballSpeedY;
    bool gameRunning;
    std::string playerName1;
    std::string playerName2;
    
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
    void getPlayerNames();
    void runGameWithPlayers();
    void showHighScores();
};

#endif