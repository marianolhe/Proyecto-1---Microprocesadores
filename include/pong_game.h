#ifndef PONG_GAME_H
#define PONG_GAME_H

#include "pong_render.h"
#include "highscores.h"
#include <string>
#include <pthread.h>
#include <queue>
#include <atomic>

// Forward declaration
class PongGame;

// Estructura para pasar datos a los hilos
struct ThreadData {
    PongGame* game;
    int player_id;
};

class PongGame {
private:
    // Datos del juego
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
    
    // Datos para los hilos
    ThreadData playerAData;
    ThreadData playerBData;
    ThreadData aiData;
    
    // Sincronización y eventos
    enum class EventType { P1_UP, P1_DOWN, P2_UP, P2_DOWN };
    std::queue<EventType> queueP1;
    std::queue<EventType> queueP2;
    
    // Mutex y variables de condición POSIX
    pthread_mutex_t mtxQueueP1;
    pthread_mutex_t mtxQueueP2;
    pthread_mutex_t mutex_paddleA;
    pthread_mutex_t mutex_paddleB;
    pthread_mutex_t mutex_start_round;
    
    pthread_cond_t cvP1;
    pthread_cond_t cvP2;
    pthread_cond_t cond_start_round;

    // Hilos POSIX
    pthread_t input_thread;
    pthread_t player1_thread;
    pthread_t player2_thread;
    pthread_t serve_thread;

    // Estados del juego
    std::atomic<bool> resetRequested{false};
    std::atomic<bool> roundInProgress{false};
    std::atomic<bool> isAIEnabled{false};
    float ai_difficulty{1.0f};

    // Funciones wrapper para pthread_create
    static void* inputThreadWrapper(void* arg);
    static void* playerThreadWrapper(void* arg);
    static void* aiThreadWrapper(void* arg);
    static void* serveThreadWrapper(void* arg);

    // Wrappers para CPU vs CPU
    static void* ballThreadWrapper(void* arg);
    static void* cpuPlayerAThreadWrapper(void* arg);
    static void* cpuPlayerBThreadWrapper(void* arg);

    // Métodos de ejecución de hilos
    void inputThread();
    void playerThread(int player_id);
    void aiThread();
    void serveThread();
    // Implementaciones internas usadas en .cpp
    void inputListenerThread();
    void playerAThread();
    void playerBThread();
    void player_keyboard_adapter_thread();
    void serve_manager_thread();
    void ai_opponent_thread();
    
    // Lógica del juego por ticks
    void updatePhysics();
    void checkCollisions();
    void checkScoring();

    // Hilos CPU vs CPU
    pthread_t ball_thread;
    pthread_t cpuA_thread;
    pthread_t cpuB_thread;

    // Sincronización del estado global
    pthread_mutex_t mutex_game_state;
    pthread_cond_t cond_frame_ready;
    
public:
    PongGame();
    ~PongGame();
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