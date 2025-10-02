#ifndef MENU_H
#define MENU_H

enum MenuOption {
    INICIAR_PARTIDA,
    JUGADOR_VS_JUGADOR,
    JUGADOR_VS_CPU,
    CPU_VS_CPU,
    INSTRUCCIONES,
    PUNTAJES,
    SALIR
};

MenuOption mostrarMenu();

#endif