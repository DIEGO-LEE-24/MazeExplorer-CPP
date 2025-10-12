#ifndef CONFIG_H
#define CONFIG_H

// Resolución de referencia (de diseño)
const int BASE_WIDTH = 1920;
const int BASE_HEIGHT = 1080;

// La tamaño real de la pantalla se determina en tiempo de ejecución.
extern int CURRENT_WIDTH;
extern int CURRENT_HEIGHT;

// función de cálculo del factor de escala
inline float getScaleX() {
    return (float)CURRENT_WIDTH / BASE_WIDTH;
}

inline float getScaleY() {
    return (float)CURRENT_HEIGHT / BASE_HEIGHT;
}

inline float getMinScale() {
    float sx = getScaleX();
    float sy = getScaleY();
    return (sx < sy) ? sx : sy;
}

// Funciones auxiliares que devuelven valores escalados
inline int scaleX(int x) {
    return (int)(x * getScaleX());
}

inline int scaleY(int y) {
    return (int)(y * getScaleY());
}

inline int scaleSize(int size) {
    return (int)(size * getMinScale());
}

// Dimensiones del laberinto
const int MAZE_ROWS = 35;
const int MAZE_COLS = 65;
const int CELL_SIZE = 28;

// Colores
struct Color {
    unsigned char r, g, b;
};

const Color COLOR_BG = { 20, 20, 30 };
const Color COLOR_WALL = { 200, 200, 200 };
const Color COLOR_PATH = { 50, 50, 60 };
const Color COLOR_START = { 0, 255, 0 };
const Color COLOR_END = { 255, 0, 0 };
const Color COLOR_SOLUTION = { 100, 150, 255 };
const Color COLOR_PLAYER = { 255, 100, 255 };

#endif