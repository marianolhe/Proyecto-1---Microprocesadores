#include <iostream>
#include <cstdlib>
#include "menu.h"
#include "utils.h"

using namespace std;

int mostrarMenuConcurrente() {
    int seleccion = 0;
    const int numOpciones = 4;
    string opciones[numOpciones] = {
        "Jugador vs Jugador (PvP) con Hilos",
        "Jugador vs Computadora (PvC) con Hilos", 
        "Computadora vs Computadora (CvC) con Hilos",
        "Volver al Menú Principal"
    };

    while (true) {
        system("clear");
        cout << "========================================\n";
        cout << "       PONG - MODOS CONCURRENTES       \n";
        cout << "========================================\n\n";
        cout << "Selecciona el modo de juego con hilos POSIX:\n\n";

        for (int i = 0; i < numOpciones; i++) {
            if (i == seleccion) cout << "   > " << opciones[i] << "\n";
            else cout << "     " << opciones[i] << "\n";
        }

        cout << "\n========================================\n";
        cout << " Características técnicas:\n";
        cout << " • 6 hilos POSIX simultáneos\n";
        cout << " • Sincronización con mutex y semáforos\n"; 
        cout << " • Renderizado en tiempo real\n";
        cout << " • Detección de colisiones concurrente\n";
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
            return seleccion;
        }
    }
}