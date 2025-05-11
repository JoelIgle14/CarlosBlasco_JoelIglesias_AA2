#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int VIEW_WIDTH = 20;
const int VIEW_HEIGHT = 10;

struct CJ {
    int x, y;
    char symbol;
};

struct Peaton {
    int x, y;
    bool vertical;
    bool vivo;
};

struct Coche {
    int x, y;
    bool ocupado;
};

vector<Coche> coches;
bool cjEnCoche = false;
vector<Peaton> peatones;
vector<pair<int, int>> dinero;

int dineroCJ = 0;
int maxDineroIsla = 100;
int numPeatones = 6;
int colMuro1, colMuro2, filaPuente;


void readConfig(int& rows, int& cols, int& numPeatones1, int& peaje1, int& maxDinero1,
    int& numPeatones2, int& peaje2, int& maxDinero2) {
    ifstream file("config.txt");
    if (!file.is_open()) {
        cerr << "No se pudo abrir config.txt\n";
        exit(1);
    }

    char sep;

    file >> cols >> sep >> rows;

    file >> numPeatones1 >> sep >> peaje1 >> sep >> maxDinero1;
    file >> numPeatones2 >> sep >> peaje2 >> sep >> maxDinero2;

    file.close();
}

char** createMap(int rows, int cols) {
    char** map = new char* [rows];
    for (int i = 0; i < rows; ++i) {
        map[i] = new char[cols];
        for (int j = 0; j < cols; ++j) {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                map[i][j] = 'X'; // bordes del mapa
            else
                map[i][j] = ' ';
        }
    }

    // División en islas: dos muros verticales con un puente (hueco en el centro)
    colMuro1 = cols / 3;
    colMuro2 = 2 * cols / 3;
    filaPuente = rows / 2;


    for (int i = 1; i < rows - 1; ++i) {
        if (i == filaPuente) continue;  // dejar hueco del puente

        if (colMuro1 > 0 && colMuro1 < cols - 1)
            map[i][colMuro1] = 'X';

        if (colMuro2 > 0 && colMuro2 < cols - 1)
            map[i][colMuro2] = 'X';
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

            bool drawn = false;
            if (mapRow == cj.y && mapCol == cj.x) {
                cout << cj.symbol;
                drawn = true;
            }

            for (auto& p : peatones) {
                if (p.vivo && mapRow == p.y && mapCol == p.x) {
                    cout << 'P';
                    drawn = true;
                    break;
                }
            }

            for (auto& d : dinero) {
                if (mapRow == d.second && mapCol == d.first) {
                    cout << '$';
                    drawn = true;
                    break;
                }
            }
            for (auto& c : coches) {
                if (!c.ocupado && mapRow == c.y && mapCol == c.x) {
                    cout << 'C';
                    drawn = true;
                    break;
                }
            }
            if (!drawn && mapRow < rows && mapCol < cols)
                cout << map[mapRow][mapCol];
            else if (!drawn)
                cout << ' ';
        }
        cout << '\n';
    }
    cout << "Dinero: " << dineroCJ << "\n";
}

bool canMove(char** map, int newY, int newX, int rows, int cols) {
    return newY >= 0 && newY < rows &&
        newX >= 0 && newX < cols &&
        map[newY][newX] != 'X';
}

Peaton crearPeaton(int rows, int cols, char** map) {
    Peaton p;
    bool ocupado;
    do {
        p.x = rand() % (cols - 2) + 1;
        p.y = rand() % (rows - 2) + 1;

        ocupado = false;
        for (auto& otro : peatones) {
            if (otro.vivo && otro.x == p.x && otro.y == p.y) {
                ocupado = true;
                break;
            }
        }
    } while (map[p.y][p.x] != ' ' || ocupado);
    p.vertical = rand() % 2;
    p.vivo = true;
    return p;
}

void generarPeatones(char** map, int rows, int cols, int num) {
    peatones.clear();
    for (int i = 0; i < num; ++i) {
        peatones.push_back(crearPeaton(rows, cols, map));
    }
}

void moverPeatones(char** map, CJ cj, int rows, int cols) {
    for (auto& p : peatones) {
        if (!p.vivo) continue;

        int dx = abs(p.x - cj.x);
        int dy = abs(p.y - cj.y);
        if (dx <= 1 && dy <= 1) continue;  // Si CJ está cerca, no se mueve

        int nx = p.x, ny = p.y;
        if (p.vertical) ny += (rand() % 2 ? 1 : -1);
        else nx += (rand() % 2 ? 1 : -1);

        bool libre = true;
        for (auto& otro : peatones) {
            if (&otro != &p && otro.vivo && otro.x == nx && otro.y == ny) {
                libre = false;
                break;
            }
        }
        for (auto& d : dinero) {
            if (d.first == nx && d.second == ny) {
                libre = false;
                break;
            }
        }

        if (canMove(map, ny, nx, rows, cols) && libre) {
            p.x = nx;
            p.y = ny;
        }
    }
}

