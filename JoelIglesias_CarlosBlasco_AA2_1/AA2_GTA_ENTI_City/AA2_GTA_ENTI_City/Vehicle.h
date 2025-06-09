#ifndef VEHICLE_H
#define VEHICLE_H

struct Coche {
    int x, y;
    bool ocupado;
};

Coche crearCoche(int rows, int cols, char** map);
void generarCoches(char** map, int rows, int cols, int cantidad);
void liberarCoches();

extern Coche* coches;
extern int numCoches;

#endif
