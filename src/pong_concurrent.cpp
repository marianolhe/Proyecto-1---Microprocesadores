#include "pong_concurrent.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

PongConcurrent::PongConcurrent() {
    initializeGameState();
    initializeSynchronization();
}

PongConcurrent::~PongConcurrent() {
    stopConcurrentGame();
}

void PongConcurrent::initializeGameState() {
    gameState.scoreP1 = 0;
    gameState.scoreP2 = 0;
    gameState.paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    gameState.paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    gameState.ballX = WIDTH / 2;
    gameState.ballY = HEIGHT / 2;
    gameState.ballSpeedX = 1;
    gameState.ballSpeedY = 1;
    gameState.playerName1 = "JUGADOR 1";
    gameState.playerName2 = "JUGADOR 2";
    gameState.gameRunning = false;
    gameState.gamePaused = false;
    gameState.gameMode = 0;
    gameState.needsRedraw = true;
}

void PongConcurrent::initializeSynchronization() {
    pthread_mutex_init(&gameState.stateMutex, NULL);
    pthread_cond_init(&gameState.frameReady, NULL);
    sem_init(&gameState.highscoreSemaphore, 0, 1); // Semáforo binario para highscore
}

// HILO 1: RENDERER_THREAD - Dibujado ASCII y refresco
void* PongConcurrent::rendererThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    auto& state = game->gameState;
    
    cout << "Renderer thread iniciado\n";
    
    while (state.gameRunning) {
        game->lockState();
        
        // Esperar hasta que sea necesario redibujar
        while (!state.needsRedraw && state.gameRunning) {
            game->waitForFrame();
        }
        
        if (state.gameRunning && !state.gamePaused) {
            // Actualizar renderer con el estado actual
            game->renderer.updateScores(state.scoreP1, state.scoreP2);
            game->renderer.updatePaddles(state.paddle1Y, state.paddle2Y);
            game->renderer.updateBall(state.ballX, state.ballY, state.ballSpeedX, state.ballSpeedY);
            game->renderer.updatePlayerNames(state.playerName1, state.playerName2);
            game->renderer.renderGame();
            
            state.needsRedraw = false;
        }
        
        game->unlockState();
        sleep_for(milliseconds(33)); // ~30 FPS
    }
    
    cout << "Renderer thread terminado\n";
    return NULL;
}

// HILO 2: MENU_THREAD - Navegación y selección de modos
void* PongConcurrent::menuThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    
    cout << "Menu thread iniciado\n";
    
    // Este hilo maneja la interfaz de usuario de forma concurrente
    while (game->gameState.gameRunning) {
        // Podemos mostrar notificaciones o menús overlay aquí
        sleep_for(milliseconds(100));
    }
    
    cout << "Menu thread terminado\n";
    return NULL;
}

// HILO 3: HIGHSCORE_THREAD - Persistencia de puntajes
void* PongConcurrent::highscoreThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    
    cout << "Highscore thread iniciado\n";
    
    while (game->gameState.gameRunning) {
        // Esperar señal para guardar puntajes
        sleep_for(milliseconds(500));
        
        // Verificar si hay que guardar puntajes (cuando el juego termina)
        if (!game->gameState.gameRunning) {
            // Adquirir semáforo para acceso seguro al archivo
            sem_wait(&game->gameState.highscoreSemaphore);
            
            // Guardar puntaje de forma segura
            game->safeAddScore();
            
            sem_post(&game->gameState.highscoreSemaphore);
        }
    }
    
    cout << "Highscore thread terminado\n";
    return NULL;
}

