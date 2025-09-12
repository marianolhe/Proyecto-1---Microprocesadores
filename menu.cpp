#include <iostream>
#include <cstdlib>
#include "menu.h"
#include "utils.h"

using namespace std;

MenuOption mostrarMenu() {
    int seleccion = 0;
    const int numOpciones = 4;
    string opciones[numOpciones] = {
        "Iniciar partida",
        "Instrucciones",
        "Puntajes destacados",
        "Salir del juego"
    };

    while (true) {
        system("clear");
        cout << "========================================\n";
        cout << "         BIENVENIDO A PONG ASCII        \n";
        cout << "========================================\n\n";

        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion) cout << "   > " << opciones[i] << "\n";
            else cout << "     " << opciones[i] << "\n";
        }

        cout << "\n========================================\n";
        cout << " Usa ↑ y ↓ para moverte, Enter para seleccionar\n";
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