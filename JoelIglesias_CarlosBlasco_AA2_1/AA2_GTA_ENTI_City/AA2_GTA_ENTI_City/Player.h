#ifndef PLAYER_H
#define PLAYER_H

#include "Vehicle.h"
#include "Globals.h"
#include "Walker.h"

class CJ {
private:
    int x, y;
    char symbol;
    int vida;
    int ataque;
    bool cjEnCoche;
    int dinero;
    Coche* cocheActual;

public:
    CJ(int startX, int startY);

    void move(char** map, int rows, int cols);

    // Getters i Setters
    int getX() const;
    int getY() const;
    int getVida() const;
    int getAtaque() const;
    int getDinero() const;
    char getSymbol() const;


    void setX(int val);
    void setY(int val);
    void setVida(int val);
    void setAtaque(int val);
    void setSymbol(char s);

    void añadirDinero(int cantidad);
    void reducirVida(int danyo);
    bool estaVivo() const;
    bool estaEnCoche() const;
    Coche* getCocheActual() const;



};

#endif
