#ifndef MAZE_H
#define MAZE_H

#include <vector>

struct Cell {
    int row, col;

    // Paredes 
    bool topWall;
    bool rightWall;
    bool bottomWall;
    bool leftWall;

    // Estados 
    bool visited;      // Usado en los algoritmos de generación o exploración
    bool inSolution;   // Indica si forma parte de la ruta óptima
    bool isStart;      // Indica si es el punto de inicio
    bool isEnd;        // Indica si es el punto final


    Cell() : row(0), col(0),
        topWall(true), rightWall(true),
        bottomWall(true), leftWall(true),
        visited(false), inSolution(false),
        isStart(false), isEnd(false) {
    }

    // Verificar si todas las paredes están presentes (celda aislada)
    bool isFullyWalled() const {
        return topWall && rightWall && bottomWall && leftWall;
    }

    //  Verificar si hay una pared en una dirección específica
    bool hasWallInDirection(int dr, int dc) const {
        if (dr == -1) return topWall;
        if (dr == 1) return bottomWall;
        if (dc == -1) return leftWall;
        if (dc == 1) return rightWall;
        return true; // Dirección no válida
    }
};

class Maze {
private:
    int rows;
    int cols;
    std::vector<std::vector<Cell>> grid;

public:
    Maze(int r, int c);

    // Getters
    int getRows() const { return rows; }
    int getCols() const { return cols; }

    // Acceso a celdas (versiones const y no const)

    Cell& getCell(int row, int col);
    const Cell& getCell(int row, int col) const;

    // Inicialización y limpieza del laberinto
    void reset();               // Restaurar todas las paredes y reiniciar el estado
    void clearVisited();        // Reiniciar solo la bandera 'visited'
    void clearSolution();       // Reiniciar solo la bandera 'inSolution'


    // Validación
    bool isValidCell(int row, int col) const;

    // Manipulación de paredes
    void removeWall(Cell& current, Cell& next);
    bool hasWallBetween(const Cell& cell1, const Cell& cell2) const;

    // Búsqueda de celdas vecinas
    std::vector<Cell*> getUnvisitedNeighbors(Cell* cell);
    std::vector<Cell*> getNeighbors(Cell* cell);              // Sin importar si fueron visitadas
    std::vector<Cell*> getAccessibleNeighbors(Cell* cell);    // Solo vecinas sin paredes

    // Depuración y estadísticas
    int getTotalCells() const { return rows * cols; }
    int countWalls() const;
    void printDebugInfo() const;

};

#endif