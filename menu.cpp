#include <iostream>
#include <cstdlib>   // system("clear")
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
        system("clear"); // limpia pantalla en Linux
        cout << "========================================\n";
        cout << "         BIENVENIDO A PONG ASCII        \n";
        cout << "========================================\n\n";

        // Mostrar opciones con indicador ">"
        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion) cout << "   > " << opciones[i] << "\n";
            else cout << "     " << opciones[i] << "\n";
        }

        cout << "\n========================================\n";
        cout << " Usa ↑ y ↓ para moverte, Enter para seleccionar\n";
        cout << "========================================\n";

        int tecla = getch(); // Captura la tecla presionada

        // En Linux: flechas generan secuencia de 3 caracteres:
        // 27 (ESC), 91 ([), y 65/66 (arriba/abajo)
        if (tecla == 27 && kbhit()) {
            int t2 = getch();
            if (t2 == 91) {
                int t3 = getch();
                if (t3 == 65) { // flecha arriba
                    seleccion = (seleccion - 1 + numOpciones) % numOpciones;
                } else if (t3 == 66) { // flecha abajo
                    seleccion = (seleccion + 1) % numOpciones;
                }
            }
        } else if (tecla == 10) { // Enter en Linux
            return static_cast<MenuOption>(seleccion);
        }
    }
}
