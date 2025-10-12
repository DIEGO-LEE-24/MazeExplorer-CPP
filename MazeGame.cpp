#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <vector>
#include "Config.h"
#include "Maze.h"
#include "MazeGenerator.h"
#include "MazeSolver.h"
#include "Player.h"
#include "Statistics.h"
#include <allegro5/allegro_audio.h>      
#include <allegro5/allegro_acodec.h>

struct AlgorithmItem {
    const char* name;
    const char* description;
    MazeAlgorithm algorithm;
};

AlgorithmItem algorithmItems[] = {
    {"Recursive Backtracker", "Largos corredores, pocas bifurcaciones", RECURSIVE_BACKTRACKER},
    {"Prim's Algorithm", "Muchas bifurcaciones, mas complejo", PRIMS},
    {"Kruskal's Algorithm", "Complejidad uniforme, equilibrado", KRUSKALS},
    {"Eller's Algorithm", "Eficiente, patron horizontal", ELLERS}  
};

const int NUM_ALGORITHMS = 4;  

enum Difficulty {
    FACIL,
    NORMAL,
    DIFICIL
};

struct DifficultyConfig {
    const char* name;
    int rows;
    int cols;
    int cellSize;
};

DifficultyConfig difficulties[] = {
    {"FACIL", 15, 25, 35},
    {"NORMAL", 22, 40, 24},
    {"DIFICIL", 30, 55, 18}
};

enum GameState {
    MENU_PRINCIPAL,
    SELECCION_DIFICULTAD,
    SELECCION_ALGORITMO,
    JUGANDO,
    DEMO_MODE,
    ESTADISTICAS,
    INSTRUCCIONES,
    GANASTE
};

struct MenuItem {
    const char* text;
    int y;
};

MenuItem menuItems[] = {
    {"JUGAR", 400},
    {"DEMO", 460},
    {"RESULTADOS", 520},
    {"INSTRUCCIONES", 580},
    {"SALIR", 640}
};

const int NUM_MENU_ITEMS = 5;

int CURRENT_WIDTH = 1920;
int CURRENT_HEIGHT = 1080;

struct DemoState {
    std::vector<std::pair<int, int>> path;
    int currentStep;
    double lastMoveTime;
    bool isPlaying;
};

int countSolutionLength(Maze& maze) {
    int count = 0;
    for (int i = 0; i < maze.getRows(); i++) {
        for (int j = 0; j < maze.getCols(); j++) {
            if (maze.getCell(i, j).inSolution) {
                count++;
            }
        }
    }
    return count;
}

void buildDemoPath(Maze& maze, std::vector<std::pair<int, int>>& path) {
    path.clear();

    int startRow = -1, startCol = -1;
    for (int i = 0; i < maze.getRows(); i++) {
        for (int j = 0; j < maze.getCols(); j++) {
            if (maze.getCell(i, j).isStart) {
                startRow = i;
                startCol = j;
                break;
            }
        }
        if (startRow != -1) break;
    }

    std::vector<std::vector<bool>> visited(maze.getRows(),
        std::vector<bool>(maze.getCols(), false));
    std::vector<std::pair<int, int>> queue;
    std::vector<std::vector<std::pair<int, int>>> parent(maze.getRows(),
        std::vector<std::pair<int, int>>(maze.getCols(), { -1, -1 }));

    queue.push_back({ startRow, startCol });
    visited[startRow][startCol] = true;

    int endRow = -1, endCol = -1;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.erase(queue.begin());

        int row = current.first;
        int col = current.second;

        if (maze.getCell(row, col).isEnd) {
            endRow = row;
            endCol = col;
            break;
        }

        int dr[] = { -1, 1, 0, 0 };
        int dc[] = { 0, 0, -1, 1 };

        for (int i = 0; i < 4; i++) {
            int newRow = row + dr[i];
            int newCol = col + dc[i];

            if (newRow >= 0 && newRow < maze.getRows() &&
                newCol >= 0 && newCol < maze.getCols() &&
                !visited[newRow][newCol]) {

                bool canMove = false;
                if (i == 0 && !maze.getCell(row, col).topWall) canMove = true;
                if (i == 1 && !maze.getCell(row, col).bottomWall) canMove = true;
                if (i == 2 && !maze.getCell(row, col).leftWall) canMove = true;
                if (i == 3 && !maze.getCell(row, col).rightWall) canMove = true;

                if (canMove) {
                    visited[newRow][newCol] = true;
                    parent[newRow][newCol] = { row, col };
                    queue.push_back({ newRow, newCol });
                }
            }
        }
    }

    if (endRow != -1) {
        std::vector<std::pair<int, int>> reversePath;
        int r = endRow, c = endCol;

        while (r != -1 && c != -1) {
            reversePath.push_back({ r, c });
            auto p = parent[r][c];
            r = p.first;
            c = p.second;
        }

        for (int i = reversePath.size() - 1; i >= 0; i--) {
            path.push_back(reversePath[i]);
        }
    }
}

