#ifndef GLOBALS_H
#define GLOBALS_H

#include "GameState.h"


struct Dinero {
    int x, y;
    int cantidad;
};

extern Dinero* dinero;
extern int numDinero;

extern bool peaje1Pagado;
extern bool peaje2Pagado;
extern int peaje1;
extern int peaje2;

// Variables para los peatones
extern int vidaP1;
extern int ataqueP1;
extern int vidaP2;
extern int ataqueP2;
extern int vidaP3;
extern int ataqueP3;

extern GameState currentState;

extern int maxD1;
extern int maxD2;
extern int maxD3;

void añadirDinero(int x, int y, int cantidad);
void eliminarDineroEn(int index);

// NO incloguis Walker.h aquí
struct Peaton;  
void añadirPeaton(Peaton nuevo);


#endif
