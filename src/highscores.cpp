/****************************************************
 * Archivo: highscores.cpp
 * Descripción: Implementa la gestión de puntajes altos del juego Pong.
 *              Permite guardar, cargar y mostrar los mejores puntajes
 *              con los nombres de los jugadores.
 * - Marian Olivares
 * - Marcela Ordoñez
 * - Biancka Raxón
 * - Diana Sosa 
 * 
 * Fecha: Septiembre de 2025
 ****************************************************/

#include "highscores.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

HighScoreManager::HighScoreManager() {
    filename = "pong_highscores.txt";
    sem_init(&file_semaphore, 0, 1);
    loadScores();
}

HighScoreManager::~HighScoreManager() {
    sem_destroy(&file_semaphore);
}

void HighScoreManager::safeAddScore(const std::string& p1Name, const std::string& p2Name, int p1Score, int p2Score) {
    sem_wait(&file_semaphore);
    addScore(p1Name, p2Name, p1Score, p2Score);
    sem_post(&file_semaphore);
}

void HighScoreManager::addScore(const string& p1Name, const string& p2Name, int p1Score, int p2Score) {
    try {
        HighScore newScore;
        newScore.player1Name = p1Name;
        newScore.player2Name = p2Name;
        newScore.player1Score = p1Score;
        newScore.player2Score = p2Score;
        
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        if (timeinfo != nullptr) {
            char buffer[11];
            strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
            newScore.date = string(buffer);
        } else {
            newScore.date = "01/01/2024";
        }
        
        highScores.push_back(newScore);
        
        if (highScores.size() > MAX_SCORES) {
            highScores.erase(highScores.begin());
        }
        
        saveScores();
    } catch (...) {
        cout << "Error al guardar puntaje, pero el juego continúa.\n";
    }
}

void HighScoreManager::loadScores() {
    sem_wait(&file_semaphore);
    
    ifstream file(filename);
    if (!file.is_open()) {
        sem_post(&file_semaphore);
        return;
    }
    
    highScores.clear();
    string line;
    
    while (getline(file, line) && highScores.size() < MAX_SCORES) {
        stringstream ss(line);
        string p1Name, p2Name, date;
        int p1Score, p2Score;
        
        if (getline(ss, p1Name, '|') &&
            getline(ss, p2Name, '|') &&
            ss >> p1Score &&
            ss.ignore(1, '|') &&
            ss >> p2Score &&
            ss.ignore(1, '|') &&
            getline(ss, date)) {
            
            HighScore score;
            score.player1Name = p1Name;
            score.player2Name = p2Name;
            score.player1Score = p1Score;
            score.player2Score = p2Score;
            score.date = date;
            highScores.push_back(score);
        }
    }
    
    file.close();
    sem_post(&file_semaphore);
}

void HighScoreManager::saveScores() {
    sem_wait(&file_semaphore);
    
    try {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "No se pudo guardar el archivo de puntajes.\n";
            sem_post(&file_semaphore);
            return;
        }
        
        for (const auto& score : highScores) {
            file << score.player1Name << "|"
                 << score.player2Name << "|"
                 << score.player1Score << "|"
                 << score.player2Score << "|"
                 << score.date << "\n";
        }
        
        file.close();
    } catch (...) {
        cout << "Error al guardar archivo de puntajes.\n";
    }
    
    sem_post(&file_semaphore);
}

void HighScoreManager::displayHighScores() {
    sem_wait(&file_semaphore);
    
    system("clear");
    cout << "========================================\n";
    cout << "           PUNTAJES DESTACADOS          \n";
    cout << "========================================\n\n";
    
    if (highScores.empty()) {
        cout << "No hay puntajes registrados aún.\n";
        cout << "¡Juega una partida para aparecer aquí!\n\n";
    } else {
        cout << "Últimas partidas registradas:\n\n";
        cout << left << setw(15) << "Jugador 1" 
             << setw(15) << "Jugador 2" 
             << setw(10) << "Resultado" 
             << setw(12) << "Fecha" << "\n";
        cout << "--------------------------------------------------------\n";
        
        for (size_t i = 0; i < highScores.size(); i++) {
            const auto& score = highScores[i];
            cout << left << setw(15) << score.player1Name
                 << setw(15) << score.player2Name
                 << setw(5) << score.player1Score << "-" << setw(4) << score.player2Score
                 << setw(12) << score.date << "\n";
        }
    }
    
    cout << "\n========================================\n";
    cout << "Presiona Enter para volver al menú\n";
    cout << "========================================\n";
    cin.ignore();
    cin.get();
    
    sem_post(&file_semaphore);
}

vector<HighScore> HighScoreManager::getHighScores() const {
    return highScores;
}