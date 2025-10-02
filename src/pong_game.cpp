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
#include <limits>
#include <functional>

using namespace std;

PongGame::PongGame() {
    srand(time(0));
    // Inicializar nombres por defecto
    playerName1 = "Jugador 1";
    playerName2 = "Jugador 2";
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
    resetRequested = false;
    // NO sobrescribir los nombres aquí - se mantienen los que el usuario ingresó
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
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
        
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
    // Actualiza posición de la pelota
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Rebotes verticales
    if (ballY <= 1 || ballY >= HEIGHT - 2) {
        ballSpeedY = -ballSpeedY;
    }

    // Leer snapshots de paletas bajo lock breve
    int p1Y, p2Y;
    {
        std::lock_guard<std::mutex> lkA(mutex_paddleA);
        p1Y = paddle1Y;
    }
    {
        std::lock_guard<std::mutex> lkB(mutex_paddleB);
        p2Y = paddle2Y;
    }

    // Colisiones con paletas
    if (ballX <= 3) {
        if (ballY >= p1Y && ballY <= p1Y + PADDLE_HEIGHT) {
            ballSpeedX = 1;
        }
    }
    if (ballX >= WIDTH - 4) {
        if (ballY >= p2Y && ballY <= p2Y + PADDLE_HEIGHT) {
            ballSpeedX = -1;
        }
    }
}

void PongGame::handleInput() {
    // No usado directamente: la entrada se maneja por hilos
}

void PongGame::getPlayerNames() {
    system("clear");
    cout << "========================================\n";
    cout << "         CONFIGURACIÓN DE JUGADORES     \n";
    cout << "========================================\n\n";
    
    cout << "Ingresa el nombre del Jugador 1 (izquierda): ";
    // Limpiar buffer solo si hay algo pendiente
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, playerName1);
    
    if (playerName1.empty()) {
        playerName1 = "Jugador 1";
    }
    
    cout << "Ingresa el nombre del Jugador 2 (derecha): ";
    getline(cin, playerName2);
    
    if (playerName2.empty()) {
        playerName2 = "Jugador 2";
    }
    
    cout << "\n¡Perfecto! " << playerName1 << " vs " << playerName2 << "\n";
    cout << "Presiona cualquier tecla para comenzar el juego...\n";
    getch();
}

void PongGame::runGameWithPlayers() {
    getPlayerNames();
    initializeGame();

    // Actualizar los nombres en el renderer
    renderer.updatePlayerNames(playerName1, playerName2);

    // Lanzar hilos
    gameRunning = true;
    tInput = std::thread(&PongGame::inputListenerThread, this);
    tPlayerA = std::thread(&PongGame::playerAThread, this);
    tPlayerB = std::thread(&PongGame::playerBThread, this);

    // Bucle principal de juego (física + render)
    while (gameRunning) {
        if (resetRequested) {
            resetBall();
            resetRequested = false;
        }

        updatePhysics();

        // Anotaciones (cuando la pelota pasa el límite)
        if (ballX < 1) {
            scoreP2++;
            resetBall();
        } else if (ballX > WIDTH - 2) {
            scoreP1++;
            resetBall();
        }

        // Pintar
        {
            int p1, p2;
            {
                std::lock_guard<std::mutex> lkA(mutex_paddleA);
                p1 = paddle1Y;
            }
            {
                std::lock_guard<std::mutex> lkB(mutex_paddleB);
                p2 = paddle2Y;
            }
            renderer.updateScores(scoreP1, scoreP2);
            renderer.updatePaddles(p1, p2);
            renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        }
        renderer.renderGame();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Cerrar hilos limpiamente
    cvP1.notify_all();
    cvP2.notify_all();
    if (tInput.joinable()) tInput.join();
    if (tPlayerA.joinable()) tPlayerA.join();
    if (tPlayerB.joinable()) tPlayerB.join();

    // Mostrar resultados finales
    system("clear");
    cout << "========================================\n";
    cout << "           PARTIDA TERMINADA            \n";
    cout << "========================================\n\n";
    cout << "Resultado final:\n";
    cout << playerName1 << ": " << scoreP1 << " puntos\n";
    cout << playerName2 << ": " << scoreP2 << " puntos\n\n";

    // Guardar el puntaje (nota: el manager actual siempre guarda 0-0 durante desarrollo)
    try {
        scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
        cout << "Puntaje guardado exitosamente!\n";
    } catch (...) {
        cout << "No se pudo guardar el puntaje, pero el juego funcionó correctamente.\n";
    }

    cout << "\nPresiona cualquier tecla para continuar...";
    getch();
}

void PongGame::showHighScores() {
    scoreManager.displayHighScores();
}

// ===================== HILOS (JvJ) =====================

void PongGame::inputListenerThread() {
    while (gameRunning) {
        if (kbhit()) {
            int key = getch();
            if (key == 'q' || key == 'Q') {
                gameRunning = false;
                cvP1.notify_all();
                cvP2.notify_all();
                break;
            } else if (key == 'r' || key == 'R') {
                resetRequested = true;
            } else if (key == 'w' || key == 'W') {
                {
                    std::lock_guard<std::mutex> lk(mtxQueueP1);
                    queueP1.push(EventType::P1_UP);
                }
                cvP1.notify_one();
            } else if (key == 's' || key == 'S') {
                {
                    std::lock_guard<std::mutex> lk(mtxQueueP1);
                    queueP1.push(EventType::P1_DOWN);
                }
                cvP1.notify_one();
            } else if (key == 27) { // ESC: posible secuencia de flechas
                if (kbhit()) {
                    int k2 = getch();
                    if (k2 == 91 && kbhit()) { // '['
                        int k3 = getch();
                        if (k3 == 65) { // Up
                            {
                                std::lock_guard<std::mutex> lk(mtxQueueP2);
                                queueP2.push(EventType::P2_UP);
                            }
                            cvP2.notify_one();
                        } else if (k3 == 66) { // Down
                            {
                                std::lock_guard<std::mutex> lk(mtxQueueP2);
                                queueP2.push(EventType::P2_DOWN);
                            }
                            cvP2.notify_one();
                        }
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void PongGame::playerAThread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (gameRunning) {
        std::unique_lock<std::mutex> ul(mtxQueueP1);
        cvP1.wait(ul, [this]{ return !queueP1.empty() || !gameRunning; });
        if (!gameRunning) break;
        while (!queueP1.empty()) {
            EventType ev = queueP1.front(); queueP1.pop();
            ul.unlock();
            if (ev == EventType::P1_UP) {
                std::lock_guard<std::mutex> lka(mutex_paddleA);
                paddle1Y = inBounds(paddle1Y - 1);
            } else if (ev == EventType::P1_DOWN) {
                std::lock_guard<std::mutex> lka(mutex_paddleA);
                paddle1Y = inBounds(paddle1Y + 1);
            }
            ul.lock();
        }
    }
}

void PongGame::playerBThread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (gameRunning) {
        std::unique_lock<std::mutex> ul(mtxQueueP2);
        cvP2.wait(ul, [this]{ return !queueP2.empty() || !gameRunning; });
        if (!gameRunning) break;
        while (!queueP2.empty()) {
            EventType ev = queueP2.front(); queueP2.pop();
            ul.unlock();
            if (ev == EventType::P2_UP) {
                std::lock_guard<std::mutex> lkb(mutex_paddleB);
                paddle2Y = inBounds(paddle2Y - 1);
            } else if (ev == EventType::P2_DOWN) {
                std::lock_guard<std::mutex> lkb(mutex_paddleB);
                paddle2Y = inBounds(paddle2Y + 1);
            }
            ul.lock();
        }
    }
}