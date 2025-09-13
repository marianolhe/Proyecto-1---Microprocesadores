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
    scoreP1 = p1;
    scoreP2 = p2;
}

void PongRenderer::updatePaddles(int p1Y, int p2Y) {
    paddle1Y = p1Y;
    paddle2Y = p2Y;
}

void PongRenderer::updateBall(int x, int y, int dirX, int dirY) {
    ballX = x;
    ballY = y;
    ballDirX = dirX;
    ballDirY = dirY;
}

void PongRenderer::updatePlayerNames(const string& name1, const string& name2) {
    playerName1 = name1;
    playerName2 = name2;
}

void PongRenderer::clearScreen() {
    system("clear");
}

void PongRenderer::renderScoreBoard() {
    cout << "==================================================\n";
    
    // Truncar nombres si son muy largos para mantener formato
    string name1 = playerName1.length() > 10 ? playerName1.substr(0, 10) : playerName1;
    string name2 = playerName2.length() > 10 ? playerName2.substr(0, 10) : playerName2;
    
    // Formatear con espaciado dinámico para que siempre se vea bien
    cout << "  " << left << setw(12) << (name1 + ":") << setw(3) << scoreP1;
    cout << "     " << left << setw(12) << (name2 + ":") << setw(3) << scoreP2;
    cout << "\n";
    cout << "==================================================\n";
}

void PongRenderer::renderCourt() {
    for (int y = 0; y < HEIGHT; y++) {
        cout << "#";
        
        for (int x = 1; x < WIDTH - 1; x++) {
            if (x == WIDTH / 2) {
                cout << "|"; 
            } else {
                cout << " ";
            }
        }
        
        cout << "#\n";
    }
}

void PongRenderer::renderPaddles() {
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        gotoxy(2, paddle1Y + i + 3); 
        cout << "|";
    }
    
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        gotoxy(WIDTH - 3, paddle2Y + i + 3); 
        cout << "|";
    }
}

void PongRenderer::renderBall() {
    gotoxy(ballX, ballY + 3); 
    
    if (ballDirX > 0 && ballDirY > 0) cout << "↘";
    else if (ballDirX > 0 && ballDirY < 0) cout << "↗";
    else if (ballDirX < 0 && ballDirY > 0) cout << "↙";
    else if (ballDirX < 0 && ballDirY < 0) cout << "↖";
    else cout << "O";
}

void PongRenderer::renderGame() {
    clearScreen();
    renderScoreBoard();
    renderCourt();
    renderPaddles();
    renderBall();
    
    gotoxy(0, HEIGHT + 6);
    cout << "Controles: W/S (P1) ↑/↓ (P2) | Q: Salir | R: Reiniciar\n";
    cout << "==================================================\n";
}