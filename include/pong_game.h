#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "pong_render.h"
#include "highscores.h"
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

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
    std::atomic<bool> gameRunning;
    std::string playerName1;
    std::string playerName2;
    
    // Sincronización y eventos (JvJ)
    enum class EventType { P1_UP, P1_DOWN, P2_UP, P2_DOWN };
    std::queue<EventType> queueP1;
    std::queue<EventType> queueP2;
    std::mutex mtxQueueP1;
    std::mutex mtxQueueP2;
    std::condition_variable cvP1;
    std::condition_variable cvP2;

    // Mutex por paleta
    std::mutex mutex_paddleA; // protege paddle1Y
    std::mutex mutex_paddleB; // protege paddle2Y

    // Hilos
    std::thread tInput;
    std::thread tPlayerA;
    std::thread tPlayerB;

    // Señales
    std::atomic<bool> resetRequested{false};
    
    // Hilos y lógica auxiliares
    void inputListenerThread();
    void playerAThread();
    void playerBThread();
    
    // Lógica del juego por ticks
    void updatePhysics();
    void checkCollisions();
    void checkScoring();
    
public:
    PongGame();
    void initializeGame();
    void runDemo();
    void startGame(int gameMode);
    void handleInput();
    void resetBall();
    void getPlayerNames();
    void runGameWithPlayers();
    void showHighScores();
};

#endif