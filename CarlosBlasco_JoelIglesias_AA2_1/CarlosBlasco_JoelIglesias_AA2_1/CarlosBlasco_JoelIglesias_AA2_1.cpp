#include <iostream>
#include <fstream>
#include <windows.h>
using namespace std;


// La vision del jugador, 20 casillas de ancho y 10 de alto
const int ancho = 20;
const int alto = 10;

int filaStart;
int columnaStart;
int filaMapa;
int columnaMapa;

struct CJ {
    int x, y;
    char simbolo; // El simbolo que representa al jugador, ya sea >, <, v, ^
};

// Abre y lee el archivo de config 
void leerArchivo(int& filas, int& columnas) {
    ifstream file("config.txt");
    if (!file.is_open()) {
        cerr << "No se pudo abrir";
        exit(1);
    }

    char sep;
    file >> columnas >> sep >> filas;
    file.close();
}

// Crea el mapa
char** crearMapa(int rows, int cols) {
    char** map = new char* [rows];
    for (int i = 0; i < rows; ++i) {
        map[i] = new char[cols];
        for (int j = 0; j < cols; ++j) {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                map[i][j] = 'X'; // bordes del mapa
            else
                map[i][j] = ' '; // zona transitable
        }
    }
    return map;
}

// Dibuja lo que ve el jugador
void dibujarVista(char** map, int rows, int cols, CJ cj) {
    system("cls");  // Limpia la pantalla antes de dibujar la nueva vista, para que no se solape
         
    // Recorremos cada casilla visiblee, solo las visibles, por eso usamos alto y ancho en lugar de el tamaño real del mapa entero, el de config.txt
    for (int i = 0; i < alto; ++i) {
        for (int j = 0; j < ancho; ++j) {

            // Calculamos la posición real del mapa que corresponde a esta casilla de la vista
            int mapY = cj.y - alto / 2 + i;  // fila del mapa
            int mapX = cj.x - ancho / 2 + j; // columna del mapa

            // Si esta posición es la misma que la del personaje, lo dibujamos
            if (mapY == cj.y && mapX == cj.x)
                cout << cj.simbolo;
            // Si está dentro del mapa, dibujamos lo que haya en esa casilla (pared o espacio)
            else if (mapY >= 0 && mapY < rows && mapX >= 0 && mapX < cols)
                cout << map[mapY][mapX];
          
            else
                cout << ' ';
        }
        cout << '\n';  // Salto de línea después de cada fila
    }
}



void moverCJ(CJ& cj, char** map, int rows, int cols) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (map [cj.y - 1][cj.x] != 'X') {
            cj.y--;
            cj.simbolo = '^';
        }
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (map[cj.y + 1][cj.x] != 'X') {
            cj.y++;
            cj.simbolo = 'v';
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (map[cj.y][cj.x - 1] != 'X') {
            cj.x--;
            cj.simbolo = '<';
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (map[cj.y][cj.x + 1] != 'X') {
            cj.x++;
            cj.simbolo = '>';
        }
    }
}

int main() {
    int rows, cols;
    leerArchivo(rows, cols);

    char** map = crearMapa(rows, cols);

    CJ cj;
    cj.x = cols / 4;  // La posicion dodne empieza, el medio
    cj.y = rows / 2;
    cj.simbolo = 'v';

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        moverCJ(cj, map, rows, cols);
        dibujarVista(map, rows, cols, cj);
        Sleep(100);
    }

    // Liberar memoria
    for (int i = 0; i < rows; ++i)
        delete[] map[i];
    delete[] map;

    return 0;
}
