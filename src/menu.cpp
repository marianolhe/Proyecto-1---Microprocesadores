/****************************************************
 * Archivo: menu.cpp
 * Descripción: Implementa el menú principal del juego Pong en consola. Permite al usuario
 *              navegar entre opciones como iniciar partida, ver instrucciones, consultar
 *              puntajes destacados o salir del juego. Utiliza entrada de teclado para
 *              controlar la selección con flechas y confirmar con Enter.
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include <iostream>
#include "menu.h"
#include "utils.h"

using namespace std;

MenuOption mostrarMenu() {
    int seleccion = 0;
    const int numOpciones = 4;
    string opciones[numOpciones] = {
        "🎮 Iniciar partida",
        "📖 Instrucciones", 
        "🏆 Puntajes destacados",
        "🚪 Salir del juego"
    };

    while (true) {
        system("clear");
        cout << "========================================\n";
        cout << "            BIENVENIDO A PONG           \n";
        cout << "========================================\n\n";

        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion) 
                cout << "   ➤ " << opciones[i] << "\n";
            else 
                cout << "     " << opciones[i] << "\n";
        }

        cout << "\n========================================\n";
        cout << " Usa ↑/↓ para navegar, Enter para seleccionar\n";
        cout << "========================================\n";

        int tecla = getch();

        if (tecla == 27 && kbhit()) {
            int t2 = getch();
            if (t2 == 91) {
                int t3 = getch();
                if (t3 == 65) {
                    seleccion = (seleccion - 1 + numOpciones) % numOpciones;
                } else if (t3 == 66) {
                    seleccion = (seleccion + 1) % numOpciones;
                }
            }
        } else if (tecla == 10) {
            return static_cast<MenuOption>(seleccion);
        }
    }
}

GameMode mostrarModoJuego() {
    int seleccion = 0;
    const int numOpciones = 4;
    string opciones[numOpciones] = {
        "👤 Jugador vs Jugador",
        "🤖 Jugador vs CPU", 
        "⚡ CPU vs CPU",
        "🎬 Demo Automática"
    };

    while (true) {
        system("clear");
        cout << "========================================\n";
        cout << "          SELECCIONA MODO DE JUEGO      \n";
        cout << "========================================\n\n";

        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion) 
                cout << "   ➤ " << opciones[i] << "\n";
            else 
                cout << "     " << opciones[i] << "\n";
        }

        cout << "\n========================================\n";
        cout << " Usa ↑/↓ para navegar, Enter para seleccionar\n";
        cout << "========================================\n";

        int tecla = getch();

        if (tecla == 27 && kbhit()) {
            int t2 = getch();
            if (t2 == 91) {
                int t3 = getch();
                if (t3 == 65) {
                    seleccion = (seleccion - 1 + numOpciones) % numOpciones;
                } else if (t3 == 66) {
                    seleccion = (seleccion + 1) % numOpciones;
                }
            }
        } else if (tecla == 10) {
            return static_cast<GameMode>(seleccion);
        }
    }
}