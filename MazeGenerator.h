#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include "Maze.h"
#include <stack>
#include <random>
#include <vector>
#include <algorithm>
#include <set>
#include <map>

enum MazeAlgorithm {
    RECURSIVE_BACKTRACKER,
    PRIMS,
    KRUSKALS,
    ELLERS
};

class UnionFind {
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    UnionFind(int size);
    int find(int x);
    bool unite(int x, int y);
};

class MazeGenerator {
private:
    Maze* maze;
    std::mt19937 rng;

    void generateRecursiveBacktracker();
    void generatePrims();
    void generateKruskals();
    void generateEllers();
    void setSeed(unsigned int seed);

public:
    explicit MazeGenerator(Maze* m);

    void generate(MazeAlgorithm algorithm = RECURSIVE_BACKTRACKER);
    void generateWithSeed(unsigned int seed, MazeAlgorithm algorithm = RECURSIVE_BACKTRACKER);
};

#endif