// HILO 4: INPUT_THREAD - Entrada de usuario
void* PongConcurrent::inputThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    auto& state = game->gameState;
    
    cout << "Input thread iniciado\n";
    
    while (state.gameRunning) {
        if (kbhit()) {
            int key = getch();
            game->lockState();
            
            // Jugador 1 (W/S)
            if ((state.gameMode == 0 || state.gameMode == 1) && 
                key == 'w' && state.paddle1Y > 1) {
                state.paddle1Y--;
                state.needsRedraw = true;
            } else if ((state.gameMode == 0 || state.gameMode == 1) && 
                       key == 's' && state.paddle1Y < HEIGHT - PADDLE_HEIGHT - 1) {
                state.paddle1Y++;
                state.needsRedraw = true;
            }
            
            // Jugador 2 (flechas) - solo en PvP
            if (state.gameMode == 0) {
                if (key == 27 && kbhit()) {
                    int t2 = getch();
                    if (t2 == 91) {
                        int t3 = getch();
                        if (t3 == 65 && state.paddle2Y > 1) {
                            state.paddle2Y--;
                            state.needsRedraw = true;
                        } else if (t3 == 66 && state.paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) {
                            state.paddle2Y++;
                            state.needsRedraw = true;
                        }
                    }
                }
            }
            
            // Comandos generales
            if (key == 'q' || key == 'Q') {
                state.gameRunning = false;
            } else if (key == 'p' || key == 'P') {
                state.gamePaused = !state.gamePaused;
                state.needsRedraw = true;
            }
            
            game->unlockState();
        }
        
        sleep_for(milliseconds(16)); // ~60Hz para input responsivo
    }
    
    cout << "Input thread terminado\n";
    return NULL;
}

// HILO 5: PHYSICS_THREAD - Movimiento de la pelota
void* PongConcurrent::physicsThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    auto& state = game->gameState;
    
    cout << "Physics thread iniciado\n";
    
    while (state.gameRunning) {
        game->lockState();
        
        if (!state.gamePaused && state.gameRunning) {
            // Mover la pelota
            state.ballX += state.ballSpeedX;
            state.ballY += state.ballSpeedY;
            state.needsRedraw = true;
            
            // Señalizar para el hilo de colisiones
            game->signalFrame();
        }
        
        game->unlockState();
        sleep_for(milliseconds(50)); // 20 FPS para física
    }
    
    cout << "Physics thread terminado\n";
    return NULL;
}

// HILO 6: COLLISION_THREAD - Detección de colisiones (MI PARTE ESPECÍFICA)
void* PongConcurrent::collisionThread(void* arg) {
    PongConcurrent* game = static_cast<PongConcurrent*>(arg);
    auto& state = game->gameState;
    
    cout << "Collision thread iniciado\n";
    
    while (state.gameRunning) {
        game->lockState();
        
        if (!state.gamePaused && state.gameRunning) {
            // COLISIÓN CON BORDES SUPERIOR E INFERIOR
            if (state.ballY <= 1) {
                state.ballY = 1;
                state.ballSpeedY = abs(state.ballSpeedY); // Rebote hacia abajo
                state.needsRedraw = true;
            } else if (state.ballY >= HEIGHT - 2) {
                state.ballY = HEIGHT - 2;
                state.ballSpeedY = -abs(state.ballSpeedY); // Rebote hacia arriba
                state.needsRedraw = true;
            }
            
            // COLISIÓN CON RAQUETA IZQUIERDA (Jugador 1)
            if (state.ballX <= 3 && state.ballX >= 1) {
                if (state.ballY >= state.paddle1Y && state.ballY <= state.paddle1Y + PADDLE_HEIGHT) {
                    // Colisión detectada - calcular ángulo de rebote
                    state.ballX = 4; // Reposicionar para evitar stuck
                    state.ballSpeedX = abs(state.ballSpeedX); // Ir hacia derecha
                    
                    // Ángulo basado en dónde golpeó la raqueta
                    int hitPosition = state.ballY - state.paddle1Y;
                    if (hitPosition == 0) state.ballSpeedY = -2; // Borde superior
                    else if (hitPosition == PADDLE_HEIGHT - 1) state.ballSpeedY = 2; // Borde inferior
                    else if (hitPosition < PADDLE_HEIGHT / 2) state.ballSpeedY = -1; // Mitad superior
                    else state.ballSpeedY = 1; // Mitad inferior
                    
                    state.needsRedraw = true;
                }
            }
            
            // COLISIÓN CON RAQUETA DERECHA (Jugador 2)
            if (state.ballX >= WIDTH - 4 && state.ballX <= WIDTH - 2) {
                if (state.ballY >= state.paddle2Y && state.ballY <= state.paddle2Y + PADDLE_HEIGHT) {
                    // Colisión detectada - calcular ángulo de rebote
                    state.ballX = WIDTH - 5; // Reposicionar
                    state.ballSpeedX = -abs(state.ballSpeedX); // Ir hacia izquierda
                    
                    // Ángulo basado en dónde golpeó la raqueta
                    int hitPosition = state.ballY - state.paddle2Y;
                    if (hitPosition == 0) state.ballSpeedY = -2;
                    else if (hitPosition == PADDLE_HEIGHT - 1) state.ballSpeedY = 2;
                    else if (hitPosition < PADDLE_HEIGHT / 2) state.ballSpeedY = -1;
                    else state.ballSpeedY = 1;
                    
                    state.needsRedraw = true;
                }
            }
            
            // DETECCIÓN DE PUNTUACIÓN
            if (state.ballX <= 0) {
                state.scoreP2++;
                state.ballX = WIDTH / 2;
                state.ballY = HEIGHT / 2;
                state.ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
                state.ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
                state.needsRedraw = true;
                
                // Verificar si el juego terminó
                if (state.scoreP2 >= 5) {
                    state.gameRunning = false;
                }
            } else if (state.ballX >= WIDTH - 1) {
                state.scoreP1++;
                state.ballX = WIDTH / 2;
                state.ballY = HEIGHT / 2;
                state.ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
                state.ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
                state.needsRedraw = true;
                
                // Verificar si el juego terminó
                if (state.scoreP1 >= 5) {
                    state.gameRunning = false;
                }
            }
        }
        
        game->unlockState();
        sleep_for(milliseconds(33)); // ~30 FPS para detección de colisiones
    }
    
    cout << "Collision thread terminado\n";
    return NULL;
}

