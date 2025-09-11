#ifndef MENU_H
#define MENU_H

// Opciones disponibles en el menú
enum MenuOption {
    INICIAR_PARTIDA,
    INSTRUCCIONES,
    PUNTAJES,
    SALIR
};

// Muestra el menú y retorna la opción seleccionada
MenuOption mostrarMenu();

#endif
