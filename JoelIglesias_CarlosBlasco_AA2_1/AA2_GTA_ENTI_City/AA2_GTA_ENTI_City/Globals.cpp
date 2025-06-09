#include "Globals.h"
#include "GameState.h"
#include "Walker.h"

Dinero* dinero = nullptr;
int numDinero = 0;

bool peaje1Pagado = false;
bool peaje2Pagado = false;
int peaje1 = 0;
int peaje2 = 0;

// Variables de configuració de peatons agressius
int vidaP1;
int ataqueP1;
int vidaP2;
int ataqueP2;
int vidaP3;
int ataqueP3;

GameState currentState = INIT;

int maxD1;
int maxD2;
int maxD3;

// Definicions globals de peatones
Peaton* peatones = nullptr;
int numPeatones = 0;

// Definicions globals de Big Smoke
Peaton bigSmoke;
bool bigSmokeDerrotado = false;

// Funcions de diners
void añadirDinero(int x, int y, int cantidad) {
    Dinero* nuevo = new Dinero[numDinero + 1];
    for (int i = 0; i < numDinero; ++i) {
        nuevo[i] = dinero[i];
    }
    nuevo[numDinero] = { x, y, cantidad };
    delete[] dinero;
    dinero = nuevo;
    numDinero++;
}

void eliminarDineroEn(int index) {
    if (index < 0 || index >= numDinero) return;
    Dinero* nuevo = new Dinero[numDinero - 1];
    int pos = 0;
    for (int i = 0; i < numDinero; ++i) {
        if (i != index) {
            nuevo[pos++] = dinero[i];
        }
    }
    delete[] dinero;
    dinero = nuevo;
    numDinero--;
}

// Funcions de peatones
void añadirPeaton(Peaton nuevo) {
    Peaton* nuevoArray = new Peaton[numPeatones + 1];
    for (int i = 0; i < numPeatones; ++i) {
        nuevoArray[i] = peatones[i];
    }
    nuevoArray[numPeatones] = nuevo;
    delete[] peatones;
    peatones = nuevoArray;
    numPeatones++;
}
