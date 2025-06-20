#include "Player.h"  // Primer CJ per evitar dependències cíclices
#include "Walker.h"
#include "Globals.h" // Si es fan servir variables globals
#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <iostream>



Peaton crearPeaton(int rows, int cols, char** map) {
    Peaton p;
    bool ocupado;
    int intentos = 0;
    do {
        p.x = rand() % (cols - 2) + 1;
        p.y = rand() % (rows - 2) + 1;

        ocupado = false;
        for (int i = 0; i < numPeatones; ++i) {
            if (peatones[i].vivo && peatones[i].x == p.x && peatones[i].y == p.y) {
                ocupado = true;
                break;
            }
        }

        intentos++;
        if (intentos > 1000) break;

    } while (map[p.y][p.x] != ' ' || ocupado);

    p.vertical = rand() % 2;
    p.vivo = true;
    return p;
}

void generarPeatonesPorIsla(char** map, int rows, int cols,
    int colMuro1, int colMuro2,
    int num1, int num2, int num3) {

    extern int vidaP1, ataqueP1, vidaP2, ataqueP2, vidaP3, ataqueP3;

    liberarPeatones(); // Eliminar anteriores
    numPeatones = num1 + num2 + num3;
    peatones = new Peaton[numPeatones];

    int contador = 0;

    auto generarPeatones = [&](int num, int xMin, int xMax, int isla, int vida, int poder) {
        for (int i = 0; i < num; ++i) {
            bool colocado = false;

            for (int intento = 0; intento < 1000; ++intento) {
                int x = rand() % (xMax - xMin - 1) + xMin + 1;
                int y = rand() % (rows - 2) + 1;

                if (map[y][x] == ' ') {
                    bool ocupado = false;
                    for (int j = 0; j < contador; ++j) {
                        if (peatones[j].vivo && peatones[j].x == x && peatones[j].y == y) {
                            ocupado = true;
                            break;
                        }
                    }

                    if (!ocupado) {
                        Peaton p;
                        p.x = x;
                        p.y = y;
                        p.vertical = rand() % 2 == 0;
                        p.vivo = true;
                        p.agresivo = rand() % 2 == 0;
                        p.vida = vida;
                        p.poder = poder;
                        p.atacando = false;
                        p.ultimoAtaque = 0;
                        peatones[contador] = p;

                        contador++;
                        colocado = true;
                        break;
                    }
                }
            }

            if (!colocado) {
                std::cout << " No se pudo colocar un peatón en Isla " << isla << " tras 1000 intentos.\n";
            }
        }
        };

    generarPeatones(num1, 0, colMuro1, 1, vidaP1, ataqueP1);
    generarPeatones(num2, colMuro1, colMuro2, 2, vidaP2, ataqueP2);
    generarPeatones(num3, colMuro2, cols - 1, 3, vidaP3, ataqueP3);
}

void moverPeatones(char** map, const CJ& cj, int rows, int cols) {
    for (int i = 0; i < numPeatones; ++i) {
        Peaton& p = peatones[i];
        if (!p.vivo) continue;

        int dx = abs(p.x - cj.getX());
        int dy = abs(p.y - cj.getY());
        if (dx <= 1 && dy <= 1) continue;

        int nx = p.x, ny = p.y;
        if (p.vertical) ny += (rand() % 2 ? 1 : -1);
        else nx += (rand() % 2 ? 1 : -1);

        bool libre = true;
        for (int j = 0; j < numPeatones; ++j) {
            if (j != i && peatones[j].vivo && peatones[j].x == nx && peatones[j].y == ny) {
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

void crearBigSmoke(char** map, int rows, int cols, int colMuro2) {
    for (int intento = 0; intento < 1000; ++intento) {
        int x = rand() % (cols - colMuro2 - 2) + colMuro2 + 1;
        int y = rand() % (rows - 2) + 1;

        if (map[y][x] == ' ') {
            bool ocupado = false;
            for (int i = 0; i < numPeatones; ++i) {
                if (peatones[i].vivo && peatones[i].x == x && peatones[i].y == y) {
                    ocupado = true;
                    break;
                }
            }

            if (!ocupado) {
                bigSmoke = { x, y, false, true, true, 200, 50, false, 0 };
                std::cout << "Big Smoke aparece en (" << x << "," << y << ")\n";
                return;
            }
        }
    }
    std::cout << "No se pudo colocar a Big Smoke.\n";
}

void liberarPeatones() {
    if (peatones != nullptr) {
        delete[] peatones;
        peatones = nullptr;
        numPeatones = 0;
    }
}
