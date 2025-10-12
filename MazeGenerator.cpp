#include "MazeGenerator.h"
#include <iostream>

UnionFind::UnionFind(int size) {
    parent.resize(size);
    rank.resize(size, 0);
    for (int i = 0; i < size; i++) {
        parent[i] = i;
    }
}

int UnionFind::find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);  // Path compression
    }
    return parent[x];
}

bool UnionFind::unite(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);

    if (rootX == rootY) {
        return false;  
    }

    // Union by rank
    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    }
    else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    }
    else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }

    return true;
}
MazeGenerator::MazeGenerator(Maze* m) : maze(m) {
    if (maze == nullptr) {
        std::cerr << "Error: MazeGenerator initialized with null maze!" << std::endl;
        return;
    }

    std::random_device rd;
    rng.seed(rd());
}

void MazeGenerator::setSeed(unsigned int seed) {
    rng.seed(seed);
}

void MazeGenerator::generateWithSeed(unsigned int seed, MazeAlgorithm algorithm) {
    setSeed(seed);
    generate(algorithm);
}

void MazeGenerator::generateRecursiveBacktracker() {
    maze->reset();

    std::stack<Cell*> stack;
    Cell* current = &maze->getCell(0, 0);
    current->visited = true;
    stack.push(current);

    int totalCells = maze->getRows() * maze->getCols();
    int visitedCells = 1;

    while (!stack.empty()) {
        current = stack.top();

        std::vector<Cell*> neighbors = maze->getUnvisitedNeighbors(current);

        if (!neighbors.empty()) {
            std::uniform_int_distribution<int> dist(0, neighbors.size() - 1);
            Cell* next = neighbors[dist(rng)];

            maze->removeWall(*current, *next);

            next->visited = true;
            stack.push(next);

            visitedCells++;
        }
        else {
            stack.pop();
        }
    }

    maze->clearVisited();

    std::cout << "Recursive Backtracker: " << visitedCells << "/" << totalCells
        << " cells processed." << std::endl;
}

void MazeGenerator::generatePrims() {
    maze->reset();

    std::vector<Cell*> frontiers;

    Cell* start = &maze->getCell(0, 0);
    start->visited = true;

    auto neighbors = maze->getUnvisitedNeighbors(start);
    for (auto n : neighbors) {
        frontiers.push_back(n);
    }

    int totalCells = maze->getRows() * maze->getCols();
    int visitedCells = 1;

    while (!frontiers.empty()) {
        std::uniform_int_distribution<int> dist(0, frontiers.size() - 1);
        int idx = dist(rng);
        Cell* current = frontiers[idx];
        frontiers.erase(frontiers.begin() + idx);

        std::vector<Cell*> visitedNeighbors;
        auto allNeighbors = maze->getNeighbors(current);
        for (auto n : allNeighbors) {
            if (n->visited) {
                visitedNeighbors.push_back(n);
            }
        }

        if (!visitedNeighbors.empty()) {
            std::uniform_int_distribution<int> dist2(0, visitedNeighbors.size() - 1);
            Cell* neighbor = visitedNeighbors[dist2(rng)];
            maze->removeWall(*current, *neighbor);

            current->visited = true;
            visitedCells++;

            auto newNeighbors = maze->getUnvisitedNeighbors(current);
            for (auto n : newNeighbors) {
                if (std::find(frontiers.begin(), frontiers.end(), n) == frontiers.end()) {
                    frontiers.push_back(n);
                }
            }
        }
    }

    maze->clearVisited();

    std::cout << "Prim's Algorithm: " << visitedCells << "/" << totalCells
        << " cells processed." << std::endl;
}

