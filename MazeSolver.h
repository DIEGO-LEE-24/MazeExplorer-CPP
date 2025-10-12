#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include "Maze.h"
#include <queue>
#include <map>
#include <vector>

class MazeSolver {
private:
    Maze* maze;

    // Verificar si es posible moverse entre dos celdas
    bool canMove(Cell* from, Cell* to) const;

    // Reconstruir la ruta (retroceso)
    void reconstructPath(Cell* end, std::map<Cell*, Cell*>& parent);

    // Calcular la longitud del camino
    int calculatePathLength(Cell* end, std::map<Cell*, Cell*>& parent) const;

public:
    explicit MazeSolver(Maze* m);

    // Encontrar la ruta más corta usando BFS
    bool solveBFS();

    // Devolver información sobre la ruta
    int getPathLength() const;

    // Verificar si existe una solución
    bool hasSolution() const;
};


#endif