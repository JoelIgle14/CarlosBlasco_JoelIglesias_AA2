#include "Config.h"
#include "Globals.h"
#include <fstream>
#include <iostream>
using namespace std;

void readConfig(int& rows, int& cols,
    int& vidaCJ, int& ataqueCJ,
    int& peaje1, int& peaje2,
    int& numP1, int& maxD1, int& vidaP1, int& ataqueP1,
    int& numP2, int& maxD2, int& vidaP2, int& ataqueP2,
    int& numP3, int& maxD3, int& vidaP3, int& ataqueP3) {

    ifstream file("config.txt");
    if (!file.is_open()) {
        cerr << "No se pudo abrir config.txt\n";
        exit(1);
    }

    char sep;
    file >> cols >> sep >> rows;
    file >> vidaCJ >> sep >> ataqueCJ;
    file >> peaje1 >> sep >> peaje2;
    file >> numP1 >> sep >> maxD1 >> sep >> vidaP1 >> sep >> ataqueP1;
    file >> numP2 >> sep >> maxD2 >> sep >> vidaP2 >> sep >> ataqueP2;
    file >> numP3 >> sep >> maxD3 >> sep >> vidaP3 >> sep >> ataqueP3;

    file.close();
}