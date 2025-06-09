#include <iostream>
#include <fstream>
#include <windows.h>
using namespace std;

const int VIEW_WIDTH = 20;
const int VIEW_HEIGHT = 10;

struct CJ {
    int x, y;
    char symbol;
};

void readConfig(int& rows, int& cols) {
    ifstream file("config.txt");
    if (!file.is_open()) {
        cerr << "No se pudo abrir config.txt\n";
        exit(1);
    }

    char sep;
    file >> cols >> sep >> rows;
    file.close();
}

char** createMap(int rows, int cols) {
    char** map = new char*[rows];
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


void drawView(char** map, int rows, int cols, CJ cj) {
    system("cls");

    int startRow = max(0, cj.y - VIEW_HEIGHT / 2);
    int startCol = max(0, cj.x - VIEW_WIDTH / 2);

    for (int i = 0; i < VIEW_HEIGHT; ++i) {
        for (int j = 0; j < VIEW_WIDTH; ++j) {
            int mapRow = startRow + i;
            int mapCol = startCol + j;

            if (mapRow == cj.y && mapCol == cj.x)
                cout << cj.symbol;
            else if (mapRow < rows && mapCol < cols)
                cout << map[mapRow][mapCol];
            else
                cout << ' ';
        }
        cout << '\n';
    }
}

bool canMove(char** map, int newY, int newX, int rows, int cols) {
    return newY >= 0 && newY < rows &&
        newX >= 0 && newX < cols &&
        map[newY][newX] != 'X'; // No puede pasar por paredes
}

void moveCJ(CJ& cj, char** map, int rows, int cols) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (canMove(map, cj.y - 1, cj.x, rows, cols)) {
            cj.y--;
            cj.symbol = '^';
        }
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (canMove(map, cj.y + 1, cj.x, rows, cols)) {
            cj.y++;
            cj.symbol = 'v';
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (canMove(map, cj.y, cj.x - 1, rows, cols)) {
            cj.x--;
            cj.symbol = '<';
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (canMove(map, cj.y, cj.x + 1, rows, cols)) {
            cj.x++;
            cj.symbol = '>';
        }
    }
}

int main() {
    int rows, cols;
    readConfig(rows, cols);

    char** map = createMap(rows, cols);

    CJ cj;
    cj.x = cols / 4;  // Empieza en el primer cuadrante
    cj.y = rows / 2;
    cj.symbol = 'v';

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        moveCJ(cj, map, rows, cols);
        drawView(map, rows, cols, cj);
        Sleep(100);
    }

    // Liberar memoria
    for (int i = 0; i < rows; ++i)
        delete[] map[i];
    delete[] map;

    return 0;
}
