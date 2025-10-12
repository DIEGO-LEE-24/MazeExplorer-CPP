#include "Maze.h"
#include <iostream>
#include <stdexcept>

Maze::Maze(int r, int c) : rows(r), cols(c) {
    // Validación
    if (r <= 0 || c <= 0) {
        throw std::invalid_argument("Maze dimensions must be positive");
    }

    if (r > 1000 || c > 1000) {
        throw std::invalid_argument("Maze dimensions too large");
    }

    // Inicialización de la cuadrícula
    grid.resize(rows);
    for (int i = 0; i < rows; i++) {
        grid[i].resize(cols);
        for (int j = 0; j < cols; j++) {
            grid[i][j].row = i;
            grid[i][j].col = j;
        }
    }

    // Marcar el punto de inicio y el punto final
    grid[0][0].isStart = true;
    grid[rows - 1][cols - 1].isEnd = true;
}

Cell& Maze::getCell(int row, int col) {
    if (!isValidCell(row, col)) {
        throw std::out_of_range("Cell coordinates out of range");
    }
    return grid[row][col];
}

const Cell& Maze::getCell(int row, int col) const {
    if (!isValidCell(row, col)) {
        throw std::out_of_range("Cell coordinates out of range");
    }
    return grid[row][col];
}

void Maze::reset() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Cell& cell = grid[i][j];
            cell.topWall = true;
            cell.rightWall = true;
            cell.bottomWall = true;
            cell.leftWall = true;
            cell.visited = false;
            cell.inSolution = false;
            // Mantener isStart e isEnd

        }
    }
}

void Maze::clearVisited() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].visited = false;
        }
    }
}

void Maze::clearSolution() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            grid[i][j].inSolution = false;
        }
    }
}

bool Maze::isValidCell(int row, int col) const {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

void Maze::removeWall(Cell& current, Cell& next) {
    int dr = next.row - current.row;
    int dc = next.col - current.col;

    // 인접하지 않은 셀인지 확인
    if ((dr != 0 && dc != 0) || (dr == 0 && dc == 0)) {
        std::cerr << "Warning: Attempting to remove wall between non-adjacent cells"
            << std::endl;
        return;
    }

    
    if (dr < -1 || dr > 1 || dc < -1 || dc > 1) {
        std::cerr << "Warning: Invalid cell distance for wall removal" << std::endl;
        return;
    }

    
    if (dr == -1) {        
        current.topWall = false;
        next.bottomWall = false;
    }
    else if (dr == 1) {    
        current.bottomWall = false;
        next.topWall = false;
    }
    else if (dc == -1) {   
        current.leftWall = false;
        next.rightWall = false;
    }
    else if (dc == 1) {    
        current.rightWall = false;
        next.leftWall = false;
    }
}

bool Maze::hasWallBetween(const Cell& cell1, const Cell& cell2) const {
    int dr = cell2.row - cell1.row;
    int dc = cell2.col - cell1.col;

    // Siempre devuelve true si no son adyacentes (se considera que hay una pared)
    if ((dr != 0 && dc != 0) || (dr == 0 && dc == 0)) {
        return true;
    }

    if (dr == -1) return cell1.topWall;
    if (dr == 1) return cell1.bottomWall;
    if (dc == -1) return cell1.leftWall;
    if (dc == 1) return cell1.rightWall;

    return true;
}

std::vector<Cell*> Maze::getUnvisitedNeighbors(Cell* cell) {
    std::vector<Cell*> neighbors;

    if (cell == nullptr) return neighbors;

    int row = cell->row;
    int col = cell->col;

    // Comprobar en el orden: arriba, abajo, izquierda y derecha
    if (isValidCell(row - 1, col) && !grid[row - 1][col].visited) {
        neighbors.push_back(&grid[row - 1][col]);
    }
    if (isValidCell(row + 1, col) && !grid[row + 1][col].visited) {
        neighbors.push_back(&grid[row + 1][col]);
    }
    if (isValidCell(row, col - 1) && !grid[row][col - 1].visited) {
        neighbors.push_back(&grid[row][col - 1]);
    }
    if (isValidCell(row, col + 1) && !grid[row][col + 1].visited) {
        neighbors.push_back(&grid[row][col + 1]);
    }

    return neighbors;
}

std::vector<Cell*> Maze::getNeighbors(Cell* cell) {
    std::vector<Cell*> neighbors;

    if (cell == nullptr) return neighbors;

    int row = cell->row;
    int col = cell->col;

    if (isValidCell(row - 1, col)) {
        neighbors.push_back(&grid[row - 1][col]);
    }
    if (isValidCell(row + 1, col)) {
        neighbors.push_back(&grid[row + 1][col]);
    }
    if (isValidCell(row, col - 1)) {
        neighbors.push_back(&grid[row][col - 1]);
    }
    if (isValidCell(row, col + 1)) {
        neighbors.push_back(&grid[row][col + 1]);
    }

    return neighbors;
}

std::vector<Cell*> Maze::getAccessibleNeighbors(Cell* cell) {
    std::vector<Cell*> neighbors;

    if (cell == nullptr) return neighbors;

    int row = cell->row;
    int col = cell->col;

	// arriba
    if (isValidCell(row - 1, col) && !cell->topWall) {
        neighbors.push_back(&grid[row - 1][col]);
    }
    // abajo
    if (isValidCell(row + 1, col) && !cell->bottomWall) {
        neighbors.push_back(&grid[row + 1][col]);
    }
    // izq
    if (isValidCell(row, col - 1) && !cell->leftWall) {
        neighbors.push_back(&grid[row][col - 1]);
    }
    // derecha
    if (isValidCell(row, col + 1) && !cell->rightWall) {
        neighbors.push_back(&grid[row][col + 1]);
    }

    return neighbors;
}

int Maze::countWalls() const {
    int wallCount = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            const Cell& cell = grid[i][j];
            if (cell.topWall) wallCount++;
            if (cell.rightWall) wallCount++;
            if (cell.bottomWall) wallCount++;
            if (cell.leftWall) wallCount++;
        }
    }
    return wallCount / 2;  // Cada pared se cuenta dos veces, por lo tanto se divide entre 2
}

void Maze::printDebugInfo() const {
    std::cout << "\n=== Maze Debug Info ===" << std::endl;
    std::cout << "Dimensions: " << rows << "x" << cols << std::endl;
    std::cout << "Total cells: " << getTotalCells() << std::endl;
    std::cout << "Total walls: " << countWalls() << std::endl;
    std::cout << "Start: (" << 0 << ", " << 0 << ")" << std::endl;
    std::cout << "End: (" << rows - 1 << ", " << cols - 1 << ")" << std::endl;
    std::cout << "======================\n" << std::endl;
}