#ifndef MENU_H
#define MENU_H

enum MenuOption {
    INICIAR_PARTIDA,
    INSTRUCCIONES,
    PUNTAJES,
    SALIR
};

enum GameMode {
    JUGADOR_VS_JUGADOR,
    JUGADOR_VS_CPU,
    CPU_VS_CPU,
    DEMO
};

MenuOption mostrarMenu();
GameMode mostrarModoJuego();

#endif