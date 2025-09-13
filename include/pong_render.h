#ifndef PONG_RENDER_H
#define PONG_RENDER_H

#include <iostream>
#include <string>
#include "utils.h"

using namespace std;

const int WIDTH = 80;
const int HEIGHT = 25;
const int PADDLE_HEIGHT = 3;

class PongRenderer {
private:
    int scoreP1;
    int scoreP2;
    int paddle1Y;
    int paddle2Y;
    int ballX;
    int ballY;
    int ballDirX;
    int ballDirY;
    string playerName1;
    string playerName2;

public:
    PongRenderer();
    void updateScores(int p1, int p2);
    void updatePaddles(int p1Y, int p2Y);
    void updateBall(int x, int y, int dirX, int dirY);
    void updatePlayerNames(const string& name1, const string& name2);
    void renderGame();
    void renderScoreBoard();
    void renderCourt();
    void renderPaddles();
    void renderBall();
    void clearScreen();
};

#endif