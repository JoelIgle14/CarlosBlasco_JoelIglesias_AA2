#ifndef WALKER_H
#define WALKER_H

#include "Player.h"  
#include <ctime> 

struct Peaton {
    int x, y;
    bool vertical;
    bool vivo;
    bool agresivo;
    int vida;
    int poder;
    bool atacando;
    clock_t ultimoAtaque;
};


Peaton crearPeaton(int rows, int cols, char** map);

void generarPeatonesPorIsla(char** map, int rows, int cols,
    int colMuro1, int colMuro2,
    int num1, int num2, int num3);

void moverPeatones(char** map, const CJ& cj, int rows, int cols);

void crearBigSmoke(char** map, int rows, int cols, int colMuro2);

void liberarPeatones();

extern Peaton* peatones;
extern int numPeatones;

extern Peaton bigSmoke;
extern bool bigSmokeDerrotado;

#endif
