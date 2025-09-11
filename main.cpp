#include <iostream>
#include "menu.h"
#include "instrucciones.h"

using namespace std;

int main() {
    bool salir = false;

    while (!salir) {
        MenuOption opcion = mostrarMenu();

        switch (opcion) {
            case INICIAR_PARTIDA:
                cout << ">> Aquí más adelante iniciaremos el juego...\n";
                cout << "Presiona Enter para volver al menú\n";
                cin.ignore();
                cin.get();
                break;

            case INSTRUCCIONES:
                mostrarInstrucciones();
                break;

            case PUNTAJES:
                cout << ">> Pantalla de puntajes en construcción...\n";
                cout << "Presiona Enter para volver al menú\n";
                cin.ignore();
                cin.get();
                break;

            case SALIR:
                salir = true;
                break;
        }
    }

    cout << "Gracias por jugar Pong ASCII!\n";
    return 0;
}