void renderBackground(int width, int height) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    int horizonY = height / 3;

    for (int i = 0; i < 20; i++) {
        int y = horizonY + (i * (height - horizonY) / 20);
        float depth = (float)i / 20.0f;

        int r = (int)(255 * (1.0f - depth * 0.7f));
        int g = (int)(0 * (1.0f - depth));
        int b = (int)(255 * (1.0f - depth * 0.5f));

        al_draw_line(0, y, width, y,
            al_map_rgb(r / 3, g, b / 3), depth < 0.5f ? 2 : 1);
    }

    for (int i = 0; i < 10; i++) {
        float xRatio = (float)i / 9.0f;
        int topX = (int)(width * xRatio);
        int bottomX = (int)(width / 2 + (topX - width / 2) * 2.5f);

        al_draw_line(topX, horizonY, bottomX, height,
            al_map_rgba(100, 0, 200, 80), 1);
    }

    for (int i = 0; i < 50; i++) {
        int x = (i * 157) % width;
        int y = (i * 73) % horizonY;
        int size = 1 + (i % 2);

        al_draw_filled_circle(x, y, size, al_map_rgb(200, 200, 255));
    }

    al_draw_rectangle(5, 5, width - 5, height - 5, al_map_rgb(0, 150, 255), 2);

    for (int y = 0; y < height; y += 4) {
        al_draw_line(0, y, width, y, al_map_rgba(0, 0, 0, 40), 1);
    }
}

void renderMenuPrincipal(ALLEGRO_FONT* font, int selectedItem) {
    renderBackground(CURRENT_WIDTH, CURRENT_HEIGHT);

    int centerX = CURRENT_WIDTH / 2;

    al_draw_text(font, al_map_rgb(100, 0, 150),
        centerX + 3, scaleY(200), ALLEGRO_ALIGN_CENTER, "MAZE");
    al_draw_text(font, al_map_rgb(100, 0, 150),
        centerX + 3, scaleY(260), ALLEGRO_ALIGN_CENTER, "EXPLORER");

    al_draw_text(font, al_map_rgb(255, 0, 255),
        centerX, scaleY(197), ALLEGRO_ALIGN_CENTER, "MAZE");
    al_draw_text(font, al_map_rgb(0, 255, 255),
        centerX, scaleY(257), ALLEGRO_ALIGN_CENTER, "EXPLORER");

    al_draw_filled_rectangle(centerX - scaleX(150), scaleY(320),
        centerX + scaleX(150), scaleY(322), al_map_rgb(255, 255, 0));

    al_draw_text(font, al_map_rgb(100, 255, 100),
        centerX, scaleY(350), ALLEGRO_ALIGN_CENTER, "PRESS START");

    for (int i = 0; i < NUM_MENU_ITEMS; i++) {
        int itemY = scaleY(menuItems[i].y);

        ALLEGRO_COLOR color;
        if (i == selectedItem) {
            al_draw_filled_rectangle(
                centerX - scaleX(200), itemY - scaleY(5),
                centerX + scaleX(200), itemY + scaleY(25),
                al_map_rgb(255, 255, 0)
            );
            al_draw_rectangle(
                centerX - scaleX(202), itemY - scaleY(7),
                centerX + scaleX(202), itemY + scaleY(27),
                al_map_rgb(255, 100, 0), 2
            );
            color = al_map_rgb(0, 0, 0);

            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX - scaleX(220), itemY,
                ALLEGRO_ALIGN_RIGHT, ">>");
            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX + scaleX(220), itemY,
                ALLEGRO_ALIGN_LEFT, "<<");
        }
        else {
            color = al_map_rgb(150, 200, 255);
        }

        al_draw_text(font, color, centerX, itemY,
            ALLEGRO_ALIGN_CENTER, menuItems[i].text);
    }

    al_draw_filled_rectangle(0, CURRENT_HEIGHT - scaleY(50),
        CURRENT_WIDTH, CURRENT_HEIGHT - scaleY(48),
        al_map_rgb(0, 200, 255));

    al_draw_text(font, al_map_rgb(255, 255, 0),
        centerX, CURRENT_HEIGHT - scaleY(35), ALLEGRO_ALIGN_CENTER,
        "ARROWS=MOVE  ENTER=SELECT");

    al_draw_text(font, al_map_rgb(255, 255, 255),
        scaleX(20), CURRENT_HEIGHT - scaleY(35), 0, "1UP");

    al_draw_text(font, al_map_rgb(255, 100, 100),
        CURRENT_WIDTH - scaleX(200), CURRENT_HEIGHT - scaleY(35), 0, "HI-SCORE");
}

