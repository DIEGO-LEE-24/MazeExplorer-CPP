#ifndef PLAYER_H
#define PLAYER_H

#include "Maze.h"

class Player {
private:
    int row, col;
    Maze* maze;

    // Verificar si es posible moverse en una dirección (chequeo de paredes)
    bool canMoveInDirection(int dr, int dc) const;

public:
    explicit Player(Maze* m);

    // Getters
    int getRow() const { return row; }
    int getCol() const { return col; }

    // Establecer posición específica (modo demo)
    void setPosition(int r, int c);

    // Reiniciar a la posición inicial
    void reset();

    // Intentar moverse (devuelve true si tiene éxito)
    bool move(int dr, int dc);

    // Verificar si ha llegado al punto objetivo
    bool hasWon() const;

    // Verificar si está en el punto de inicio
    bool isAtStart() const;

    // Verificar si la posición actual es válida
    bool isValidPosition() const;
};


#endif