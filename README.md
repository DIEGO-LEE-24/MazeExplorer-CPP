# Maze Explorer - Generador y Explorador de Laberintos

## Descripción del Proyecto

Maze Explorer es un videojuego educativo avanzado desarrollado en C++ utilizando la librería Allegro 5. El proyecto implementa **cuatro algoritmos diferentes** de generación procedural de laberintos y resolución automática mediante BFS (Breadth-First Search), combinando conceptos avanzados de estructuras de datos, algoritmos de grafos, programación orientada a objetos y desarrollo de videojuegos.

## 🎥 Video Demostrativo

**[▶️ Ver Video Demostrativo Completo](https://youtu.be/Kt0ZhN3iNSI)**

El video demuestra todas las características del juego:
- ✅ Navegación por menús y selección de opciones
- ✅ Gameplay en los 3 niveles de dificultad (Fácil, Normal, Difícil)
- ✅ Los 4 algoritmos de generación en acción
- ✅ Modo Demo con visualización de BFS
- ✅ Sistema de estadísticas y persistencia de datos
- ✅ Todas las funcionalidades implementadas

---

## Características Principales

### Generación de Laberintos
- **Cuatro algoritmos de generación**:
  - **Recursive Backtracker (DFS)**: Genera laberintos con largos corredores y pocas bifurcaciones
  - **Prim's Algorithm**: Crea laberintos con muchas bifurcaciones y mayor complejidad
  - **Kruskal's Algorithm**: Produce laberintos con complejidad uniforme y equilibrada
  - **Eller's Algorithm**: Generación eficiente línea por línea con patrón horizontal

### Niveles de Dificultad
- **Fácil**: 15×25 celdas (375 celdas totales)
- **Normal**: 22×40 celdas (880 celdas totales)
- **Difícil**: 30×55 celdas (1,650 celdas totales)

### Modos de Juego
- **Modo Jugar**: Resuelve el laberinto manualmente
- **Modo Demo**: Observa cómo el algoritmo BFS resuelve automáticamente
- **Sistema de Estadísticas**: Seguimiento completo de rendimiento
- **Visualización en tiempo real**: Animación del proceso de solución en Demo

### Características Técnicas
- **Interfaz escalable**: Adaptación automática a cualquier resolución (probado hasta 2560×1600)
- **Diseño retro-arcade**: Estética inspirada en clásicos de los 80s-90s
- **Persistencia de datos**: Almacenamiento de estadísticas en archivo CSV
- **Arquitectura modular**: Código limpio con separación de responsabilidades

## Requisitos del Sistema

### Software Necesario
- **Compilador C++**: 
  - MinGW-w64 GCC 14.2.0 o superior, o
  - Visual Studio 2019 o posterior
  - Soporte para C++17
- **Allegro 5**: Versión 5.2.8 o superior
- **Sistema Operativo**: Windows 10/11 (64-bit)

### Addons de Allegro Requeridos
- `allegro-5.2` (core)
- `allegro_primitives-5.2`
- `allegro_font-5.2`
- `allegro_ttf-5.2`

## Estructura del Proyecto

```
MazeExplorer/
├── Config.h                 # Configuración global y funciones de escalado
├── Maze.h/cpp              # Clase principal del laberinto y estructura Cell
├── MazeGenerator.h/cpp     # Cuatro algoritmos de generación + Union-Find
├── MazeSolver.h/cpp        # Algoritmo de resolución BFS
├── Player.h/cpp            # Lógica del jugador y movimiento
├── Statistics.h/cpp        # Sistema de estadísticas y persistencia
├── MazeGame.cpp            # Punto de entrada y game loop principal
├── maze_stats.csv          # Archivo de estadísticas (generado automáticamente)
└── README.md               # Este archivo
```

## Compilación

### Opción 1: Visual Studio con NuGet (⭐ Recomendado)

1. Abrir Visual Studio
2. Crear nuevo proyecto "Aplicación de consola C++"
3. Agregar todos los archivos `.h` y `.cpp` al proyecto
4. Click derecho en el proyecto → "Administrar paquetes NuGet"
5. Buscar e instalar el paquete **"allegro"**
6. Compilar (F7) y ejecutar (Ctrl+F5)

### Opción 2: Code::Blocks con MinGW

1. Descargar Allegro 5 binarios para MinGW desde: https://liballeg.org/
2. Configurar rutas en Code::Blocks:
   ```
   Build options → Search directories → Compiler: [ruta_allegro]/include
   Build options → Search directories → Linker: [ruta_allegro]/lib
   Linker settings → Link libraries: liballegro_monolith-mt.a
   ```
3. Agregar define: `ALLEGRO_STATICLINK` (si enlace estático)
4. Compilar y ejecutar

### Opción 3: Línea de Comandos (g++)

```bash
g++ -std=c++17 MazeGame.cpp Maze.cpp MazeGenerator.cpp MazeSolver.cpp Player.cpp Statistics.cpp \
    -o MazeExplorer.exe \
    -I"C:/allegro/include" \
    -L"C:/allegro/lib" \
    -lallegro_monolith-mt \
    -mwindows
```

## Instrucciones de Uso

### Controles del Menú
- **Flechas ↑↓**: Navegar por las opciones
- **Enter**: Seleccionar opción
- **ESC**: Volver atrás / Salir

### Menú Principal
1. **JUGAR**: Iniciar una partida
   - Seleccionar dificultad (Fácil/Normal/Difícil)
   - Seleccionar algoritmo de generación
2. **DEMO**: Ver solución automática con BFS
3. **RESULTADOS**: Ver estadísticas globales
4. **INSTRUCCIONES**: Ver ayuda detallada
5. **SALIR**: Cerrar el programa

### Controles en el Juego
- **Flechas ↑↓←→**: Mover al jugador
- **S**: Mostrar/ocultar la solución óptima
- **Espacio**: Generar nuevo laberinto (mismo algoritmo y dificultad)
- **ESC**: Volver al menú principal

### Objetivo
Navegar desde el **punto verde** (inicio) hasta el **punto rojo** (meta) en el menor tiempo posible y con la menor cantidad de movimientos.

## Sistema de Estadísticas

### Métricas Registradas
- **Partidas totales**: Contador de juegos completados
- **Tiempo promedio y mejor tiempo**: En segundos
- **Movimientos promedio**: Número de pasos dados
- **Eficiencia**: Porcentaje de ruta óptima seguida
- **Estadísticas por dificultad**: Desglose por nivel

### Formato de Datos
Los datos se guardan en `maze_stats.csv`:
```
mazeRows,mazeCols,moves,timeSeconds,optimalPathLength
15,25,120,45.5,99
22,40,250,120.3,155
```

## Algoritmos Implementados

### 1. Recursive Backtracker (DFS)
**Características:**
- Complejidad temporal: O(n×m)
- Usa una pila para backtracking
- Genera laberintos con largos corredores
- Menor número de bifurcaciones

**Ventajas:**
- Implementación simple
- Laberintos desafiantes con rutas largas

### 2. Prim's Algorithm
**Características:**
- Complejidad temporal: O(n×m log(n×m))
- Basado en árboles de expansión mínima
- Muchas bifurcaciones y opciones
- Alta densidad de caminos

**Ventajas:**
- Laberintos más complejos
- Múltiples rutas posibles

### 3. Kruskal's Algorithm
**Características:**
- Complejidad temporal: O(n×m log(n×m))
- Usa estructura Union-Find
- Complejidad uniforme
- Balance entre longitud y bifurcaciones

**Ventajas:**
- Laberintos equilibrados
- Dificultad consistente

### 4. Eller's Algorithm
**Características:**
- Complejidad temporal: O(n×m)
- Genera línea por línea
- Memoria eficiente: O(m)
- Patrón horizontal distintivo

**Ventajas:**
- Muy eficiente en memoria
- Puede generar laberintos infinitos

### Resolución: Breadth-First Search (BFS)
**Características:**
- Complejidad temporal: O(n×m)
- Garantiza el camino más corto
- Explora nivel por nivel
- Usado en Modo Demo

## Arquitectura del Código

### Patrones de Diseño
- **Single Responsibility Principle**: Cada clase tiene una responsabilidad única
- **Strategy Pattern**: Diferentes algoritmos intercambiables
- **State Pattern**: Máquina de estados para flujo del juego
- **Encapsulation**: Datos privados con interfaces públicas claras

### Características Avanzadas de C++
- **STL Containers**: `vector`, `queue`, `map`, `unordered_map`, `stack`, `set`
- **Smart Memory Management**: Referencias y punteros gestionados
- **Templates y Genéricos**: Uso de templates STL
- **Exception Handling**: Validación y manejo robusto de errores
- **Modern Random**: `std::mt19937` para generación aleatoria de calidad
- **Lambda Functions**: Usado en algoritmos
- **C++11/17 Features**: Auto, range-based for loops, structured bindings

### Estructuras de Datos Implementadas
- **Union-Find (Disjoint Set)**: Para Kruskal's Algorithm
- **Grafos implícitos**: Representación del laberinto
- **Colas de prioridad**: En algoritmos de búsqueda
- **Árboles de expansión**: Base de los algoritmos de generación

## Resolución de Problemas Comunes

### Errores de Compilación

**"Cannot find allegro.h"**
```
Solución: Verificar rutas de inclusión en configuración del proyecto
```

**"Undefined reference to al_init"**
```
Solución: Verificar que las librerías estén enlazadas correctamente
```

**"std::map not found"**
```
Solución: Agregar #include <map> en MazeGenerator.h
```

### Errores de Ejecución

**Pantalla negra**
```
Solución: 
1. Verificar resolución mínima 1280×720
2. Ejecutar como administrador
3. Revisar consola (CMD) para mensajes de error
```

**"No se pudo cargar la fuente TTF"**
```
Solución: 
- Windows: Usa C:\Windows\Fonts\arial.ttf (automático)
- Otros: Cambiar ruta en MazeGame.cpp línea ~500
- Fallback: Usa fuente integrada automáticamente
```

**Estadísticas no se guardan**
```
Solución:
1. Verificar permisos de escritura
2. Ejecutar desde carpeta con permisos
3. Eliminar maze_stats.csv corrupto
```

### Problemas de Rendimiento

**Lag en laberintos grandes**
```
Solución: Es normal en DIFICIL (1650 celdas)
Recomendación: Usar FACIL o NORMAL para mejor fluidez
```

## Comparación de Algoritmos

| Algoritmo | Velocidad | Complejidad Laberinto | Bifurcaciones | Memoria |
|-----------|-----------|----------------------|---------------|---------|
| Recursive Backtracker | ⚡⚡⚡ | Media | Pocas | O(n×m) |
| Prim's | ⚡⚡ | Alta | Muchas | O(n×m) |
| Kruskal's | ⚡⚡ | Media-Alta | Balanceadas | O(n×m) |
| Eller's | ⚡⚡⚡ | Media | Horizontales | O(m) |

## Extensiones Futuras Posibles

- [ ] Más algoritmos (Aldous-Broder, Wilson's, etc.)
- [ ] Modo multijugador local
- [ ] Diferentes formas de laberinto (circular, hexagonal)
- [ ] Editor de laberintos
- [ ] Sistema de logros
- [ ] Música de fondo
- [ ] Efectos de partículas
- [ ] Modo tutorial interactivo

## Créditos y Referencias

### Desarrollador
**Proyecto Académico - Programación en C++**
- Curso: Programación Avanzada en C++
- Fecha: Octubre 2025

### Librerías Utilizadas
- **Allegro 5** (https://liballeg.org/)
  - Licencia: zlib/libpng
  - Framework multiplataforma para desarrollo de videojuegos

### Referencias Académicas
- **Jamis Buck**: "Mazes for Programmers" (Algoritmos de generación)
- **Cormen, Leiserson, Rivest, Stein**: "Introduction to Algorithms" (BFS, Union-Find)
- **Robert Sedgewick**: "Algorithms in C++" (Estructuras de datos)

### Algoritmos
- Recursive Backtracker: Basado en DFS clásico
- Prim's Algorithm: Adaptado de árboles de expansión mínima
- Kruskal's Algorithm: Con estructura Union-Find optimizada
- Eller's Algorithm: Implementación de generación línea por línea

## Testing y Validación

### Pruebas Realizadas
✅ Compilación en Visual Studio 2022  
✅ Compilación con MinGW GCC 14.2.0  
✅ Ejecución en Windows 10/11  
✅ Resoluciones probadas: 1920×1080, 2560×1600  
✅ Todos los algoritmos generan laberintos perfectos  
✅ BFS siempre encuentra solución óptima  
✅ Persistencia de datos funcional  
✅ Sin memory leaks (verificado con herramientas de debug)  

## Estadísticas del Proyecto

- **Líneas de código**: ~2,500
- **Archivos fuente**: 13 (7 .h + 6 .cpp)
- **Clases implementadas**: 6
- **Algoritmos implementados**: 5 (4 generación + 1 resolución)
- **Tiempo de desarrollo**: ~20 horas

## Licencia

Este proyecto es de uso **exclusivamente educativo**. El código fuente está disponible bajo términos académicos.

**Uso permitido:**
- Estudio personal
- Referencias educativas
- Proyectos académicos con atribución

**Uso NO permitido:**
- Distribución comercial
- Plagio en trabajos académicos
- Redistribución sin atribución

## Contacto y Soporte

Para preguntas sobre el proyecto:
- Documentación técnica incluida en cada archivo fuente
- Comentarios detallados en el código
- README.md (este archivo)

---

**Versión**: 2.0  
**Última actualización**: Octubre 2025  
**Estado**: Completado y Funcional  

**Nota**: Este proyecto representa la aplicación práctica de conceptos avanzados de programación en C++, estructuras de datos, algoritmos de grafos, y desarrollo de videojuegos.
```

---