void renderInstrucciones(ALLEGRO_FONT* font) {
    renderBackground(CURRENT_WIDTH, CURRENT_HEIGHT);

    al_draw_text(font, al_map_rgb(255, 255, 100),
        CURRENT_WIDTH / 2, scaleY(50), ALLEGRO_ALIGN_CENTER,
        "=== INSTRUCCIONES ===");

    const char* instrucciones[] = {
        "OBJETIVO:",
        "Llega desde el punto verde (inicio)",
        "hasta el punto rojo (meta)",
        "",
        "CONTROLES:",
        "Flechas - Mover jugador",
        "SPACE   - Nuevo laberinto",
        "S       - Mostrar/ocultar solucion",
        "ESC     - Volver al menu",
        "",
        "MODO DEMO:",
        "Ver como la IA resuelve el laberinto",
        "automaticamente con BFS",
        "",
        "Presiona ESC para volver"
    };

    int y = scaleY(150);
    for (int i = 0; i < 15; i++) {
        ALLEGRO_COLOR color = (i == 0 || i == 4 || i == 10) ?
            al_map_rgb(100, 200, 255) : al_map_rgb(200, 200, 200);
        al_draw_text(font, color, CURRENT_WIDTH / 2, y,
            ALLEGRO_ALIGN_CENTER, instrucciones[i]);
        y += scaleY(30);
    }
}

void renderSeleccionDificultad(ALLEGRO_FONT* font, int selectedDifficulty) {
    renderBackground(CURRENT_WIDTH, CURRENT_HEIGHT);

    int centerX = CURRENT_WIDTH / 2;

    al_draw_text(font, al_map_rgb(255, 0, 255),
        centerX, scaleY(60), ALLEGRO_ALIGN_CENTER, "SELECCIONA");
    al_draw_text(font, al_map_rgb(0, 255, 255),
        centerX, scaleY(90), ALLEGRO_ALIGN_CENTER, "DIFICULTAD");

    al_draw_filled_rectangle(centerX - scaleX(150), scaleY(125),
        centerX + scaleX(150), scaleY(127), al_map_rgb(255, 255, 0));

    int startY = scaleY(350);
    int spacing = scaleY(150);

    for (int i = 0; i < 3; i++) {
        int y = startY + (i * spacing);

        if (i == selectedDifficulty) {
            al_draw_filled_rectangle(
                centerX - scaleX(150), y - scaleY(5),
                centerX + scaleX(150), y + scaleY(80),
                al_map_rgb(255, 255, 0)
            );
            al_draw_rectangle(
                centerX - scaleX(152), y - scaleY(7),
                centerX + scaleX(152), y + scaleY(82),
                al_map_rgb(255, 100, 0), 3
            );

            al_draw_text(font, al_map_rgb(0, 0, 0),
                centerX, y, ALLEGRO_ALIGN_CENTER,
                difficulties[i].name);

            char buffer[100];
            sprintf_s(buffer, sizeof(buffer), "%dx%d celdas",
                difficulties[i].rows, difficulties[i].cols);
            al_draw_text(font, al_map_rgb(50, 50, 50),
                centerX, y + scaleY(25), ALLEGRO_ALIGN_CENTER, buffer);

            sprintf_s(buffer, sizeof(buffer), "Tamano: %dpx",
                difficulties[i].cellSize);
            al_draw_text(font, al_map_rgb(50, 50, 50),
                centerX, y + scaleY(50), ALLEGRO_ALIGN_CENTER, buffer);

            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX - scaleX(170), y + scaleY(25), ALLEGRO_ALIGN_RIGHT, ">>");
            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX + scaleX(170), y + scaleY(25), ALLEGRO_ALIGN_LEFT, "<<");
        }
        else {
            al_draw_rectangle(
                centerX - scaleX(150), y - scaleY(5),
                centerX + scaleX(150), y + scaleY(80),
                al_map_rgba(100, 150, 200, 100), 2
            );

            al_draw_text(font, al_map_rgb(150, 200, 255),
                centerX, y + scaleY(25), ALLEGRO_ALIGN_CENTER,
                difficulties[i].name);
        }
    }

    al_draw_filled_rectangle(0, CURRENT_HEIGHT - scaleY(50),
        CURRENT_WIDTH, CURRENT_HEIGHT - scaleY(48),
        al_map_rgb(0, 200, 255));

    al_draw_text(font, al_map_rgb(255, 255, 0),
        centerX, CURRENT_HEIGHT - scaleY(35), ALLEGRO_ALIGN_CENTER,
        "ARROWS=MOVE  ENTER=SELECT  ESC=BACK");
}

