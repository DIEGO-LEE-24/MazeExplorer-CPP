#include "MazeSolver.h"
#include <iostream>

MazeSolver::MazeSolver(Maze* m) : maze(m) {
    if (maze == nullptr) {
        std::cerr << "Error: MazeSolver initialized with null maze!" << std::endl;
    }
}

bool MazeSolver::canMove(Cell* from, Cell* to) const {
    if (from == nullptr || to == nullptr) {
        return false;
    }

    int dr = to->row - from->row;
    int dc = to->col - from->col;

    
    if ((dr != 0 && dc != 0) || (dr == 0 && dc == 0)) {
        return false;
    }

    
    if (dr < -1 || dr > 1 || dc < -1 || dc > 1) {
        return false;
    }

    
    if (dr == -1) return !from->topWall;      
    if (dr == 1) return !from->bottomWall;    
    if (dc == -1) return !from->leftWall;     
    if (dc == 1) return !from->rightWall;     

    return false;
}

int MazeSolver::calculatePathLength(Cell* end, std::map<Cell*, Cell*>& parent) const {
    int length = 0;
    Cell* current = end;

    while (current != nullptr) {
        length++;
        current = parent[current];
    }

    return length;
}

void MazeSolver::reconstructPath(Cell* end, std::map<Cell*, Cell*>& parent) {
    if (end == nullptr) {
        std::cerr << "Error: Cannot reconstruct path - end cell is null" << std::endl;
        return;
    }

    Cell* current = end;
    int pathLength = 0;

    while (current != nullptr) {
        current->inSolution = true;
        current = parent[current];
        pathLength++;
    }

    std::cout << "Solution path found: " << pathLength << " steps" << std::endl;
}

bool MazeSolver::solveBFS() {
    if (maze == nullptr) {
        std::cerr << "Error: Cannot solve - maze is null!" << std::endl;
        return false;
    }

    // 초기화
    maze->clearVisited();
    maze->clearSolution();

    std::queue<Cell*> q;
    std::map<Cell*, Cell*> parent;

    // 시작점과 끝점 찾기
    Cell* start = nullptr;
    Cell* end = nullptr;

    for (int i = 0; i < maze->getRows(); i++) {
        for (int j = 0; j < maze->getCols(); j++) {
            if (maze->getCell(i, j).isStart) {
                start = &maze->getCell(i, j);
            }
            if (maze->getCell(i, j).isEnd) {
                end = &maze->getCell(i, j);
            }
        }
    }

    // 유효성 검증
    if (start == nullptr || end == nullptr) {
        std::cerr << "Error: Start or end point not found in maze!" << std::endl;
        return false;
    }

    // BFS 시작
    start->visited = true;
    q.push(start);
    parent[start] = nullptr;

    int nodesExplored = 0;

    while (!q.empty()) {
        Cell* current = q.front();
        q.pop();
        nodesExplored++;

        // 목표 지점 도달
        if (current == end) {
            reconstructPath(end, parent);
            std::cout << "BFS explored " << nodesExplored << " nodes" << std::endl;
            return true;
        }

        int row = current->row;
        int col = current->col;

        
        Cell* neighbors[4] = {
            maze->isValidCell(row - 1, col) ? &maze->getCell(row - 1, col) : nullptr, 
            maze->isValidCell(row + 1, col) ? &maze->getCell(row + 1, col) : nullptr,  
            maze->isValidCell(row, col - 1) ? &maze->getCell(row, col - 1) : nullptr,  
            maze->isValidCell(row, col + 1) ? &maze->getCell(row, col + 1) : nullptr   
        };

        for (int i = 0; i < 4; i++) {
            Cell* next = neighbors[i];
            if (next && !next->visited && canMove(current, next)) {
                next->visited = true;
                parent[next] = current;
                q.push(next);
            }
        }
    }

    
    std::cerr << "Warning: No path found from start to end!" << std::endl;
    return false;
}

int MazeSolver::getPathLength() const {
    if (maze == nullptr) return 0;

    int length = 0;
    for (int i = 0; i < maze->getRows(); i++) {
        for (int j = 0; j < maze->getCols(); j++) {
            if (maze->getCell(i, j).inSolution) {
                length++;
            }
        }
    }
    return length;
}

bool MazeSolver::hasSolution() const {
    return getPathLength() > 0;
}