void PongConcurrent::startConcurrentGame(int mode) {
    initializeGameState();
    gameState.gameRunning = true;
    gameState.gameMode = mode;
    gameState.needsRedraw = true;
    
    // Crear todos los hilos
    pthread_create(&renderer_thread, NULL, &PongConcurrent::rendererThread, this);
    pthread_create(&menu_thread, NULL, &PongConcurrent::menuThread, this);
    pthread_create(&highscore_thread, NULL, &PongConcurrent::highscoreThread, this);
    pthread_create(&input_thread, NULL, &PongConcurrent::inputThread, this);
    pthread_create(&physics_thread, NULL, &PongConcurrent::physicsThread, this);
    pthread_create(&collision_thread, NULL, &PongConcurrent::collisionThread, this);
    
    cout << "Juego concurrente iniciado con " << 6 << " hilos\n";
}

void PongConcurrent::stopConcurrentGame() {
    gameState.gameRunning = false;
    
    // Señalizar a todos los hilos que terminen
    signalFrame();
    
    // Esperar a que todos los hilos terminen
    pthread_join(renderer_thread, NULL);
    pthread_join(menu_thread, NULL);
    pthread_join(highscore_thread, NULL);
    pthread_join(input_thread, NULL);
    pthread_join(physics_thread, NULL);
    pthread_join(collision_thread, NULL);
    
    // Guardar puntaje final de forma segura
    safeAddScore();
}

void PongConcurrent::safeAddScore() {
    sem_wait(&gameState.highscoreSemaphore);
    scoreManager.addScore(gameState.playerName1, gameState.playerName2, 
                         gameState.scoreP1, gameState.scoreP2);
    sem_post(&gameState.highscoreSemaphore);
}

// Funciones de sincronización
void PongConcurrent::lockState() {
    pthread_mutex_lock(&gameState.stateMutex);
}

void PongConcurrent::unlockState() {
    pthread_mutex_unlock(&gameState.stateMutex);
}

void PongConcurrent::waitForFrame() {
    pthread_cond_wait(&gameState.frameReady, &gameState.stateMutex);
}

void PongConcurrent::signalFrame() {
    pthread_cond_broadcast(&gameState.frameReady);
}