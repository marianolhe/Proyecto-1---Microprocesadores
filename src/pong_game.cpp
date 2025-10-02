/****************************************************
 * Archivo: pong_game.cpp
 * Descripción: Controla la lógica principal del juego Pong, incluyendo la inicialización,
 *              simulación de movimiento, detección de colisiones, actualización de puntajes
 *              y renderizado en modo demostración.
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa
 *
 * Fecha: Septiembre de 2025
 ****************************************************/

#include "pong_game.h"
#include <unistd.h>
#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iostream>
#include <string>
#include <limits>
#include <functional>

using namespace std;

// ===================== LÓGICA DE COLISIONES Y ANOTACIONES =====================
void PongGame::checkCollisions() {
    // Colisión con paleta izquierda
    pthread_mutex_lock(&mutex_paddleA);
    int p1Y = paddle1Y;
    pthread_mutex_unlock(&mutex_paddleA);
    if (ballX <= 3) {
        if (ballY >= p1Y && ballY <= p1Y + PADDLE_HEIGHT) {
            ballSpeedX = 1;
        }
    }
    // Colisión con paleta derecha
    pthread_mutex_lock(&mutex_paddleB);
    int p2Y = paddle2Y;
    pthread_mutex_unlock(&mutex_paddleB);
    if (ballX >= WIDTH - 4) {
        if (ballY >= p2Y && ballY <= p2Y + PADDLE_HEIGHT) {
            ballSpeedX = -1;
        }
    }
    // Rebote superior/inferior
    if (ballY <= 1 || ballY >= HEIGHT - 2) {
        ballSpeedY = -ballSpeedY;
    }
}

void PongGame::checkScoring() {
    // Anotación jugador 2
    if (ballX < 1) {
        scoreP2++;
        resetBall();
        roundInProgress = false;
        // Notificar al serve_thread que hay un reinicio pendiente
        pthread_cond_signal(&cond_start_round);
    }
    // Anotación jugador 1
    else if (ballX > WIDTH - 2) {
        scoreP1++;
        resetBall();
        roundInProgress = false;
        pthread_cond_signal(&cond_start_round);
    }
}

// Funciones wrapper para pthread_create (ya declaradas en el header)
void* PongGame::inputThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);
    game->inputThread();
    return nullptr;
}

// ===================== HILO DE LA PELOTA =====================
void* PongGame::ballThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);

    while (game->gameRunning) {
        pthread_mutex_lock(&game->mutex_game_state);

        // Mover pelota
        game->ballX += game->ballSpeedX;
        game->ballY += game->ballSpeedY;

        // Rebotes con bordes superior e inferior
        if (game->ballY <= 1 || game->ballY >= HEIGHT - 2) {
            game->ballSpeedY = -game->ballSpeedY;
        }

        // Colisiones con paletas
        if (game->ballX <= 3) {
            if (game->ballY >= game->paddle1Y && game->ballY <= game->paddle1Y + PADDLE_HEIGHT) {
                game->ballSpeedX = 1;
            } else {
                game->scoreP2++;
                game->resetBall();
            }
        }
        if (game->ballX >= WIDTH - 4) {
            if (game->ballY >= game->paddle2Y && game->ballY <= game->paddle2Y + PADDLE_HEIGHT) {
                game->ballSpeedX = -1;
            } else {
                game->scoreP1++;
                game->resetBall();
            }
        }

        pthread_mutex_unlock(&game->mutex_game_state);

        pthread_cond_signal(&game->cond_frame_ready); // Notificar frame listo
        usleep(100 * 1000); // ~60 FPS
    }
    return nullptr;
}

// ===================== HILO CPU PLAYER A =====================
void* PongGame::cpuPlayerAThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);

    while (game->gameRunning) {
        pthread_mutex_lock(&game->mutex_game_state);

        if (game->ballSpeedX < 0) { // Pelota va hacia la izquierda
            int targetY = game->ballY - PADDLE_HEIGHT / 2;
            if (game->paddle1Y < targetY) game->paddle1Y++;
            else if (game->paddle1Y > targetY) game->paddle1Y--;
        }

        pthread_mutex_unlock(&game->mutex_game_state);
        usleep(16 * 1000);
    }
    return nullptr;
}