void renderSeleccionAlgoritmo(ALLEGRO_FONT* font, int selectedAlgorithm) {
    renderBackground(CURRENT_WIDTH, CURRENT_HEIGHT);

    int centerX = CURRENT_WIDTH / 2;

    al_draw_text(font, al_map_rgb(255, 0, 255),
        centerX, scaleY(60), ALLEGRO_ALIGN_CENTER, "SELECCIONA");
    al_draw_text(font, al_map_rgb(0, 255, 255),
        centerX, scaleY(90), ALLEGRO_ALIGN_CENTER, "ALGORITMO");

    al_draw_filled_rectangle(centerX - scaleX(150), scaleY(125),
        centerX + scaleX(150), scaleY(127), al_map_rgb(255, 255, 0));

    int startY = scaleY(280);  
    int spacing = scaleY(120); 

    for (int i = 0; i < NUM_ALGORITHMS; i++) {
        int y = startY + (i * spacing);

        if (i == selectedAlgorithm) {
            al_draw_filled_rectangle(
                centerX - scaleX(350), y - scaleY(5),      
                centerX + scaleX(350), y + scaleY(80),     
                al_map_rgb(255, 255, 0)
            );
            al_draw_rectangle(
                centerX - scaleX(352), y - scaleY(7),      
                centerX + scaleX(352), y + scaleY(82),     
                al_map_rgb(255, 100, 0), 3
            );

            al_draw_text(font, al_map_rgb(0, 0, 0),
                centerX, y, ALLEGRO_ALIGN_CENTER,
                algorithmItems[i].name);

            al_draw_text(font, al_map_rgb(50, 50, 50),
                centerX, y + scaleY(35), ALLEGRO_ALIGN_CENTER,
                algorithmItems[i].description);

            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX - scaleX(370), y + scaleY(25), ALLEGRO_ALIGN_RIGHT, ">>");  
            al_draw_text(font, al_map_rgb(255, 0, 0),
                centerX + scaleX(370), y + scaleY(25), ALLEGRO_ALIGN_LEFT, "<<");  
        }
        else {
            al_draw_rectangle(
                centerX - scaleX(350), y - scaleY(5),      
                centerX + scaleX(350), y + scaleY(80),     
                al_map_rgba(100, 150, 200, 100), 2
            );

            al_draw_text(font, al_map_rgb(150, 200, 255),
                centerX, y + scaleY(25), ALLEGRO_ALIGN_CENTER,
                algorithmItems[i].name);
        }
    }

    al_draw_filled_rectangle(0, CURRENT_HEIGHT - scaleY(50),
        CURRENT_WIDTH, CURRENT_HEIGHT - scaleY(48),
        al_map_rgb(0, 200, 255));

    al_draw_text(font, al_map_rgb(255, 255, 0),
        centerX, CURRENT_HEIGHT - scaleY(35), ALLEGRO_ALIGN_CENTER,
        "ARROWS=MOVE  ENTER=SELECT  ESC=BACK");
}

// ========== VARIABLES DE AUDIO ==========
ALLEGRO_SAMPLE* bgMusic1 = nullptr;
ALLEGRO_SAMPLE* bgMusic2 = nullptr;
ALLEGRO_SAMPLE_INSTANCE* musicInstance = nullptr;
ALLEGRO_SAMPLE* explosionSound = nullptr;
ALLEGRO_SAMPLE* tieSound = nullptr;
ALLEGRO_SAMPLE* xwingSound = nullptr;

int currentMusicTrack = 0;
float musicVolume = 0.5f;
float sfxVolume = 0.7f;

