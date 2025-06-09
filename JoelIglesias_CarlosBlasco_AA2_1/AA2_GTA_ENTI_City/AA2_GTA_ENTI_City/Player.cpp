
// Player.cpp
#include "Player.h"
#include "Map.h"
#include "Walker.h"
#include "Globals.h"
#include "Vehicle.h"
#include "GameState.h"
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

bool cjEnCoche = false;
int dineroCJ = 0;

void CJ::move(char** map, int rows, int cols) {
    if (GetAsyncKeyState(VK_UP) & 0x8000) {
        if (canMove(map, y - 1, x, rows, cols)) {
            y--;
            if (!cjEnCoche) symbol = '^';
        }
    }
    else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
        if (canMove(map, y + 1, x, rows, cols)) {
            y++;
            if (!cjEnCoche) symbol = 'v';
        }
    }
    else if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        if (canMove(map, y, x - 1, rows, cols)) {
            x--;
            if (!cjEnCoche) symbol = '<';
        }
    }
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        if (canMove(map, y, x + 1, rows, cols)) {
            x++;
            if (!cjEnCoche) symbol = '>';
        }
    }

    // Si CJ va en cotxe i atropella un peatón
    if (cjEnCoche) {
        for (int i = 0; i < numPeatones; ++i) {
            Peaton& p = peatones[i];

            if (p.vivo && x == p.x && y == p.y) {
                // Evitar atropellar Big Smoke
                if (p.x == bigSmoke.x && p.y == bigSmoke.y) continue;

                p.vivo = false;

                // Generar diners en una casella lliure adjacent
                const int dx[] = { -1, 1, 0, 0 };
                const int dy[] = { 0, 0, -1, 1 };

                for (int d = 0; d < 4; ++d) {
                    int nx = x + dx[d];
                    int ny = y + dy[d];

                    if (map[ny][nx] == ' ') {
                        int cantidad = 0;
                        if (x < colMuro1) {
                            int minPeaje = peaje1 / 6;
                            cantidad = minPeaje + rand() % (maxD1 - minPeaje + 1);
                        }
                        else if (x < colMuro2) {
                            int minPeaje = peaje2 / 10;
                            cantidad = minPeaje + rand() % (maxD2 - minPeaje + 1);
                        }
                        else {
                            cantidad = maxD3 / 2 + rand() % (maxD3 / 2 + 1);
                        }

                        añadirDinero(x, y, cantidad);

                        std::cout << "Has atropellat un peatón. Ha deixat $" << cantidad << " a (" << nx << "," << ny << ")\n";
                        break;
                    }
                }
            }
        }
    }


    // Atacar amb espai
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        for (int i = 0; i < numPeatones; ++i) {
            Peaton& p = peatones[i];

            if (p.vivo && abs(x - p.x) <= 1 && abs(y - p.y) <= 1) {
                if (p.agresivo && !p.atacando) {
                    p.atacando = true;
                    p.ultimoAtaque = clock();
                    std::cout << "Has provocado a un peatón agresivo!\n";
                    break;
                }

                p.vida -= 50;
                std::cout << "Atacando a peatón. Vida restante: " << p.vida << "\n";

                if (p.vida <= 0) {
                    p.vivo = false;
                    int cantidad = 0;

                    if (p.x < colMuro1) { // Los Santos
                        int minPeaje = peaje1 / 6; // mínim per peató
                        cantidad = minPeaje + rand() % (maxD1 - minPeaje + 1);
                    }
                    else if (p.x < colMuro2) { // San Fierro
                        int minPeaje = peaje2 / 10;
                        cantidad = minPeaje + rand() % (maxD2 - minPeaje + 1);
                    }
                    else { // Las Venturas
                        cantidad = maxD3 / 2 + rand() % (maxD3 / 2 + 1);
                    }

                    añadirDinero(x, y, cantidad);


                    añadirPeaton(crearPeaton(rows, cols, map));
                }
                break;
            }
        }

        // Big Smoke ataca si estàs a prop
        if (bigSmoke.vivo) {
            int dx = abs(bigSmoke.x - x);
            int dy = abs(bigSmoke.y - y);
            if (dx <= 1 && dy <= 1) {
                clock_t ahora = clock();
                double segundos = double(ahora - bigSmoke.ultimoAtaque) / CLOCKS_PER_SEC;
                if (segundos >= 1.0) {
                    vida -= bigSmoke.poder;
                    bigSmoke.ultimoAtaque = ahora;

                    std::cout << "Big Smoke te ha atacado! Vida CJ: " << vida << "\n";

                    if (vida <= 0) {
                        std::cout << "Big Smoke te ha derrotado. Game Over.\n";
                        currentState = GAMEOVER;
                    }
                }
            }
        }


        // Atacar a Big Smoke
        if (bigSmoke.vivo && abs(x - bigSmoke.x) <= 1 && abs(y - bigSmoke.y) <= 1) {
            bigSmoke.vida -= ataque;
            std::cout << "Has atacado a Big Smoke! Vida restante: " << bigSmoke.vida << "\n";

            if (bigSmoke.vida <= 0) {
                bigSmoke.vivo = false;
                bigSmokeDerrotado = true;
                std::cout << "Has derrotado a Big Smoke! Felicidades, has completado el juego.\n";
                currentState = VICTORY;
            }
        }

    }

    // Tecla E per pujar/baixar del cotxe
    if (GetAsyncKeyState(0x45) & 0x8000) {
        if (!cjEnCoche) {
            for (int i = 0; i < numCoches; ++i) {
                Coche& c = coches[i];

                if (!c.ocupado && x == c.x && y == c.y && map[y][x] != 'X') {
                    cjEnCoche = true;
                    c.ocupado = true;
                    symbol = 'O';
                    break;
                }
            }
        }
        else {
            for (int i = 0; i < numCoches; ++i) {
                Coche& c = coches[i];

                if (c.ocupado && x == c.x && y == c.y) {
                    c.ocupado = false;
                    cjEnCoche = false;
                    symbol = 'v';
                    break;
                }
            }
        }
    }

    // Recollir diners si no estem al cotxe
    for (int i = 0; i < numDinero; ++i) {
        if (!cjEnCoche && x == dinero[i].x && y == dinero[i].y) {
            dineroCJ += dinero[i].cantidad;
            std::cout << "Has recollit $" << dinero[i].cantidad << ". Total: $" << dineroCJ << "\n";
            eliminarDineroEn(i);

            break;
        }

    }

    // Comprobar ataques de peatones agresivos
    for (int i = 0; i < numPeatones; ++i) {
        Peaton& p = peatones[i];

        if (!p.vivo || !p.agresivo || !p.atacando) continue;

        int dx = abs(p.x - x);
        int dy = abs(p.y - y);
        if (dx <= 1 && dy <= 1) {
            clock_t ahora = clock();
            double segundos = double(ahora - p.ultimoAtaque) / CLOCKS_PER_SEC;
            if (segundos >= 1.0) {
                vida -= p.poder;
                p.ultimoAtaque = ahora;
                if (vida <= 0) {
                    std::cout << "CJ ha muerto. Game Over.\n";
                    currentState = GAMEOVER;
                }

                std::cout << "Peatón agresivo te ataca. Vida CJ: " << vida << "\n";

            }
        }
        else {
            p.atacando = false; // fuera de rango de pelea
        }
    }

    // Comprobar si está cruzando un peaje
    if (y == filaPuente) {
        if (x == colMuro1) {
            if (!peaje1Pagado) {
                if (dineroCJ >= peaje1) {
                    dineroCJ -= peaje1;
                    peaje1Pagado = true;
                    std::cout << "Peaje 1 pagado. ¡Puedes cruzar a San Fierro!, Pulse una tecla para seguir\n";
                    map[y][x] = ' '; // Eliminar la 'T'
                    std::cin.get(); // Pausa para ver el resultado

                }
                else {
                    std::cout << "No tienes dinero para el peaje a San Fierro. Te detiene la policía.\n";
                    exit(0); // Finalizar juego
                }
            }
        }
        else if (x == colMuro2) {
            if (!peaje2Pagado) {
                if (dineroCJ >= peaje2) {
                    dineroCJ -= peaje2;
                    peaje2Pagado = true;
                    std::cout << "Peaje 2 pagado. ¡Puedes cruzar a Las Venturas! Pulse una tecla para seguir\n";
                    map[y][x] = ' '; // Eliminar la 'T'
                    std::cin.get(); // Pausa para ver el resultado

                }
                else {
                    std::cout << "No tienes dinero para el peaje a Las Venturas. Te detiene la policía.\n";
                    exit(0); // Finalizar juego
                }
            }
        }
    }

}

