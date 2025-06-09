#include "Vehicle.h"
#include "Walker.h"
#include "Player.h"
#include "Globals.h"
#include <cstdlib>

Coche* coches = nullptr;
int numCoches = 0;

Coche crearCoche(int rows, int cols, char** map) {
    Coche c;
    bool ocupado;
    do {
        c.x = rand() % (cols - 2) + 1;
        c.y = rand() % (rows - 2) + 1;

        ocupado = false;
        for (int i = 0; i < numPeatones; ++i)
            if (peatones[i].x == c.x && peatones[i].y == c.y)
                ocupado = true;

        for (int i = 0; i < numDinero; ++i)
            if (dinero[i].x == c.x && dinero[i].y == c.y)
                ocupado = true;

    } while (map[c.y][c.x] != ' ' || ocupado);

    c.ocupado = false;
    return c;
}

void generarCoches(char** map, int rows, int cols, int cantidad) {
    if (coches != nullptr) {
        delete[] coches;
    }

    numCoches = cantidad;
    coches = new Coche[numCoches];

    for (int i = 0; i < numCoches; ++i) {
        coches[i] = crearCoche(rows, cols, map);
    }
}

void liberarCoches() {
    if (coches != nullptr) {
        delete[] coches;
        coches = nullptr;
        numCoches = 0;
    }
}
