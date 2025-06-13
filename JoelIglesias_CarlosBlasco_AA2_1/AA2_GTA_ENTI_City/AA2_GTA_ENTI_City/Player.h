#ifndef PLAYER_H
#define PLAYER_H

#include "Globals.h"
#include "Vehicle.h"

class CJ {
public:
    int x, y;
    char symbol;
    int vida;
    int ataque;
    Coche* cocheActual = nullptr;

    void move(char** map, int rows, int cols);
};

extern bool cjEnCoche;
extern int dineroCJ;
extern Coche* cocheActual;


#endif
