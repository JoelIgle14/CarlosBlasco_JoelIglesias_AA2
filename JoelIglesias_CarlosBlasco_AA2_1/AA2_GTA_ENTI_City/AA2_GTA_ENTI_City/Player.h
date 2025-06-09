#ifndef PLAYER_H
#define PLAYER_H

#include "Globals.h"

class CJ {
public:
    int x, y;
    char symbol;
    int vida;
    int ataque;

    void move(char** map, int rows, int cols);
};

extern bool cjEnCoche;
extern int dineroCJ;

#endif
