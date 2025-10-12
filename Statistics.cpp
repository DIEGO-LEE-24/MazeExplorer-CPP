#include "Statistics.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>

Statistics::Statistics(const std::string& file) : filename(file) {
    loadFromFile();
}

bool Statistics::fileExists() const {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void Statistics::addGame(const GameStats& stats) {
    
    if (stats.moves <= 0 || stats.timeSeconds < 0 || stats.optimalPathLength <= 0) {
        std::cerr << "Warning: Invalid game stats, skipping..." << std::endl;
        return;
    }
    history.push_back(stats);
}

void Statistics::saveToFile() {
    if (history.empty()) return;

    std::ofstream file(filename, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    const GameStats& s = history.back();
    file << s.mazeRows << "," << s.mazeCols << ","
        << s.moves << "," << std::fixed << std::setprecision(2)
        << s.timeSeconds << "," << s.optimalPathLength << "\n";
    file.close();
}

void Statistics::loadFromFile() {
    if (!fileExists()) {
        std::cout << "Stats file not found, starting fresh." << std::endl;
        return;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open stats file: " << filename << std::endl;
        return;
    }

    history.clear();
    GameStats s;
    char comma;

    while (file >> s.mazeRows >> comma >> s.mazeCols >> comma
        >> s.moves >> comma >> s.timeSeconds >> comma
        >> s.optimalPathLength) {
        // 데이터 유효성 검증
        if (s.moves > 0 && s.timeSeconds >= 0 && s.optimalPathLength > 0) {
            history.push_back(s);
        }
    }
    file.close();

    std::cout << "Loaded " << history.size() << " game records." << std::endl;
}

void Statistics::clearHistory() {
    history.clear();
    // 파일도 삭제
    std::ofstream file(filename, std::ios::trunc);
    file.close();
}

double Statistics::getAverageTime() const {
    if (history.empty()) return 0.0;

    double sum = 0.0;
    for (const auto& s : history) {
        sum += s.timeSeconds;
    }
    return sum / history.size();
}

double Statistics::getAverageEfficiency() const {
    if (history.empty()) return 0.0;

    double sum = 0.0;
    for (const auto& s : history) {
        sum += s.getEfficiency();
    }
    return (sum / history.size()) * 100.0;
}

int Statistics::getBestTime() const {
    if (history.empty()) return 0;

    int best = static_cast<int>(history[0].timeSeconds);
    for (const auto& s : history) {
        int time = static_cast<int>(s.timeSeconds);
        if (time < best) {
            best = time;
        }
    }
    return best;
}

int Statistics::getWorstTime() const {
    if (history.empty()) return 0;

    int worst = static_cast<int>(history[0].timeSeconds);
    for (const auto& s : history) {
        int time = static_cast<int>(s.timeSeconds);
        if (time > worst) {
            worst = time;
        }
    }
    return worst;
}

double Statistics::getAverageMoves() const {
    if (history.empty()) return 0.0;

    double sum = 0.0;
    for (const auto& s : history) {
        sum += s.moves;
    }
    return sum / history.size();
}

int Statistics::getTotalGamesForDifficulty(int rows, int cols) const {
    int count = 0;
    for (const auto& s : history) {
        if (s.mazeRows == rows && s.mazeCols == cols) {
            count++;
        }
    }
    return count;
}

double Statistics::getAverageTimeForDifficulty(int rows, int cols) const {
    double sum = 0.0;
    int count = 0;

    for (const auto& s : history) {
        if (s.mazeRows == rows && s.mazeCols == cols) {
            sum += s.timeSeconds;
            count++;
        }
    }

    return count > 0 ? sum / count : 0.0;
}

void Statistics::printSummary() const {
    std::cout << "\n=== ESTADISTICAS GLOBALES ===" << std::endl;

    if (history.empty()) {
        std::cout << "No hay datos de partidas." << std::endl;
        std::cout << "============================\n" << std::endl;
        return;
    }

    std::cout << "Partidas jugadas: " << getTotalGames() << std::endl;
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "Tiempo promedio: " << getAverageTime() << " seg" << std::endl;
    std::cout << "Mejor tiempo: " << getBestTime() << " seg" << std::endl;
    std::cout << "Peor tiempo: " << getWorstTime() << " seg" << std::endl;
    std::cout << "Movimientos promedio: " << getAverageMoves() << std::endl;
    std::cout << "Eficiencia promedio: " << getAverageEfficiency() << "%" << std::endl;

    std::cout << "\n--- Por Dificultad ---" << std::endl;
    std::cout << "FACIL (15x25): " << getTotalGamesForDifficulty(15, 25)
        << " partidas, " << getAverageTimeForDifficulty(15, 25) << " seg promedio" << std::endl;
    std::cout << "NORMAL (22x40): " << getTotalGamesForDifficulty(22, 40)
        << " partidas, " << getAverageTimeForDifficulty(22, 40) << " seg promedio" << std::endl;
    std::cout << "DIFICIL (30x55): " << getTotalGamesForDifficulty(30, 55)
        << " partidas, " << getAverageTimeForDifficulty(30, 55) << " seg promedio" << std::endl;

    std::cout << "============================\n" << std::endl;
}