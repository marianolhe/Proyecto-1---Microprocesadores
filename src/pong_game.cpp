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
    // Por implementar
}

void PongGame::handleInput() {
    // Por implementar
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
    
    cout << "\n¡Comenzando partida entre " << playerName1 << " y " << playerName2 << "!\n";
    cout << "Como el juego está en desarrollo, esta es una demostración.\n";
    cout << "Los puntajes se guardarán como 0-0 por ahora.\n";
    cout << "Presiona 'Q' para salir durante el juego.\n\n";
    cout << "Presiona Enter para continuar...";
    
    // Usar getch() en lugar de cin.get() para evitar conflictos
    getch();
    
    // Ejecutar la demo pero con los nombres de los jugadores
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
    
    // Mostrar resultados finales
    system("clear");
    cout << "========================================\n";
    cout << "           PARTIDA TERMINADA            \n";
    cout << "========================================\n\n";
    cout << "Resultado final:\n";
    cout << playerName1 << ": " << scoreP1 << " puntos\n";
    cout << playerName2 << ": " << scoreP2 << " puntos\n\n";
    
    // Como está en desarrollo, guardamos siempre 0-0
    cout << "Puntaje guardado (en desarrollo): " << playerName1 << " 0 - 0 " << playerName2 << "\n\n";
    
    // Guardar el puntaje en el sistema con manejo de errores
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