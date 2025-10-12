#ifndef STATISTICS_H
#define STATISTICS_H

#include <string>
#include <vector>

struct GameStats {
    int mazeRows;
    int mazeCols;
    int moves;
    double timeSeconds;
    int optimalPathLength;

    GameStats() : mazeRows(0), mazeCols(0), moves(0),
        timeSeconds(0.0), optimalPathLength(0) {
    }

    // Calcular la eficiencia (0.0 ~ 1.0)
    double getEfficiency() const {
        if (moves == 0) return 0.0;
        return static_cast<double>(optimalPathLength) / moves;
    }
};

class Statistics {
private:
    std::vector<GameStats> history;
    std::string filename;

    // Verificar si el archivo existe
    bool fileExists() const;

public:
    explicit Statistics(const std::string& file);

    void addGame(const GameStats& stats);
    void saveToFile();
    void loadFromFile();
    void clearHistory(); // Agregar función para reiniciar las estadísticas

    // Getters
    int getTotalGames() const { return static_cast<int>(history.size()); }
    double getAverageTime() const;
    double getAverageEfficiency() const;
    int getBestTime() const;
    int getWorstTime() const; // Agregar tiempo más largo (peor)
    double getAverageMoves() const; // Agregar número promedio de movimientos

    // Estadísticas por nivel de dificultad
    int getTotalGamesForDifficulty(int rows, int cols) const;
    double getAverageTimeForDifficulty(int rows, int cols) const;

    void printSummary() const;

    // Verificar si está vacío
    bool isEmpty() const { return history.empty(); }
};

#endif