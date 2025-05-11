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

    int startRow = cj.y - VIEW_HEIGHT / 2;
    int startCol = cj.x - VIEW_WIDTH / 2;

    // Ajuste para que la vista no se salga del mapa
    if (startRow < 0) startRow = 0;
    if (startCol < 0) startCol = 0;
    if (startRow + VIEW_HEIGHT > rows) startRow = rows - VIEW_HEIGHT;
    if (startCol + VIEW_WIDTH > cols) startCol = cols - VIEW_WIDTH;

    for (int i = 0; i < VIEW_HEIGHT; ++i) {
        for (int j = 0; j < VIEW_WIDTH; ++j) {
            int mapRow = startRow + i;
            int mapCol = startCol + j;

            bool drawn = false;

            // Dibujar CJ
            if (mapRow == cj.y && mapCol == cj.x) {
                cout << cj.symbol;
                drawn = true;
            }

            // Dibujar peatones
            if (!drawn) {
                for (int k = 0; k < peatones.size(); ++k) {
                    if (peatones[k].vivo && mapRow == peatones[k].y && mapCol == peatones[k].x) {
                        cout << 'P';
                        drawn = true;
                        break;
                    }
                }
            }

            // Dibujar dinero
            if (!drawn) {
                for (int k = 0; k < dinero.size(); ++k) {
                    if (mapRow == dinero[k].second && mapCol == dinero[k].first) {
                        cout << '$';
                        drawn = true;
                        break;
                    }
                }
            }

            // Dibujar coches
            if (!drawn) {
                for (int k = 0; k < coches.size(); ++k) {
                    if (!coches[k].ocupado && mapRow == coches[k].y && mapCol == coches[k].x) {
                        cout << 'C';
                        drawn = true;
                        break;
                    }
                }
            }

            // Dibujar mapa o espacio vacío
            if (!drawn) {
                if (mapRow >= 0 && mapRow < rows && mapCol >= 0 && mapCol < cols)
                    cout << map[mapRow][mapCol];
                else
                    cout << ' ';
            }
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

void generarPeatonesPorIsla(char** map, int rows, int cols, int num1, int num2, int num3) {
    peatones.clear();

    // Isla 1 (col < colMuro1)
    for (int i = 0; i < num1; ++i) {
        Peaton p;
        do {
            p.x = rand() % (colMuro1 - 2) + 1;
            p.y = rand() % (rows - 2) + 1;
        } while (map[p.y][p.x] != ' ');
        p.vivo = true;
        p.vertical = rand() % 2;
        peatones.push_back(p);
    }

    // Isla 2 (entre muros)
    for (int i = 0; i < num2; ++i) {
        Peaton p;
        do {
            p.x = rand() % (colMuro2 - colMuro1 - 2) + colMuro1 + 1;
            p.y = rand() % (rows - 2) + 1;
        } while (map[p.y][p.x] != ' ');
        p.vivo = true;
        p.vertical = rand() % 2;
        peatones.push_back(p);
    }

    // Isla 3 (col > colMuro2)
    for (int i = 0; i < num3; ++i) {
        Peaton p;
        do {
            p.x = rand() % (cols - colMuro2 - 2) + colMuro2 + 1;
            p.y = rand() % (rows - 2) + 1;
        } while (map[p.y][p.x] != ' ');
        p.vivo = true;
        p.vertical = rand() % 2;
        peatones.push_back(p);
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
            for (std::vector<Coche>::iterator it = coches.begin(); it != coches.end(); ++it) {
                if (!it->ocupado && cj.x == it->x && cj.y == it->y && map[cj.y][cj.x] != 'X') {
                    cjEnCoche = true;
                    it->ocupado = true;
                    cj.symbol = 'O'; // símbolo dentro del coche
                    break;
                }
            }
        }

        else {
            for (auto& c : coches) {
                if (c.ocupado && cj.x == c.x && cj.y == c.y) {
                    c.ocupado = false;
                    cjEnCoche = false;
                    cj.symbol = 'v';
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

    } while (map[c.y][c.x] == 'X' || map[c.y][c.x] != ' ' || ocupado);

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
    generarPeatonesPorIsla(map, rows, cols, numPeatones1, numPeatones2, 4); // 4 es el d la isla 3
    generarCoches(map, rows, cols, 4);

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        moveCJ(cj, map, rows, cols);

        // Peaje 1 (Los Santos -> San Fierro)
        if (cj.x == colMuro1 && cj.y == filaPuente && dineroCJ < peaje1) {
            cout << "¡No tienes suficiente dinero para cruzar a San Fierro!\n";
            Sleep(1000);
            // Mover a CJ atrás para que no cruce el puente
            cj.x--; // vuelve a la casilla anterior
        }


        // Peaje 2 (San Fierro -> Las Venturas)
        if (cj.x == colMuro2 && cj.y == filaPuente && dineroCJ < peaje1) {
            cout << "¡No tienes suficiente dinero para cruzar a San Fierro!\n";
            Sleep(1000);
            // Mover a CJ atrás para que no cruce el puente
            cj.x--; // vuelve a la casilla anterior
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
                // Hacemos que haya un minimo de dinero para que siempre de para pasar a la siguiente isla
                int dineroBase = (cj.x < colMuro1) ? peaje1 / numPeatones1 :
                    (cj.x < colMuro2) ? peaje2 / numPeatones2 : 1;
                dineroCJ += dineroBase + rand() % 3; // un pequeño extra aleatorio
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