// ===================== HILO CPU PLAYER B =====================
void* PongGame::cpuPlayerBThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);

    while (game->gameRunning) {
        pthread_mutex_lock(&game->mutex_game_state);

        if (game->ballSpeedX > 0) { // Pelota va hacia la derecha
            int targetY = game->ballY - PADDLE_HEIGHT / 2;
            if (game->paddle2Y < targetY) game->paddle2Y++;
            else if (game->paddle2Y > targetY) game->paddle2Y--;
        }

        pthread_mutex_unlock(&game->mutex_game_state);
        usleep(16 * 1000);
    }
    return nullptr;
}


void* PongGame::playerThreadWrapper(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    // Llamamos al método genérico definido en el header
    data->game->playerThread(data->player_id);
    return nullptr;
}

void* PongGame::aiThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);
    game->aiThread();
    return nullptr;
}

void* PongGame::serveThreadWrapper(void* arg) {
    PongGame* game = static_cast<PongGame*>(arg);
    game->serveThread();
    return nullptr;
}

PongGame::~PongGame() {
    // Destruir mutex
    pthread_mutex_destroy(&mtxQueueP1);
    pthread_mutex_destroy(&mtxQueueP2);
    pthread_mutex_destroy(&mutex_paddleA);
    pthread_mutex_destroy(&mutex_paddleB);
    pthread_mutex_destroy(&mutex_start_round);
    pthread_mutex_destroy(&mutex_game_state);

    // Destruir variables de condición
    pthread_cond_destroy(&cvP1);
    pthread_cond_destroy(&cvP2);
    pthread_cond_destroy(&cond_start_round);
    pthread_cond_destroy(&cond_frame_ready);
}

PongGame::PongGame() {
    srand(time(0));

    // Inicializar nombres por defecto
    playerName1 = "Jugador 1";
    playerName2 = "Jugador 2";

    // Inicializar mutex
    pthread_mutex_init(&mtxQueueP1, nullptr);
    pthread_mutex_init(&mtxQueueP2, nullptr);
    pthread_mutex_init(&mutex_paddleA, nullptr);
    pthread_mutex_init(&mutex_paddleB, nullptr);
    pthread_mutex_init(&mutex_start_round, nullptr);
    pthread_mutex_init(&mutex_game_state, nullptr);
    pthread_cond_init(&cond_frame_ready, nullptr);


    // Inicializar variables de condición
    pthread_cond_init(&cvP1, nullptr);
    pthread_cond_init(&cvP2, nullptr);
    pthread_cond_init(&cond_start_round, nullptr);

    // Inicializar datos de los hilos
    playerAData = {this, 1};
    playerBData = {this, 2};
    aiData = {this, 2};

    initializeGame();
}

void PongGame::initializeGame() {
    scoreP1 = 0;
    scoreP2 = 0;
    paddle1Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    paddle2Y = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
    ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
    gameRunning = true;
    resetRequested = false;
    // NO sobrescribir los nombres aquí - se mantienen los que el usuario ingresó
}

void PongGame::runDemo() {
    initializeGame();

    for (int i = 0; i < 100 && gameRunning; i++) {
        ballX += ballSpeedX;
        ballY += ballSpeedY;

        if (ballY <= 1 || ballY >= HEIGHT - 2) {
            ballSpeedY = -ballSpeedY;
        }

        if (ballX <= 3) {
            if (ballY >= paddle1Y && ballY <= paddle1Y + PADDLE_HEIGHT) {
                ballSpeedX = 1;
            } else {
                scoreP2++;
                resetBall();
            }
        }

        if (ballX >= WIDTH - 4) {
            if (ballY >= paddle2Y && ballY <= paddle2Y + PADDLE_HEIGHT) {
                ballSpeedX = -1;
            } else {
                scoreP1++;
                resetBall();
            }
        }

        if (ballY < paddle1Y + PADDLE_HEIGHT / 2 && paddle1Y > 1) {
            paddle1Y--;
        } else if (ballY > paddle1Y + PADDLE_HEIGHT / 2 && paddle1Y < HEIGHT - PADDLE_HEIGHT - 1) {
            paddle1Y++;
        }

        if (ballY < paddle2Y + PADDLE_HEIGHT / 2 && paddle2Y > 1) {
            paddle2Y--;
        } else if (ballY > paddle2Y + PADDLE_HEIGHT / 2 && paddle2Y < HEIGHT - PADDLE_HEIGHT - 1) {
            paddle2Y++;
        }

        renderer.updateScores(scoreP1, scoreP2);
        renderer.updatePaddles(paddle1Y, paddle2Y);
        renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        renderer.renderGame();

        usleep(100 * 1000);

        if (kbhit()) {
            int key = getch();
            if (key == 'q' || key == 'Q') {
                gameRunning = false;
            }
        }
    }

    cout << "Demo finalizada. Presiona cualquier tecla para continuar...";
    getch();
}

