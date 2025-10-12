#include "Player.h"

Player::Player(Maze* m) : maze(m), row(0), col(0) {
    if (maze == nullptr) {
        
        row = 0;
        col = 0;
    }
}

void Player::reset() {
    
    for (int i = 0; i < maze->getRows(); i++) {
        for (int j = 0; j < maze->getCols(); j++) {
            if (maze->getCell(i, j).isStart) {
                row = i;
                col = j;
                return;
            }
        }
    }
    
    row = 0;
    col = 0;
}

void Player::setPosition(int r, int c) {
    
    if (maze->isValidCell(r, c)) {
        row = r;
        col = c;
    }
}

bool Player::canMoveInDirection(int dr, int dc) const {
    const Cell& current = maze->getCell(row, col);

    
    if (dr == -1 && current.topWall) return false;    
    if (dr == 1 && current.bottomWall) return false; 
    if (dc == -1 && current.leftWall) return false;   
    if (dc == 1 && current.rightWall) return false;   

    return true;
}

bool Player::move(int dr, int dc) {
    
    if ((dr != 0 && dc != 0) || (dr == 0 && dc == 0)) {
        return false;
    }

    
    if (dr < -1 || dr > 1 || dc < -1 || dc > 1) {
        return false;
    }

    int newRow = row + dr;
    int newCol = col + dc;

    
    if (!maze->isValidCell(newRow, newCol)) {
        return false;
    }

    
    if (!canMoveInDirection(dr, dc)) {
        return false;
    }

    
    row = newRow;
    col = newCol;
    return true;
}

bool Player::hasWon() const {
    
    if (maze->isValidCell(row, col)) {
        return maze->getCell(row, col).isEnd;
    }
    return false;
}

bool Player::isAtStart() const {
    if (maze->isValidCell(row, col)) {
        return maze->getCell(row, col).isStart;
    }
    return false;
}

bool Player::isValidPosition() const {
    return maze->isValidCell(row, col);
}