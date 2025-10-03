#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <string>
#include <vector>
#include <semaphore.h>

struct HighScore {
    std::string player1Name;
    std::string player2Name;
    int player1Score;
    int player2Score;
    std::string date;
};

class HighScoreManager {
private:
    std::vector<HighScore> highScores;
    static const int MAX_SCORES = 10;
    std::string filename;
    sem_t file_semaphore;
    
public:
    HighScoreManager();
    ~HighScoreManager();
    void addScore(const std::string& p1Name, const std::string& p2Name, int p1Score, int p2Score);
    void loadScores();
    void saveScores();
    void displayHighScores();
    std::vector<HighScore> getHighScores() const;
    void safeAddScore(const std::string& p1Name, const std::string& p2Name, int p1Score, int p2Score);
};

#endif