void MazeGenerator::generateKruskals() {
    maze->reset();

    int rows = maze->getRows();
    int cols = maze->getCols();
    int totalCells = rows * cols;


    struct Wall {
        Cell* cell1;
        Cell* cell2;
    };

    std::vector<Wall> walls;

 
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Cell* current = &maze->getCell(i, j);

          
            if (j < cols - 1) {
                walls.push_back({ current, &maze->getCell(i, j + 1) });
            }
           
            if (i < rows - 1) {
                walls.push_back({ current, &maze->getCell(i + 1, j) });
            }
        }
    }

    
    std::shuffle(walls.begin(), walls.end(), rng);

    
    UnionFind uf(totalCells);

    
    auto getCellId = [cols](Cell* cell) {
        return cell->row * cols + cell->col;
        };

    int wallsRemoved = 0;

    
    for (const auto& wall : walls) {
        int id1 = getCellId(wall.cell1);
        int id2 = getCellId(wall.cell2);

        
        if (uf.unite(id1, id2)) {
            maze->removeWall(*wall.cell1, *wall.cell2);
            wallsRemoved++;

            
            if (wallsRemoved == totalCells - 1) {
                break;
            }
        }
    }

    std::cout << "Kruskal's Algorithm: " << wallsRemoved << " walls removed, "
        << totalCells << " cells connected." << std::endl;
}

// ========== Eller's Algorithm ==========
void MazeGenerator::generateEllers() {
    maze->reset();

    int rows = maze->getRows();
    int cols = maze->getCols();

    
    std::vector<int> rowSets(cols);
    int nextSetId = 0;

    
    for (int i = 0; i < cols; i++) {
        rowSets[i] = nextSetId++;
    }

    for (int row = 0; row < rows; row++) {
        
        for (int col = 0; col < cols - 1; col++) {
            
            std::uniform_int_distribution<int> dist(0, 1);

            if (rowSets[col] != rowSets[col + 1] &&
                (row == rows - 1 || dist(rng) == 0)) {
                
                Cell* current = &maze->getCell(row, col);
                Cell* right = &maze->getCell(row, col + 1);
                maze->removeWall(*current, *right);

                
                int oldSet = rowSets[col + 1];
                int newSet = rowSets[col];
                for (int c = 0; c < cols; c++) {
                    if (rowSets[c] == oldSet) {
                        rowSets[c] = newSet;
                    }
                }
            }
        }

       
        if (row == rows - 1) {
            break;
        }

        
        std::map<int, std::vector<int>> setMembers;
        for (int col = 0; col < cols; col++) {
            setMembers[rowSets[col]].push_back(col);
        }

        std::vector<int> nextRowSets(cols, -1);

        for (auto& pair : setMembers) {
            int setId = pair.first;
            std::vector<int>& members = pair.second;

            
            std::shuffle(members.begin(), members.end(), rng);

            
            std::uniform_int_distribution<int> dist(0, 1);
            bool hasConnection = false;

            for (int col : members) {
                if (!hasConnection || dist(rng) == 0) {
                    
                    Cell* current = &maze->getCell(row, col);
                    Cell* below = &maze->getCell(row + 1, col);
                    maze->removeWall(*current, *below);

                    
                    nextRowSets[col] = setId;
                    hasConnection = true;
                }
                else {
                    
                    nextRowSets[col] = nextSetId++;
                }
            }
        }

        rowSets = nextRowSets;
    }

    std::cout << "Eller's Algorithm: Maze generated row by row, "
        << rows * cols << " cells processed." << std::endl;
}
// ========== Eller's ==========

void MazeGenerator::generate(MazeAlgorithm algorithm) {
    if (maze == nullptr) {
        std::cerr << "Error: Cannot generate maze - maze is null!" << std::endl;
        return;
    }

    switch (algorithm) {
    case RECURSIVE_BACKTRACKER:
        generateRecursiveBacktracker();
        break;
    case PRIMS:
        generatePrims();
        break;
    case KRUSKALS:
        generateKruskals();
        break;
    case ELLERS:  // ← 추가
        generateEllers();
        break;
    default:
        generateRecursiveBacktracker();
        break;
    }

    maze->getCell(0, 0).isStart = true;
    maze->getCell(maze->getRows() - 1, maze->getCols() - 1).isEnd = true;
}