void moveCJ(CJ& cj, char** map, int rows, int cols) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (canMove(map, cj.y - 1, cj.x, rows, cols)) {
            cj.y--;
            if (!cjEnCoche) cj.symbol = '^';
        }
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (canMove(map, cj.y + 1, cj.x, rows, cols)) {
            cj.y++;
            if (!cjEnCoche) cj.symbol = 'v';
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (canMove(map, cj.y, cj.x - 1, rows, cols)) {
            cj.x--;
            if (!cjEnCoche) cj.symbol = '<';
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (canMove(map, cj.y, cj.x + 1, rows, cols)) {
            cj.x++;
            if (!cjEnCoche) cj.symbol = '>';
        }
    }


    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        for (auto& p : peatones) {
            if (p.vivo && abs(cj.x - p.x) <= 1 && abs(cj.y - p.y) <= 1) {
                p.vivo = false;
                dinero.push_back({ p.x, p.y });

                // Regenerar nuevo peatón
                peatones.push_back(crearPeaton(rows, cols, map));
                break;
            }
        }
    }

    if (GetAsyncKeyState(0x45) & 0x8000) { // Tecla E
        if (!cjEnCoche) {
            for (auto& c : coches) {
                if (!c.ocupado && cj.x == c.x && cj.y == c.y) {
                    cjEnCoche = true;
                    c.ocupado = true;
                    cj.symbol = 'O'; // símbolo dentro del coche
                    break;
                }
            }
        }
        else {
            for (auto& c : coches) {
                if (c.ocupado) {
                    c.ocupado = false;
                    cjEnCoche = false;
                    cj.symbol = 'v'; // vuelve a pie
                    break;
                }
            }
        }
    }
}

Coche crearCoche(int rows, int cols, char** map) {
    Coche c;
    bool ocupado;
    do {
        c.x = rand() % (cols - 2) + 1;
        c.y = rand() % (rows - 2) + 1;

        ocupado = false;
        for (auto& p : peatones)
            if (p.x == c.x && p.y == c.y)
                ocupado = true;

        for (auto& d : dinero)
            if (d.first == c.x && d.second == c.y)
                ocupado = true;

    } while (map[c.y][c.x] != ' ' || ocupado);
    c.ocupado = false;
    return c;
}

void generarCoches(char** map, int rows, int cols, int cantidad) {
    for (int i = 0; i < cantidad; ++i) {
        coches.push_back(crearCoche(rows, cols, map));
    }
}

int main() {
    int rows, cols;
    int peaje1, peaje2;
    int maxDinero1, maxDinero2;
    int numPeatones1, numPeatones2;

    readConfig(rows, cols, numPeatones1, peaje1, maxDinero1, numPeatones2, peaje2, maxDinero2);

    char** map = createMap(rows, cols);

    CJ cj;
    cj.x = cols / 4;
    cj.y = rows / 2;
    cj.symbol = 'v';

    srand(time(0));
    generarPeatones(map, rows, cols, numPeatones1);
    generarCoches(map, rows, cols, 4);

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        moveCJ(cj, map, rows, cols);

        // Peaje 1 (Los Santos -> San Fierro)
        if (cj.x == colMuro1 && cj.y == filaPuente) {
            if (dineroCJ < peaje1) {
                cout << "¡No tienes suficiente dinero para cruzar a San Fierro!\n";
                Sleep(2000);
                exit(0);
            }
        }

        // Peaje 2 (San Fierro -> Las Venturas)
        if (cj.x == colMuro2 && cj.y == filaPuente) {
            if (dineroCJ < peaje2) {
                cout << "¡No tienes suficiente dinero para cruzar a Las Venturas!\n";
                Sleep(2000);
                exit(0);
            }
        }


        if (cjEnCoche) {
            for (auto& p : peatones) {
                if (p.vivo && p.x == cj.x && p.y == cj.y) {
                    p.vivo = false;
                    dinero.push_back({ p.x, p.y });

                    // Regenerar otro peatón
                    peatones.push_back(crearPeaton(rows, cols, map));
                    break;
                }
            }
        }

        moverPeatones(map, cj, rows, cols);
        drawView(map, rows, cols, cj);

        for (int i = 0; i < dinero.size(); ++i) {
            if (!cjEnCoche && cj.x == dinero[i].first && cj.y == dinero[i].second) {
                dineroCJ += rand() % maxDineroIsla + 1;
                dinero.erase(dinero.begin() + i);
                break;
            }
        }


        Sleep(100);
    }


    for (int i = 0; i < rows; ++i)
        delete[] map[i];
    delete[] map;

    return 0;
}
