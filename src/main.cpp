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
#include <unistd.h>
using namespace std;

int main() {
    PongGame game;
    bool salir = false;

    while (!salir) {
        MenuOption opcion = mostrarMenu();

        switch (opcion) {
            case INICIAR_PARTIDA:
                cout << ">> Iniciando partida ...\n";
                sleep(1);
                game.runDemo();
                break;

            case JUGADOR_VS_JUGADOR:
                cout << ">> Iniciando partida Jugador vs Jugador...\n";
                sleep(1);
                game.startGame(1); // Modo JvJ
                break;
                
            case JUGADOR_VS_CPU:
                cout << ">> Iniciando partida Jugador vs CPU...\n";
                sleep(1);
                game.startGame(2); // Modo JvsCPU
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

    cout << "Gracias por jugar Pong ASCII!\n";
    return 0;
}