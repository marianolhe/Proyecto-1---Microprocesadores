#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "pong_render.h"
#include "highscores.h"
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <semaphore.h>

class PongGame {
private:
    PongRenderer renderer;
    HighScoreManager scoreManager;
    
    // Estado del juego
    int scoreP1;
    int scoreP2;
    int paddle1Y;
    int paddle2Y;
    int ballX;
    int ballY;
    int ballSpeedX;
    int ballSpeedY;
    
    std::atomic<bool> gameRunning;
    std::string playerName1;
    std::string playerName2;
    
    // Sincronización
    std::mutex gameMutex;
    sem_t sem_highscore;
    
    // Hilos del Integrante 4
    std::thread renderer_thread;
    std::thread highscore_thread;
    std::thread collision_thread;

public:
    PongGame();
    ~PongGame();
    
    // Métodos principales
    void initializeGame();
    void runDemo();
    void getPlayerNames();
    void runGameWithPlayers();
    void showHighScores();
    void resetBall();
    
    // Métodos que faltaban
    void startGame(int gameMode);
    void handleInput();

private:
    void rendererThread();
    void highscoreThread();
    void collisionThread();
    void processInput();
};

#endif