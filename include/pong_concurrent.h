#ifndef PONG_CONCURRENT_H
#define PONG_CONCURRENT_H

#include "pong_game.h"
#include "highscores.h"
#include "utils.h"
#include <pthread.h>
#include <semaphore.h>
#include <atomic>

// Estructura para compartir estado entre hilos
struct GameState {
    // Estado del juego
    int scoreP1;
    int scoreP2;
    int paddle1Y;
    int paddle2Y;
    int ballX;
    int ballY;
    int ballSpeedX;
    int ballSpeedY;
    std::string playerName1;
    std::string playerName2;
    
    // Control del juego
    std::atomic<bool> gameRunning;
    std::atomic<bool> gamePaused;
    int gameMode; // 0: PvP, 1: PvC, 2: CvC
    
    // Sincronización
    pthread_mutex_t stateMutex;
    pthread_cond_t frameReady;
    sem_t highscoreSemaphore;
    
    // Renderizado
    bool needsRedraw;
};

class PongConcurrent {
private:
    GameState gameState;
    PongRenderer renderer;
    HighScoreManager scoreManager;
    
    // Hilos
    pthread_t renderer_thread;
    pthread_t menu_thread;
    pthread_t highscore_thread;
    pthread_t input_thread;
    pthread_t physics_thread;
    pthread_t collision_thread;
    
    // Funciones de inicialización
    void initializeGameState();
    void initializeSynchronization();
    
public:
    PongConcurrent();
    ~PongConcurrent();
    
    // Funciones principales
    void startConcurrentGame(int mode);
    void stopConcurrentGame();
    
    // Funciones de hilos (static para compatibilidad)
    static void* rendererThread(void* arg);
    static void* menuThread(void* arg);
    static void* highscoreThread(void* arg);
    static void* inputThread(void* arg);
    static void* physicsThread(void* arg);
    static void* collisionThread(void* arg);
    
    // Funciones de ayuda para sincronización
    void lockState();
    void unlockState();
    void waitForFrame();
    void signalFrame();
    void safeAddScore();
    
    // Getters
    GameState& getGameState() { return gameState; }
};

#endif