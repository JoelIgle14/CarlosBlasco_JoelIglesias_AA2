#include "Map.h"
#include "Player.h"
#include "Walker.h"  
#include "Vehicle.h" 
#include "Globals.h"
#include <iostream>
using namespace std;

int colMuro1, colMuro2, filaPuente;
const int VIEW_WIDTH = 20;
const int VIEW_HEIGHT = 10;

char** createMap(int rows, int cols) {
    char** map = new char* [rows];
    for (int i = 0; i < rows; ++i) {
        map[i] = new char[cols];
        for (int j = 0; j < cols; ++j) {
            if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
                map[i][j] = 'X';
            else
                map[i][j] = ' ';
        }
    }

    colMuro1 = cols / 3;
    colMuro2 = 2 * cols / 3;
    filaPuente = rows / 2;

    for (int i = 1; i < rows - 1; ++i) {
        if (i == filaPuente) continue;
        if (colMuro1 > 0 && colMuro1 < cols - 1)
            map[i][colMuro1] = 'X';
        if (colMuro2 > 0 && colMuro2 < cols - 1)
            map[i][colMuro2] = 'X';
    }

    // Añadir peajes
    map[filaPuente][colMuro1] = 'T';
    map[filaPuente][colMuro2] = 'T';

    return map;
}

void drawView(char** map, int rows, int cols, CJ& cj) {
    system("cls");

    int startRow = cj.y - VIEW_HEIGHT / 2;
    int startCol = cj.x - VIEW_WIDTH / 2;

    if (startRow < 0) startRow = 0;
    if (startCol < 0) startCol = 0;
    if (startRow + VIEW_HEIGHT > rows) startRow = rows - VIEW_HEIGHT;
    if (startCol + VIEW_WIDTH > cols) startCol = cols - VIEW_WIDTH;

    for (int i = 0; i < VIEW_HEIGHT; ++i) {
        for (int j = 0; j < VIEW_WIDTH; ++j) {
            int mapRow = startRow + i;
            int mapCol = startCol + j;

            bool drawn = false;

            if (mapRow == cj.y && mapCol == cj.x) {
                cout << cj.symbol;
                drawn = true;
            }

            if (!drawn) {
                for (int i = 0; i < numPeatones; ++i) {
                    if (peatones[i].vivo && mapRow == peatones[i].y && mapCol == peatones[i].x) {
                        cout << 'P';
                        drawn = true;
                        break;
                    }
                }
            }

            /*

            PER A DEPURAR, PINTA ELS AGRESIUS AMB UNA 'A' PER DIFERENCIAR

            if (!drawn) {
                for (const auto& p : peatones) {
                    if (p.vivo && mapRow == p.y && mapCol == p.x) {
                        cout << (p.agresivo ? 'A' : 'P');
                        drawn = true;
                        break;
                    }
                }
            }

            */

            if (!drawn) {
                for (int i = 0; i < numDinero; ++i) {
                    if (mapRow == dinero[i].y && mapCol == dinero[i].x) {
                        cout << '$';
                        drawn = true;
                        break;
                    }
                }

            }

            if (!drawn) {
                for (int i = 0; i < numCoches; ++i) {
                    if (!coches[i].ocupado && mapRow == coches[i].y && mapCol == coches[i].x) {
                        cout << 'C';
                        drawn = true;
                        break;
                    }
                }

            }

            if (!drawn && bigSmoke.vivo && mapRow == bigSmoke.y && mapCol == bigSmoke.x) {
                cout << 'B';
                drawn = true;
            }


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
    cout << "Vida: " << cj.vida << "\n";
}

bool canMove(char** map, int newY, int newX, int rows, int cols) {
    return newY >= 0 && newY < rows &&
        newX >= 0 && newX < cols &&
        map[newY][newX] != 'X';
}

void deleteMap(char** map, int rows) {
    for (int i = 0; i < rows; ++i)
        delete[] map[i];
    delete[] map;
}