// ========== FUNCIONES HELPER DE AUDIO ==========
void playSound(ALLEGRO_SAMPLE* sample, float gain = 1.0f) {
    if (sample) {
        al_play_sample(sample, gain * sfxVolume, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void switchMusic(int track) {
    if (!musicInstance) return;

    al_stop_sample_instance(musicInstance);
    al_detach_sample_instance(musicInstance);

    if (track == 0 && bgMusic1) {
        al_set_sample(musicInstance, bgMusic1);
    }
    else if (track == 1 && bgMusic2) {
        al_set_sample(musicInstance, bgMusic2);
    }

    al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());
    al_play_sample_instance(musicInstance);
    currentMusicTrack = track;
}

void toggleMusic() {
    if (!musicInstance) return;

    if (al_get_sample_instance_playing(musicInstance)) {
        al_stop_sample_instance(musicInstance);
    }
    else {
        al_play_sample_instance(musicInstance);
    }
}
int main() {
    al_init();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();

    // ========== INICIALIZAR AUDIO (INSERTAR AQUÍ) ==========
    printf("\n=== Inicializando sistema de audio ===\n");

    if (!al_install_audio()) {
        printf("Error: No se pudo inicializar audio\n");
    }
    else {
        printf("✓ Audio inicializado\n");
    }

    if (!al_init_acodec_addon()) {
        printf("Error: No se pudo inicializar codec de audio\n");
    }
    else {
        printf("✓ Codec de audio inicializado\n");
    }

    al_reserve_samples(8);
    printf("✓ 8 canales de audio reservados\n");

    // ========== CARGAR ARCHIVOS DE AUDIO ==========
    printf("\n=== Cargando archivos de audio ===\n");

    bgMusic1 = al_load_sample("C:/Users/diego/Downloads/MazeGame/Musica.wav");
    if (bgMusic1) printf("✓ Musica.wav cargada\n");
    else printf("✗ Error al cargar Musica.wav\n");

    bgMusic2 = al_load_sample("C:/Users/diego/Downloads/MazeGame/Musica2.wav");
    if (bgMusic2) printf("✓ Musica2.wav cargada\n");
    else printf("✗ Error al cargar Musica2.wav\n");

    explosionSound = al_load_sample("C:/Users/diego/Downloads/MazeGame/explo.wav");
    if (explosionSound) printf("✓ explo.wav cargado\n");
    else printf("✗ Error al cargar explo.wav\n");

    tieSound = al_load_sample("C:/Users/diego/Downloads/MazeGame/Tie fighter blaster.wav");
    if (tieSound) printf("✓ Tie fighter blaster.wav cargado\n");
    else printf("✗ Error al cargar Tie fighter blaster.wav\n");

    xwingSound = al_load_sample("C:/Users/diego/Downloads/MazeGame/X wing fighter blaster.wav");
    if (xwingSound) printf("✓ X wing fighter blaster.wav cargado\n");
    else printf("✗ Error al cargar X wing fighter blaster.wav\n");

    // ========== INICIAR MÚSICA DE FONDO ==========
    if (bgMusic1) {
        musicInstance = al_create_sample_instance(bgMusic1);
        if (musicInstance) {
            al_set_sample_instance_playmode(musicInstance, ALLEGRO_PLAYMODE_LOOP);
            al_set_sample_instance_gain(musicInstance, musicVolume);
            al_attach_sample_instance_to_mixer(musicInstance, al_get_default_mixer());
            al_play_sample_instance(musicInstance);
            printf("✓ Música de fondo iniciada (Track 1)\n");
        }
    }
    printf("=== Sistema de audio listo ===\n\n");
    // ========== FIN DE INICIALIZACIÓN DE AUDIO ==========

    ALLEGRO_DISPLAY_MODE disp_data;
    al_get_display_mode(0, &disp_data);

    CURRENT_WIDTH = disp_data.width;
    CURRENT_HEIGHT = disp_data.height;

    printf("Resolucion detectada: %dx%d\n", CURRENT_WIDTH, CURRENT_HEIGHT);
    printf("Scale X: %.2f, Scale Y: %.2f\n", getScaleX(), getScaleY());

    ALLEGRO_DISPLAY* display = al_create_display(CURRENT_WIDTH, CURRENT_HEIGHT);
    al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, true);

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    int fontSize = scaleSize(32);
    ALLEGRO_FONT* font = al_load_ttf_font("C:\\Windows\\Fonts\\arial.ttf", fontSize, 0);

    if (!font) {
        printf("No se pudo cargar la fuente TTF, usando built-in\n");
        font = al_create_builtin_font();
    }

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());

    Maze maze(MAZE_ROWS, MAZE_COLS);
    MazeGenerator generator(&maze);
    MazeSolver solver(&maze);
    Player player(&maze);
    Statistics stats("maze_stats.csv");

    GameState state = MENU_PRINCIPAL;
    int selectedMenuItem = 0;
    int selectedDifficulty = 1;
    int selectedAlgorithm = 0;
    Difficulty currentDifficulty = NORMAL;
    bool running = true;
    bool showSolution = false;
    int moves = 0;
    time_t startTime;

    DemoState demoState;
    demoState.isPlaying = false;
    demoState.currentStep = 0;
    demoState.lastMoveTime = 0;

    printf("\n=== MAZE EXPLORER ===\n");
    printf("Sistema de menus iniciado\n\n");

    while (running) {
        if (state == DEMO_MODE && demoState.isPlaying) {
            double currentTime = al_get_time();
            if (currentTime - demoState.lastMoveTime > 0.2) {
                if (demoState.currentStep < demoState.path.size()) {
                    demoState.currentStep++;
                    demoState.lastMoveTime = currentTime;
                }
                else {
                    demoState.isPlaying = false;
                }
            }
        }

        switch (state) {
        case MENU_PRINCIPAL:
            renderMenuPrincipal(font, selectedMenuItem);
            break;

        case INSTRUCCIONES:
            renderInstrucciones(font);
            break;

        case SELECCION_DIFICULTAD:
            renderSeleccionDificultad(font, selectedDifficulty);
            break;
        case SELECCION_ALGORITMO:
            renderSeleccionAlgoritmo(font, selectedAlgorithm);
            break;


        case ESTADISTICAS: {
            renderBackground(CURRENT_WIDTH, CURRENT_HEIGHT);

            al_draw_text(font, al_map_rgb(255, 255, 0),
                CURRENT_WIDTH / 2, scaleY(50), ALLEGRO_ALIGN_CENTER,
                "=== ESTADISTICAS ===");

            char buffer[100];
            sprintf_s(buffer, sizeof(buffer), "Partidas jugadas: %d", stats.getTotalGames());
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH / 2, scaleY(120), ALLEGRO_ALIGN_CENTER, buffer);

            sprintf_s(buffer, sizeof(buffer), "Tiempo promedio: %.1f seg", stats.getAverageTime());
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH / 2, scaleY(160), ALLEGRO_ALIGN_CENTER, buffer);

            sprintf_s(buffer, sizeof(buffer), "Mejor tiempo: %d seg", stats.getBestTime());
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH / 2, scaleY(200), ALLEGRO_ALIGN_CENTER, buffer);

            sprintf_s(buffer, sizeof(buffer), "Eficiencia promedio: %.1f%%",
                stats.getAverageEfficiency());
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH / 2, scaleY(240), ALLEGRO_ALIGN_CENTER, buffer);

            al_draw_text(font, al_map_rgb(200, 200, 200),
                CURRENT_WIDTH / 2, scaleY(320), ALLEGRO_ALIGN_CENTER,
                "Presiona ESC para volver");
            break;
        }

        case DEMO_MODE: {
            int baseCellSize = difficulties[selectedDifficulty].cellSize;

            int maxWidth = CURRENT_WIDTH * 0.9;
            int maxHeight = CURRENT_HEIGHT * 0.9;

            int maxCellSizeByWidth = maxWidth / maze.getCols();
            int maxCellSizeByHeight = maxHeight / maze.getRows();

            int cellSize = (maxCellSizeByWidth < maxCellSizeByHeight) ?
                maxCellSizeByWidth : maxCellSizeByHeight;

            if (cellSize < baseCellSize) cellSize = baseCellSize;

            int mazeWidth = maze.getCols() * cellSize;
            int mazeHeight = maze.getRows() * cellSize;
            int offsetX = (CURRENT_WIDTH - mazeWidth) / 2;
            int offsetY = (CURRENT_HEIGHT - mazeHeight) / 2;

            al_clear_to_color(al_map_rgb(COLOR_BG.r, COLOR_BG.g, COLOR_BG.b));

            for (int i = 0; i < maze.getRows(); i++) {
                for (int j = 0; j < maze.getCols(); j++) {
                    Cell& cell = maze.getCell(i, j);
                    int x = offsetX + (j * cellSize);
                    int y = offsetY + (i * cellSize);

                    if (cell.isStart) {
                        al_draw_filled_rectangle(x, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_START.r, COLOR_START.g, COLOR_START.b));
                    }

                    if (cell.isEnd) {
                        al_draw_filled_rectangle(x, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_END.r, COLOR_END.g, COLOR_END.b));
                    }

                    if (cell.topWall) {
                        al_draw_line(x, y, x + cellSize, y,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.rightWall) {
                        al_draw_line(x + cellSize, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.bottomWall) {
                        al_draw_line(x, y + cellSize, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.leftWall) {
                        al_draw_line(x, y, x, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                }
            }

            for (int i = 0; i < demoState.currentStep && i < demoState.path.size(); i++) {
                auto pos = demoState.path[i];
                int x = offsetX + (pos.second * cellSize);
                int y = offsetY + (pos.first * cellSize);
                al_draw_filled_rectangle(x + 5, y + 5, x + cellSize - 5, y + cellSize - 5,
                    al_map_rgba(100, 150, 255, 150));
            }

            if (demoState.currentStep < demoState.path.size()) {
                auto pos = demoState.path[demoState.currentStep];
                int px = offsetX + (pos.second * cellSize);
                int py = offsetY + (pos.first * cellSize);
                al_draw_filled_circle(px + cellSize / 2, py + cellSize / 2, cellSize / 3,
                    al_map_rgb(COLOR_PLAYER.r, COLOR_PLAYER.g, COLOR_PLAYER.b));
            }

            char buffer[100];
            sprintf_s(buffer, sizeof(buffer), "Paso: %d / %d",
                demoState.currentStep, (int)demoState.path.size());
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH - scaleX(250), scaleY(20), 0, buffer);

            al_draw_text(font, al_map_rgb(100, 255, 100),
                CURRENT_WIDTH / 2, scaleY(20), ALLEGRO_ALIGN_CENTER,
                "MODO DEMO - Solucion Automatica");

            break;
        }

        case JUGANDO:
        case GANASTE: {
            int baseCellSize = difficulties[selectedDifficulty].cellSize;

            int maxWidth = CURRENT_WIDTH * 0.9;
            int maxHeight = CURRENT_HEIGHT * 0.9;

            int maxCellSizeByWidth = maxWidth / maze.getCols();
            int maxCellSizeByHeight = maxHeight / maze.getRows();

            int cellSize = (maxCellSizeByWidth < maxCellSizeByHeight) ?
                maxCellSizeByWidth : maxCellSizeByHeight;

            if (cellSize < baseCellSize) cellSize = baseCellSize;

            int mazeWidth = maze.getCols() * cellSize;
            int mazeHeight = maze.getRows() * cellSize;
            int offsetX = (CURRENT_WIDTH - mazeWidth) / 2;
            int offsetY = (CURRENT_HEIGHT - mazeHeight) / 2;

            al_clear_to_color(al_map_rgb(COLOR_BG.r, COLOR_BG.g, COLOR_BG.b));

            for (int i = 0; i < maze.getRows(); i++) {
                for (int j = 0; j < maze.getCols(); j++) {
                    Cell& cell = maze.getCell(i, j);
                    int x = offsetX + (j * cellSize);
                    int y = offsetY + (i * cellSize);

                    if (showSolution && cell.inSolution && !cell.isStart && !cell.isEnd) {
                        al_draw_filled_rectangle(x + 5, y + 5, x + cellSize - 5, y + cellSize - 5,
                            al_map_rgb(COLOR_SOLUTION.r, COLOR_SOLUTION.g, COLOR_SOLUTION.b));
                    }

                    if (cell.isStart) {
                        al_draw_filled_rectangle(x, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_START.r, COLOR_START.g, COLOR_START.b));
                    }

                    if (cell.isEnd) {
                        al_draw_filled_rectangle(x, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_END.r, COLOR_END.g, COLOR_END.b));
                    }

                    if (cell.topWall) {
                        al_draw_line(x, y, x + cellSize, y,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.rightWall) {
                        al_draw_line(x + cellSize, y, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.bottomWall) {
                        al_draw_line(x, y + cellSize, x + cellSize, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                    if (cell.leftWall) {
                        al_draw_line(x, y, x, y + cellSize,
                            al_map_rgb(COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b), 2);
                    }
                }
            }

            int px = offsetX + (player.getCol() * cellSize);
            int py = offsetY + (player.getRow() * cellSize);
            al_draw_filled_circle(px + cellSize / 2, py + cellSize / 2, cellSize / 3,
                al_map_rgb(COLOR_PLAYER.r, COLOR_PLAYER.g, COLOR_PLAYER.b));

            char buffer[100];
            sprintf_s(buffer, sizeof(buffer), "Movimientos: %d", moves);
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH - scaleX(250), scaleY(20), 0, buffer);

            int elapsed = (int)difftime(time(NULL), startTime);
            sprintf_s(buffer, sizeof(buffer), "Tiempo: %d seg", elapsed);
            al_draw_text(font, al_map_rgb(255, 255, 255),
                CURRENT_WIDTH - scaleX(250), scaleY(50), 0, buffer);

            if (state == GANASTE) {
                al_draw_filled_rectangle(
                    CURRENT_WIDTH / 2 - scaleX(250), CURRENT_HEIGHT / 2 - scaleY(40),
                    CURRENT_WIDTH / 2 + scaleX(250), CURRENT_HEIGHT / 2 + scaleY(40),
                    al_map_rgba(0, 0, 0, 180)
                );
                al_draw_text(font, al_map_rgb(0, 255, 0),
                    CURRENT_WIDTH / 2, CURRENT_HEIGHT / 2 - scaleY(20), ALLEGRO_ALIGN_CENTER,
                    "GANASTE!");
                al_draw_text(font, al_map_rgb(200, 200, 200),
                    CURRENT_WIDTH / 2, CURRENT_HEIGHT / 2 + scaleY(10), ALLEGRO_ALIGN_CENTER,
                    "Presiona ESC para volver al menu");
            }
            break;
        }
        }

        al_flip_display();

        ALLEGRO_EVENT event;
        if (al_get_next_event(queue, &event)) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = false;
            }
            else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (state == MENU_PRINCIPAL) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                        playSound(tieSound, 0.3f);
                        selectedMenuItem = (selectedMenuItem - 1 + NUM_MENU_ITEMS) % NUM_MENU_ITEMS;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        playSound(tieSound, 0.3f);
                        selectedMenuItem = (selectedMenuItem + 1) % NUM_MENU_ITEMS;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        playSound(xwingSound, 0.5f);
                        if (selectedMenuItem == 0) {
                            state = SELECCION_DIFICULTAD;
                        }
                        else if (selectedMenuItem == 1) {
                            generator.generate(PRIMS);
                            solver.solveBFS();
                            player.reset();
                            moves = 0;
                            startTime = time(NULL);
                            showSolution = false;

                            buildDemoPath(maze, demoState.path);
                            demoState.currentStep = 0;
                            demoState.isPlaying = true;
                            demoState.lastMoveTime = al_get_time();

                            state = DEMO_MODE;
                        }
                        else if (selectedMenuItem == 2) {
                            stats.printSummary();
                            state = ESTADISTICAS;
                        }
                        else if (selectedMenuItem == 3) {
                            state = INSTRUCCIONES;
                        }
                        else if (selectedMenuItem == 4) {
                            running = false;
                        }
                    }
                }
                else if (state == JUGANDO) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        state = MENU_PRINCIPAL;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_S) {
                        showSolution = !showSolution;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                        generator.generate();
                        solver.solveBFS();
                        player.reset();
                        moves = 0;
                        startTime = time(NULL);
                    }
                    else {
                        bool moved = false;
                        if (event.keyboard.keycode == ALLEGRO_KEY_UP) moved = player.move(-1, 0);
                        else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) moved = player.move(1, 0);
                        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) moved = player.move(0, -1);
                        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) moved = player.move(0, 1);

                        if (moved) {
                            playSound(tieSound, 0.2f);
                            moves++;
                            if (player.hasWon()) {
                                playSound(explosionSound, 0.8f);
                                state = GANASTE;
                                GameStats gameStats;
                                gameStats.mazeRows = difficulties[selectedDifficulty].rows;
                                gameStats.mazeCols = difficulties[selectedDifficulty].cols;
                                gameStats.moves = moves;
                                gameStats.timeSeconds = difftime(time(NULL), startTime);
                                gameStats.optimalPathLength = countSolutionLength(maze);
                                stats.addGame(gameStats);
                                stats.saveToFile();
                            }
                        }
                    }
                }
                else if (state == SELECCION_DIFICULTAD) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                        playSound(tieSound, 0.3f);
                        selectedDifficulty = (selectedDifficulty - 1 + 3) % 3;
                    }

                    else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        playSound(tieSound, 0.3f);
                        selectedDifficulty = (selectedDifficulty + 1) % 3;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        playSound(xwingSound, 0.5f);
                        currentDifficulty = (Difficulty)selectedDifficulty;
                        DifficultyConfig config = difficulties[selectedDifficulty];

                        Maze newMaze(config.rows, config.cols);
                        maze = newMaze;

                        generator = MazeGenerator(&maze);
                        solver = MazeSolver(&maze);
                        player = Player(&maze);

                        generator.generate();
                        solver.solveBFS();
                        player.reset();
                        moves = 0;
                        startTime = time(NULL);
                        showSolution = false;
                        state = SELECCION_ALGORITMO;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        state = MENU_PRINCIPAL;
                    }
                }
                else if (state == SELECCION_ALGORITMO) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                        playSound(tieSound, 0.3f);
                        selectedAlgorithm = (selectedAlgorithm - 1 + NUM_ALGORITHMS) % NUM_ALGORITHMS;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                        playSound(tieSound, 0.3f);
                        selectedAlgorithm = (selectedAlgorithm + 1) % NUM_ALGORITHMS;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                        playSound(xwingSound, 0.5f);
                        currentDifficulty = (Difficulty)selectedDifficulty;
                        DifficultyConfig config = difficulties[selectedDifficulty];

                        Maze newMaze(config.rows, config.cols);
                        maze = newMaze;

                        generator = MazeGenerator(&maze);
                        solver = MazeSolver(&maze);
                        player = Player(&maze);

                        generator.generate(algorithmItems[selectedAlgorithm].algorithm);
                        solver.solveBFS();
                        player.reset();
                        moves = 0;
                        startTime = time(NULL);
                        showSolution = false;
                        state = JUGANDO;
                    }
                    else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        state = SELECCION_DIFICULTAD;
                    }
}
                else if (state == INSTRUCCIONES || state == ESTADISTICAS ||
                    state == DEMO_MODE || state == GANASTE) {
                    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        state = MENU_PRINCIPAL;
                    }
                }

                if (event.keyboard.keycode == ALLEGRO_KEY_M) {
                    switchMusic((currentMusicTrack + 1) % 2);
                }
                if (event.keyboard.keycode == ALLEGRO_KEY_P) {
                    toggleMusic();
                }
            }
        }


        al_rest(0.016);
    }

    // ========== LIBERAR RECURSOS DE AUDIO ==========
    printf("\n=== Liberando recursos de audio ===\n");

    if (musicInstance) {
        al_stop_sample_instance(musicInstance);
        al_destroy_sample_instance(musicInstance);
        printf("✓ Instancia de música destruida\n");
    }

    if (bgMusic1) al_destroy_sample(bgMusic1);
    if (bgMusic2) al_destroy_sample(bgMusic2);
    if (explosionSound) al_destroy_sample(explosionSound);
    if (tieSound) al_destroy_sample(tieSound);
    if (xwingSound) al_destroy_sample(xwingSound);

    printf("✓ Todos los recursos de audio liberados\n");
    // ========== FIN DE LIMPIEZA DE AUDIO ==========

    al_destroy_font(font);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}