void PongGame::inputThread() { this->inputListenerThread(); }
void PongGame::playerThread(int player_id) {
    if (player_id == 1) this->playerAThread();
    else this->playerBThread();
}
void PongGame::aiThread() { this->ai_opponent_thread(); }
void PongGame::serveThread() { this->serve_manager_thread(); }

void PongGame::resetBall() {
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    ballSpeedX = (rand() % 2 == 0) ? 1 : -1;
    ballSpeedY = (rand() % 2 == 0) ? 1 : -1;
}

void PongGame::startGame(int gameMode) {
    initializeGame();
    getPlayerNames();

    if (gameMode == 1) { // JvJ
        isAIEnabled = false;
        roundInProgress = true;
        pthread_create(&input_thread, nullptr, &PongGame::inputThreadWrapper, this);
        pthread_create(&player1_thread, nullptr, &PongGame::playerThreadWrapper, &playerAData);
        pthread_create(&player2_thread, nullptr, &PongGame::playerThreadWrapper, &playerBData);
    } else if (gameMode == 2) { // JvsCPU
        isAIEnabled = true;
        ai_difficulty = 0.8f;
        roundInProgress = true;
        // Un único lector de teclado: inputListenerThread crea eventos en la cola
        pthread_create(&input_thread, nullptr, [](void* arg) -> void* { static_cast<PongGame*>(arg)->inputListenerThread(); return nullptr; }, this);
        // player_keyboard_adapter_thread ahora consume la cola (no lee teclado directamente)
        pthread_create(&player1_thread, nullptr, [](void* arg) -> void* { static_cast<PongGame*>(arg)->player_keyboard_adapter_thread(); return nullptr; }, this);
        pthread_create(&player2_thread, nullptr, &PongGame::aiThreadWrapper, this);
        pthread_create(&serve_thread, nullptr, &PongGame::serveThreadWrapper, this);
    } else if (gameMode == 3) { // CPU vs CPU
        isAIEnabled = true;
        gameRunning = true;

        // Crear hilos
        pthread_create(&ball_thread, nullptr, &PongGame::ballThreadWrapper, this);
        pthread_create(&cpuA_thread, nullptr, &PongGame::cpuPlayerAThreadWrapper, this);
        pthread_create(&cpuB_thread, nullptr, &PongGame::cpuPlayerBThreadWrapper, this);

        // Bucle de renderizado sincronizado con cond_frame_ready
        while (gameRunning) {
            pthread_mutex_lock(&mutex_game_state);
            // Esperar hasta que el hilo de la pelota genere un nuevo frame
            pthread_cond_wait(&cond_frame_ready, &mutex_game_state);

            // Actualizar renderer con el estado actual
            renderer.updateScores(scoreP1, scoreP2);
            renderer.updatePaddles(paddle1Y, paddle2Y);
            renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
            pthread_mutex_unlock(&mutex_game_state);

            // Renderizar
            renderer.renderGame();
        }

        // Esperar a que terminen los hilos
        pthread_join(ball_thread, nullptr);
        pthread_join(cpuA_thread, nullptr);
        pthread_join(cpuB_thread, nullptr);
    }


    // Bucle principal del juego
    while (gameRunning) {
        // Siempre renderiza, pero solo actualiza física si la ronda está activa
        if (roundInProgress) {
            updatePhysics();
            checkCollisions();
            checkScoring();
        }
        renderer.updateScores(scoreP1, scoreP2);
        renderer.updatePaddles(paddle1Y, paddle2Y);
        renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        renderer.renderGame();
        usleep(16 * 1000); // ~60 FPS
    }

    // Esperar a que terminen todos los hilos
    pthread_join(input_thread, nullptr);
    pthread_join(player1_thread, nullptr);
    pthread_join(player2_thread, nullptr);
    if (isAIEnabled) {
        pthread_join(serve_thread, nullptr);
    }

    // Limpiar estado para volver al menú correctamente
    isAIEnabled = false;
    roundInProgress = false;
    gameRunning = true;
}

