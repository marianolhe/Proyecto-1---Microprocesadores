/****************************************************
 * Archivo: pong_render.cpp
 * Descripción: Encargado de pintar el juego en pantalla. Aquí se dibuja todo: cancha,
 *              jugadores, pelota y marcador. También se actualiza la posición de cada
 *              elemento según el estado del juego.
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include "pong_render.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

PongRenderer::PongRenderer() {
    scoreP1 = 0;
    scoreP2 = 0;
    paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballDirX = 1;
    ballDirY = 1;
    playerName1 = "JUGADOR 1";
    playerName2 = "JUGADOR 2";
}

void PongRenderer::updateScores(int p1, int p2) {
    lock_guard<mutex> lock(renderMutex);
    scoreP1 = p1;
    scoreP2 = p2;
}

void PongRenderer::updatePaddles(int p1Y, int p2Y) {
    lock_guard<mutex> lock(renderMutex);
    paddle1Y = p1Y;
    paddle2Y = p2Y;
}

void PongRenderer::updateBall(int x, int y, int dirX, int dirY) {
    lock_guard<mutex> lock(renderMutex);
    ballX = x;
    ballY = y;
    ballDirX = dirX;
    ballDirY = dirY;
}

void PongRenderer::updatePlayerNames(const string& name1, const string& name2) {
    lock_guard<mutex> lock(renderMutex);
    playerName1 = name1;
    playerName2 = name2;
}

void PongRenderer::clearScreen() {
    system("clear");
}

void PongRenderer::renderScoreBoard() {
    string name1 = playerName1.length() > 10 ? playerName1.substr(0, 10) : playerName1;
    string name2 = playerName2.length() > 10 ? playerName2.substr(0, 10) : playerName2;
    
    cout << "==================================================\n";
    cout << "  " << left << setw(12) << (name1 + ":") << setw(3) << scoreP1;
    cout << "     " << left << setw(12) << (name2 + ":") << setw(3) << scoreP2;
    cout << "\n";
    cout << "==================================================\n";
}

void PongRenderer::renderCourt() {
    vector<string> screen(HEIGHT, string(WIDTH, ' '));
    
    // Bordes y línea central
    for (int y = 0; y < HEIGHT; y++) {
        screen[y][0] = '#';
        screen[y][WIDTH - 1] = '#';
        screen[y][WIDTH / 2] = '|';
    }
    
    // Paleta izquierda
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        if (paddle1Y + i < HEIGHT) {
            screen[paddle1Y + i][2] = '|';
        }
    }
    
    // Paleta derecha
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        if (paddle2Y + i < HEIGHT) {
            screen[paddle2Y + i][WIDTH - 3] = '|';
        }
    }
    
    // Pelota con dirección
    if (ballX >= 0 && ballX < WIDTH && ballY >= 0 && ballY < HEIGHT) {
        screen[ballY][ballX] = (ballDirX > 0) ? '>' : '<';
    }
    
    // Imprimir pantalla
    for (int y = 0; y < HEIGHT; y++) {
        cout << screen[y] << "\n";
    }
}

void PongRenderer::renderGame() {
    lock_guard<mutex> lock(renderMutex);
    clearScreen();
    renderScoreBoard();
    renderCourt();
    
    cout << "Controles: W/S (P1) ↑/↓ (P2) | Q: Salir | R: Reiniciar\n";
    cout << "==================================================\n";
    cout.flush();
}