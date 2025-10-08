/****************************************************
 * Archivo: main.cpp
 * Descripción: Punto de entrada del juego Pong en consola. Muestra el menú principal,
 *              permite al jugador iniciar la demo, leer instrucciones o salir.
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include <iostream>
#include "menu.h"
#include "instrucciones.h"
#include "pong_game.h"
#include "utils.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::this_thread; 
using namespace std::chrono;      

int main() {
    PongGame game;
    bool salir = false;

    while (!salir) {
        MenuOption opcion = mostrarMenu();

        switch (opcion) {
            case INICIAR_PARTIDA:
                {
                    int modo = mostrarModoJuego();
                    game.startGame(modo);
                }
                break;

            case INSTRUCCIONES:
                mostrarInstrucciones();
                break;

            case PUNTAJES:
                game.showHighScores();
                break;

            case SALIR:
                salir = true;
                break;
        }
    }

    cout << "🎮 Gracias por jugar Pong ASCII! 🎮\n";
    return 0;
}