void PongGame::updatePhysics() {
    // Actualiza posición de la pelota
    ballX += ballSpeedX;
    ballY += ballSpeedY;

    // Rebotes verticales
    if (ballY <= 1 || ballY >= HEIGHT - 2) {
        ballSpeedY = -ballSpeedY;
    }

    // Leer snapshots de paletas bajo lock breve (pthread)
    int p1Y, p2Y;
    pthread_mutex_lock(&mutex_paddleA);
    p1Y = paddle1Y;
    pthread_mutex_unlock(&mutex_paddleA);

    pthread_mutex_lock(&mutex_paddleB);
    p2Y = paddle2Y;
    pthread_mutex_unlock(&mutex_paddleB);

    // Colisiones con paletas
    if (ballX <= 3) {
        if (ballY >= p1Y && ballY <= p1Y + PADDLE_HEIGHT) {
            ballSpeedX = 1;
        }
    }
    if (ballX >= WIDTH - 4) {
        if (ballY >= p2Y && ballY <= p2Y + PADDLE_HEIGHT) {
            ballSpeedX = -1;
        }
    }
}

void PongGame::handleInput() {
    // No usado directamente: la entrada se maneja por hilos
}

void PongGame::getPlayerNames() {
    system("clear");
    cout << "========================================\n";
    cout << "         CONFIGURACIÓN DE JUGADORES     \n";
    cout << "========================================\n\n";

    cout << "Ingresa el nombre del Jugador 1 (izquierda): ";
    // Limpiar buffer solo si hay algo pendiente
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, playerName1);

    if (playerName1.empty()) {
        playerName1 = "Jugador 1";
    }

    cout << "Ingresa el nombre del Jugador 2 (derecha): ";
    getline(cin, playerName2);

    if (playerName2.empty()) {
        playerName2 = "Jugador 2";
    }

    cout << "\n¡Perfecto! " << playerName1 << " vs " << playerName2 << "\n";
    cout << "Presiona cualquier tecla para comenzar el juego...\n";
    getch();
}

void PongGame::runGameWithPlayers() {
    getPlayerNames();
    initializeGame();

    // Actualizar los nombres en el renderer
    renderer.updatePlayerNames(playerName1, playerName2);

    // Lanzar hilos
    gameRunning = true;
    pthread_create(&input_thread, nullptr, &PongGame::inputThreadWrapper, this);
    pthread_create(&player1_thread, nullptr, &PongGame::playerThreadWrapper, &playerAData);
    pthread_create(&player2_thread, nullptr, &PongGame::playerThreadWrapper, &playerBData);

    // Bucle principal de juego (física + render)
    while (gameRunning) {
        if (resetRequested) {
            resetBall();
            resetRequested = false;
        }

        updatePhysics();

        // Anotaciones (cuando la pelota pasa el límite)
        if (ballX < 1) {
            scoreP2++;
            resetBall();
        } else if (ballX > WIDTH - 2) {
            scoreP1++;
            resetBall();
        }

        // Pintar
        {
            int p1, p2;
            pthread_mutex_lock(&mutex_paddleA);
            p1 = paddle1Y;
            pthread_mutex_unlock(&mutex_paddleA);
            pthread_mutex_lock(&mutex_paddleB);
            p2 = paddle2Y;
            pthread_mutex_unlock(&mutex_paddleB);
            renderer.updateScores(scoreP1, scoreP2);
            renderer.updatePaddles(p1, p2);
            renderer.updateBall(ballX, ballY, ballSpeedX, ballSpeedY);
        }
        renderer.renderGame();

        usleep(100 * 1000);
    }

    // Cerrar hilos limpiamente
    pthread_cond_broadcast(&cvP1);
    pthread_cond_broadcast(&cvP2);
    pthread_join(input_thread, nullptr);
    pthread_join(player1_thread, nullptr);
    pthread_join(player2_thread, nullptr);

    // Mostrar resultados finales
    system("clear");
    cout << "========================================\n";
    cout << "           PARTIDA TERMINADA            \n";
    cout << "========================================\n\n";
    cout << "Resultado final:\n";
    cout << playerName1 << ": " << scoreP1 << " puntos\n";
    cout << playerName2 << ": " << scoreP2 << " puntos\n\n";

    // Guardar el puntaje (nota: el manager actual siempre guarda 0-0 durante desarrollo)
    try {
        scoreManager.addScore(playerName1, playerName2, scoreP1, scoreP2);
        cout << "Puntaje guardado exitosamente!\n";
    } catch (...) {
        cout << "No se pudo guardar el puntaje, pero el juego funcionó correctamente.\n";
    }

    cout << "\nPresiona cualquier tecla para continuar...";
    getch();
}

