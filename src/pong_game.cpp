/****************************************************
 * Archivo: pong_game.cpp
 * Descripción: Controla la lógica principal del juego Pong, incluyendo la inicialización,
 *              simulación de movimiento, detección de colisiones, actualización de puntajes
 *              y renderizado en modo demostración. 
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include "pong_game.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <iostream>
#include <string>

using namespace std;

PongGame::PongGame() {
    srand(time(0));
    playerName1 = "Jugador 1";
    playerName2 = "Jugador 2";
    gameMode = 0;
    
    sem_init(&sem_highscore, 0, 1);
    initializeGame();
}

PongGame::~PongGame() {
    stopThreads();
    sem_destroy(&sem_highscore);
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

void PongGame::startThreads() {
    gameRunning = true;
    renderer_thread = thread(&PongGame::rendererThread, this);
    highscore_thread = thread(&PongGame::highscoreThread, this);
    collision_thread = thread(&PongGame::collisionThread, this);
    input_thread = thread(&PongGame::inputThread, this);
}

void PongGame::stopThreads() {
    gameRunning = false;
    if (renderer_thread.joinable()) renderer_thread.join();
    if (highscore_thread.joinable()) highscore_thread.join();
    if (collision_thread.joinable()) collision_thread.join();
    if (input_thread.joinable()) input_thread.join();
}

// ==================== HILOS INTEGRANTE 4 ====================

void PongGame::rendererThread() {
    while (gameRunning) {
        renderer.updateScores(scoreP1, scoreP2);
        renderer.updatePaddles(paddle1Y, paddle2Y);
        renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        renderer.renderGame();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void PongGame::highscoreThread() {
    while (gameRunning) {
        this_thread::sleep_for(chrono::milliseconds(5000));
    }
}

void PongGame::collisionThread() {
    while (gameRunning) {
        {
            lock_guard<mutex> lock(gameMutex);
            
            // Mover pelota
            ballX += ballSpeedX;
            ballY += ballSpeedY;
            
            // Rebotes en bordes
            if (ballY <= 0 || ballY >= HEIGHT - 1) {
                ballSpeedY = -ballSpeedY;
            }
            
            // Colisión con paleta izquierda
            if (ballX <= 2) {
                if (ballY >= paddle1Y && ballY < paddle1Y + PADDLE_HEIGHT) {
                    ballSpeedX = 1;
                    ballX = 3;
                } else if (ballX <= 0) {
                    scoreP2++;
                    resetBall();
                }
            }
            
            // Colisión con paleta derecha
            if (ballX >= WIDTH - 3) {
                if (ballY >= paddle2Y && ballY < paddle2Y + PADDLE_HEIGHT) {
                    ballSpeedX = -1;
                    ballX = WIDTH - 4;
                } else if (ballX >= WIDTH - 1) {
                    scoreP1++;
                    resetBall();
                }
            }
            
            // Actualizar IA si es necesario
            if (gameMode == 1 || gameMode == 2) {
                updateAI();
            }
            
            // Condición de victoria
            if (scoreP1 >= 5 || scoreP2 >= 5) {
                gameRunning = false;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(80));
    }
}

void PongGame::inputThread() {
    while (gameRunning) {
        if (kbhit()) {
            int key = getch();
            
            lock_guard<mutex> lock(gameMutex);
            
            if (key == 'q' || key == 'Q') {
                gameRunning = false;
            } 
            else if (key == 'r' || key == 'R') {
                resetBall();
            }
            else if (key == 'w' || key == 'W') {
                if (paddle1Y > 1) paddle1Y--;
            }
            else if (key == 's' || key == 'S') {
                if (paddle1Y < HEIGHT - PADDLE_HEIGHT - 1) paddle1Y++;
            }
            else if (key == 27) {
                if (kbhit()) {
                    int k2 = getch();
                    if (k2 == 91) {
                        if (kbhit()) {
                            int k3 = getch();
                            if (k3 == 65) {
                                if (paddle2Y > 1) paddle2Y--;
                            } else if (k3 == 66) {
                                if (paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) paddle2Y++;
                            }
                        }
                    }
                }
            }
        }
        this_thread::sleep_for(chrono::milliseconds(30));
    }
}

void PongGame::updateAI() {
    if (gameMode == 1) {
        // Jugador vs CPU - solo paleta derecha es CPU
        if (ballY < paddle2Y + PADDLE_HEIGHT / 2 - 1) {
            if (paddle2Y > 1) paddle2Y--;
        } else if (ballY > paddle2Y + PADDLE_HEIGHT / 2 + 1) {
            if (paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) paddle2Y++;
        }
    } else if (gameMode == 2) {
        // CPU vs CPU - ambas paletas
        if (ballY < paddle1Y + PADDLE_HEIGHT / 2 - 1) {
            if (paddle1Y > 1) paddle1Y--;
        } else if (ballY > paddle1Y + PADDLE_HEIGHT / 2 + 1) {
            if (paddle1Y < HEIGHT - PADDLE_HEIGHT - 1) paddle1Y++;
        }
        
        if (ballY < paddle2Y + PADDLE_HEIGHT / 2 - 1) {
            if (paddle2Y > 1) paddle2Y--;
        } else if (ballY > paddle2Y + PADDLE_HEIGHT / 2 + 1) {
            if (paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) paddle2Y++;
        }
    }
}

// ==================== MÉTODOS PRINCIPALES ====================

void PongGame::startGame(int gameMode) {
    this->gameMode = gameMode;
    
    switch (gameMode) {
        case 0: runGameWithPlayers(); break;
        case 1: runPlayerVsCPU(); break;
        case 2: runCPUVsCPU(); break;
        case 3: runDemo(); break;
        default: runGameWithPlayers(); break;
    }
}

void PongGame::runGameWithPlayers() {
    getPlayerNames();
    initializeGame();
    renderer.updatePlayerNames(playerName1, playerName2);

    cout << "🎮 Iniciando: Jugador vs Jugador\n";
    cout << "👆 Controles: W/S (Izquierda) | Flechas (Derecha)\n";
    cout << "❌ Q: Salir | 🔄 R: Reiniciar pelota\n";
    this_thread::sleep_for(chrono::milliseconds(3000));

    startThreads();

    while (gameRunning) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    stopThreads();

    sem_wait(&sem_highscore);
    scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
    sem_post(&sem_highscore);

    mostrarResultadoFinal();
}

void PongGame::runPlayerVsCPU() {
    playerName1 = "Jugador";
    playerName2 = "CPU";
    initializeGame();
    renderer.updatePlayerNames(playerName1, playerName2);

    cout << "🎮 Iniciando: Jugador vs CPU\n";
    cout << "👆 Controles: W/S para mover tu paleta\n";
    this_thread::sleep_for(chrono::milliseconds(3000));

    startThreads();

    while (gameRunning) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    stopThreads();

    sem_wait(&sem_highscore);
    scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
    sem_post(&sem_highscore);

    mostrarResultadoFinal();
}

void PongGame::runCPUVsCPU() {
    playerName1 = "CPU 1";
    playerName2 = "CPU 2";
    initializeGame();
    renderer.updatePlayerNames(playerName1, playerName2);

    cout << "🎮 Iniciando: CPU vs CPU\n";
    cout << "👀 Modo automático - observa el juego\n";
    this_thread::sleep_for(chrono::milliseconds(3000));

    startThreads();

    while (gameRunning) {
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    stopThreads();

    sem_wait(&sem_highscore);
    scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
    sem_post(&sem_highscore);

    mostrarResultadoFinal();
}

void PongGame::runDemo() {
    playerName1 = "Demo 1";
    playerName2 = "Demo 2";
    initializeGame();
    renderer.updatePlayerNames(playerName1, playerName2);

    cout << "🎬 Iniciando Demo Automática\n";
    this_thread::sleep_for(chrono::milliseconds(2000));

    startThreads();

    auto startTime = chrono::steady_clock::now();
    while (gameRunning) {
        auto currentTime = chrono::steady_clock::now();
        auto elapsed = chrono::duration_cast<chrono::seconds>(currentTime - startTime);
        
        if (elapsed.count() >= 30) {
            gameRunning = false;
        }
        
        if (scoreP1 >= 5 || scoreP2 >= 5) {
            gameRunning = false;
        }
        
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    stopThreads();

    sem_wait(&sem_highscore);
    scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
    sem_post(&sem_highscore);

    mostrarResultadoFinal();
}

void PongGame::mostrarResultadoFinal() {
    system("clear");
    cout << "========================================\n";
    cout << "           PARTIDA TERMINADA            \n";
    cout << "========================================\n\n";
    cout << "🏁 Resultado final:\n";
    cout << playerName1 << ": " << scoreP1 << " puntos\n";
    cout << playerName2 << ": " << scoreP2 << " puntos\n\n";
    
    if (scoreP1 > scoreP2) {
        cout << "🎉 ¡" << playerName1 << " GANA! 🎉\n";
    } else if (scoreP2 > scoreP1) {
        cout << "🎉 ¡" << playerName2 << " GANA! 🎉\n";
    } else {
        cout << "🤝 ¡EMPATE! 🤝\n";
    }

    cout << "\nPresiona cualquier tecla para continuar...";
    getch();
}

void PongGame::resetBall() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
    ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
}

void PongGame::getPlayerNames() {
    system("clear");
    cout << "========================================\n";
    cout << "         CONFIGURACIÓN DE JUGADORES     \n";
    cout << "========================================\n\n";
    
    cout << "Ingresa el nombre del Jugador 1 (izquierda - W/S): ";
    if (cin.peek() == '\n') cin.ignore();
    getline(cin, playerName1);
    if (playerName1.empty()) playerName1 = "Jugador 1";
    
    cout << "Ingresa el nombre del Jugador 2 (derecha - Flechas): ";
    getline(cin, playerName2);
    if (playerName2.empty()) playerName2 = "Jugador 2";
    
    cout << "\n¡Perfecto! " << playerName1 << " vs " << playerName2 << "\n";
    cout << "Presiona cualquier tecla para comenzar...";
    getch();
}

void PongGame::showHighScores() {
    scoreManager.displayHighScores();
}

void PongGame::handleInput() {
    // No usado - el input se maneja en inputThread
}