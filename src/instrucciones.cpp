/****************************************************
 * Archivo: instrucciones.cpp
 * Descripción: Muestra en pantalla las instrucciones del juego, incluyendo objetivo,
 *              controles, elementos visuales y ejemplo de tablero.
 * Integrantes:
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include <iostream>
#include <cstdlib>
#include "instrucciones.h"
#include "utils.h"

using namespace std;

void mostrarInstrucciones() {
    system("clear"); 
    cout << "========================================\n";
    cout << "              INSTRUCCIONES             \n";
    cout << "========================================\n\n";

    cout << "Objetivo:\n";
    cout << "Mantén la pelota en juego rebotándola con tu raqueta.\n";
    cout << "Si la pelota pasa tu lado, el rival gana un punto.\n\n";

    cout << "Controles:\n";
    cout << "Jugador 1 (izquierda): W = subir, S = bajar\n";
    cout << "Jugador 2 (derecha):  ↑ = subir, ↓ = bajar\n";
    cout << "Comandos generales:   Q = salir, R = reiniciar\n\n";

    cout << "Elementos visuales:\n";
    cout << "   O  -> pelota\n";
    cout << "   |  -> raqueta\n";
    cout << "   --- marcador en la parte superior\n\n";

    cout << "Ejemplo de tablero:\n";
    cout << "   Jugador 1: 0        Jugador 2: 0\n";
    cout << "   |                           |\n";
    cout << "   |            O              |\n";
    cout << "   |                           |\n";
    cout << "========================================\n";
    cout << "     Presiona cualquier tecla para volver\n";
    cout << "========================================\n";

    getch(); 
}