void PongGame::showHighScores() {
    scoreManager.displayHighScores();
}

// ===================== HILOS (JvJ) =====================

void PongGame::inputListenerThread() {
    while (gameRunning) {
        if (kbhit()) {
            int key = getch();
            if (key == 'q' || key == 'Q') {
                gameRunning = false;
                // despertar a los hilos bloqueados
                pthread_cond_broadcast(&cvP1);
                pthread_cond_broadcast(&cvP2);
                // Despertar al serve_thread si está esperando
                pthread_cond_broadcast(&cond_start_round);
                break;
            } else if (key == 'r' || key == 'R') {
                // solicitar reinicio y notificar al serve thread
                resetRequested = true;
                pthread_cond_signal(&cond_start_round);
            } else if (key == 'w' || key == 'W') {
                pthread_mutex_lock(&mtxQueueP1);
                queueP1.push(EventType::P1_UP);
                pthread_mutex_unlock(&mtxQueueP1);
                pthread_cond_signal(&cvP1);
            } else if (key == 's' || key == 'S') {
                pthread_mutex_lock(&mtxQueueP1);
                queueP1.push(EventType::P1_DOWN);
                pthread_mutex_unlock(&mtxQueueP1);
                pthread_cond_signal(&cvP1);
            } else if (key == 27) { // ESC: posible secuencia de flechas
                if (kbhit()) {
                    int k2 = getch();
                    if (k2 == 91 && kbhit()) { // '['
                        int k3 = getch();
                        if (k3 == 65) { // Up
                            pthread_mutex_lock(&mtxQueueP2);
                            queueP2.push(EventType::P2_UP);
                            pthread_mutex_unlock(&mtxQueueP2);
                            pthread_cond_signal(&cvP2);
                        } else if (k3 == 66) { // Down
                            pthread_mutex_lock(&mtxQueueP2);
                            queueP2.push(EventType::P2_DOWN);
                            pthread_mutex_unlock(&mtxQueueP2);
                            pthread_cond_signal(&cvP2);
                        }
                    }
                }
            }
        }
        usleep(5 * 1000);
    }
}

void PongGame::playerAThread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (gameRunning) {
        // Esperar hasta que haya eventos o termine el juego
        pthread_mutex_lock(&mtxQueueP1);
        while (queueP1.empty() && gameRunning) {
            pthread_cond_wait(&cvP1, &mtxQueueP1);
        }
        if (!gameRunning) {
            pthread_mutex_unlock(&mtxQueueP1);
            break;
        }
        while (!queueP1.empty()) {
            EventType ev = queueP1.front(); queueP1.pop();
            // desbloquear cola mientras procesamos para evitar retener la cola
            pthread_mutex_unlock(&mtxQueueP1);
            pthread_mutex_lock(&mutex_paddleA);
            if (ev == EventType::P1_UP) {
                paddle1Y = inBounds(paddle1Y - 1);
            } else if (ev == EventType::P1_DOWN) {
                paddle1Y = inBounds(paddle1Y + 1);
            }
            pthread_mutex_unlock(&mutex_paddleA);
            // volver a bloquear cola para posibles siguientes eventos
            pthread_mutex_lock(&mtxQueueP1);
        }
        pthread_mutex_unlock(&mtxQueueP1);
    }
}

void PongGame::playerBThread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (gameRunning) {
        pthread_mutex_lock(&mtxQueueP2);
        while (queueP2.empty() && gameRunning) {
            pthread_cond_wait(&cvP2, &mtxQueueP2);
        }
        if (!gameRunning) {
            pthread_mutex_unlock(&mtxQueueP2);
            break;
        }
        while (!queueP2.empty()) {
            EventType ev = queueP2.front(); queueP2.pop();
            pthread_mutex_unlock(&mtxQueueP2);
            pthread_mutex_lock(&mutex_paddleB);
            if (ev == EventType::P2_UP) {
                paddle2Y = inBounds(paddle2Y - 1);
            } else if (ev == EventType::P2_DOWN) {
                paddle2Y = inBounds(paddle2Y + 1);
            }
            pthread_mutex_unlock(&mutex_paddleB);
            pthread_mutex_lock(&mtxQueueP2);
        }
        pthread_mutex_unlock(&mtxQueueP2);
    }
}

