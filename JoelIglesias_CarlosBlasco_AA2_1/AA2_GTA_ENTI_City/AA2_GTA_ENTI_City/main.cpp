#include "Map.h"
#include "Player.h"
#include "Walker.h"
#include "Vehicle.h"
#include "Config.h"
#include "Globals.h"
#include "GameState.h"
#include <windows.h>
#include <ctime>
#include <iostream>

using namespace std;

int main() {
    int rows, cols;
    int vidaCJ, ataqueCJ;
    int numP1;
    int numP2;
    int numP3;
    extern int vidaP1, ataqueP1;
    extern int vidaP2, ataqueP2;
    extern int vidaP3, ataqueP3;
    extern int maxD1;
    extern int maxD2;
    extern int maxD3;
    readConfig(rows, cols, vidaCJ, ataqueCJ,
        peaje1, peaje2,
        numP1, maxD1, vidaP1, ataqueP1,
        numP2, maxD2, vidaP2, ataqueP2,
        numP3, maxD3, vidaP3, ataqueP3);

    srand(time(0));
    char** map = createMap(rows, cols);

    CJ cj(cols / 4, rows / 2);
    //cj.setX(cols / 4);
    //cj.setY(rows / 2);
    cj.setSymbol('v');
    cj.setVida(vidaCJ);
    cj.setAtaque(ataqueCJ);


    generarPeatonesPorIsla(map, rows, cols, colMuro1, colMuro2, numP1, numP2, numP3);
    crearBigSmoke(map, rows, cols, colMuro2);
    generarCoches(map, rows, cols, 4);

    currentState = INIT;
    clock_t initStart = clock();

    int selectedOption = 0;

    while (true) {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

        switch (currentState) {
        case INIT:
            system("cls");
            cout << "========================\n";
            cout << "    GTA ENTI CITY\n";
            cout << "========================\n";
            cout << "Carrega inicial...\n";
            Sleep(3000);
            currentState = MENU;
            break;

        case MENU:
            system("cls");
            cout << "===== MENU PRINCIPAL =====\n";
            cout << "Selecciona una opcion moviendo con las flechas y seleccionando con espacio...\n";
            cout << (selectedOption == 0 ? "> " : "  ") << "Play\n";
            cout << (selectedOption == 1 ? "> " : "  ") << "Exit\n";

            if (GetAsyncKeyState(VK_UP) & 0x8000) {
                selectedOption = max(0, selectedOption - 1);
                Sleep(150);
            }
            else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
                selectedOption = min(1, selectedOption + 1);
                Sleep(150);
            }
            else if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
                if (selectedOption == 0) {
                    currentState = GAME;
                }
                else {
                    return 0;
                }
                Sleep(100);
            }
            break;

        case GAME:
            cj.move(map, rows, cols);
            moverPeatones(map, cj, rows, cols);
            drawView(map, rows, cols, cj);
            Sleep(100);
            break;

        case GAMEOVER:
            system("cls");
            cout << "====================\n";
            cout << "    GAME OVER\n";
            cout << "====================\n";
            Sleep(5000);
            return 0;

        case VICTORY:
            system("cls");
            cout << "====================\n";
            cout << "  HAS VENCIDO A\n";
            cout << "     BIG SMOKE!\n";
            cout << "====================\n";
            cout << "FELICITATS! HAS GUANYAT!\n";
            cout << "El joc es tancarà en breu...\n";
            Sleep(5000);
            return 0;

        }
    }

    deleteMap(map, rows);
    return 0;
}