// Adaptador de teclado para jugador humano en modo JvsCPU
// NOW: no lee teclado directamente — consume la misma cola que playerAThread
void PongGame::player_keyboard_adapter_thread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (gameRunning) {
        // Esperar por eventos de la cola como en playerAThread
        pthread_mutex_lock(&mtxQueueP1);
        while (queueP1.empty() && gameRunning) {
            pthread_cond_wait(&cvP1, &mtxQueueP1);
        }
        if (!gameRunning) {
            pthread_mutex_unlock(&mtxQueueP1);
            break;
        }
        while (!queueP1.empty()) {
            EventType ev = queueP1.front(); queueP1.pop();
            pthread_mutex_unlock(&mtxQueueP1);

            pthread_mutex_lock(&mutex_paddleA);
            if (ev == EventType::P1_UP) {
                paddle1Y = inBounds(paddle1Y - 1);
            } else if (ev == EventType::P1_DOWN) {
                paddle1Y = inBounds(paddle1Y + 1);
            }
            pthread_mutex_unlock(&mutex_paddleA);

            pthread_mutex_lock(&mtxQueueP1);
        }
        pthread_mutex_unlock(&mtxQueueP1);

        usleep(16 * 1000); // ~60 FPS
    }
}

// Gestión de inicio/reinicio de rondas
void PongGame::serve_manager_thread() {
    while (gameRunning) {
        pthread_mutex_lock(&mutex_start_round);
        while (!resetRequested && gameRunning) {
            pthread_cond_wait(&cond_start_round, &mutex_start_round);
        }
        if (!gameRunning) {
            pthread_mutex_unlock(&mutex_start_round);
            break;
        }
        // Reiniciar posición de la pelota y estado de la ronda
        resetBall();
        roundInProgress = true;
        resetRequested = false;
        pthread_mutex_unlock(&mutex_start_round);
        // Dar tiempo a los jugadores para prepararse
        sleep(2);

        // Notificar que la ronda ha comenzado (opcional: renderer puede leer roundInProgress)
        renderer.renderScoreBoard();
    }
}

// Implementación de la IA para el modo JvsCPU
void PongGame::ai_opponent_thread() {
    auto inBounds = [](int y) {
        if (y < 1) return 1;
        if (y > HEIGHT - PADDLE_HEIGHT - 1) return HEIGHT - PADDLE_HEIGHT - 1;
        return y;
    };

    while (true) {
        if (!gameRunning) break;
        if (isAIEnabled && roundInProgress) {
            // Simple protección de acceso a la paleta
            pthread_mutex_lock(&mutex_paddleB);
            // Asegurarse de no dividir por cero
            if (ballSpeedX != 0) {
                // Solo predecir si la pelota va hacia la derecha (hacia la paleta B)
                if (ballSpeedX > 0) {
                    // Estimación simple del tiempo que tarda en llegar
                    float distance = static_cast<float>((WIDTH - 2) - ballX);
                    float timeToReach = distance / static_cast<float>(ballSpeedX);
                    // Predicción simplificada de Y con reflejos en bordes
                    float predictedY = ballY + (ballSpeedY * timeToReach);
                    // Reflejar en límites hasta que quede dentro del rango
                    while (predictedY < 0 || predictedY > HEIGHT) {
                        if (predictedY < 0) predictedY = -predictedY;
                        else if (predictedY > HEIGHT) predictedY = 2 * HEIGHT - predictedY;
                    }
                    int targetY = static_cast<int>(predictedY - PADDLE_HEIGHT / 2);
                    targetY = inBounds(targetY);
                    float errorMargin = PADDLE_HEIGHT * (1.0f - ai_difficulty);
                    int centerB = paddle2Y + PADDLE_HEIGHT / 2;
                    if (centerB < targetY - static_cast<int>(errorMargin)) {
                        paddle2Y = inBounds(paddle2Y + 1);
                    } else if (centerB > targetY + static_cast<int>(errorMargin)) {
                        paddle2Y = inBounds(paddle2Y - 1);
                    }
                }
            }
            pthread_mutex_unlock(&mutex_paddleB);
        }
        usleep(16 * 1000);
    }
}
