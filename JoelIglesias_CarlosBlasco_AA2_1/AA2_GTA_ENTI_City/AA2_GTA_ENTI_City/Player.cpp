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

CJ::CJ(int startX, int startY)
    : x(startX), y(startY), symbol('v'), vida(100), ataque(50), cjEnCoche(false), dinero(0), cocheActual(nullptr) {}

bool CJ::estaVivo() const { return vida > 0; }
bool CJ::estaEnCoche() const { return cjEnCoche; }
Coche* CJ::getCocheActual() const { return cocheActual; }
void CJ::añadirDinero(int cantidad) { dinero += cantidad; }
void CJ::reducirVida(int danyo) { vida -= danyo; }


void CJ::setX(int val) { x = val; }
void CJ::setY(int val) { y = val; }
void CJ::setVida(int val) { vida = val; }
void CJ::setAtaque(int val) { ataque = val; }
void CJ::setSymbol(char s) { symbol = s; }

int CJ::getX() const { return x; }
int CJ::getY() const { return y; }
int CJ::getVida() const { return vida; }
int CJ::getAtaque() const { return ataque; }
int CJ::getDinero() const { return dinero; }
char CJ::getSymbol() const { return symbol; }



void CJ::move(char** map, int rows, int cols) {
    // Movimiento
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

    // Si CJ está en coche, mover también el coche
    if (cjEnCoche && cocheActual != nullptr) {
        cocheActual->x = x;
        cocheActual->y = y;
    }

    // Tecla E: Subir o bajar del coche
    if (GetAsyncKeyState(0x45) & 0x8000) {  // 'E'
        if (!cjEnCoche) {
            // Intentar subir a un coche cercano
            for (int i = 0; i < numCoches; ++i) {
                Coche& c = coches[i];
                int dx = abs(x - c.x);
                int dy = abs(y - c.y);

                if (!c.ocupado && dx <= 1 && dy <= 1 && map[c.y][c.x] != 'X') {
                    cjEnCoche = true;
                    c.ocupado = true;
                    cocheActual = &c;
                    x = c.x;
                    y = c.y;
                    symbol = 'O';
                    std::cout << "Has subido al coche.\n";
                    break;
                }
            }
        }
        else {
            // Intentar bajarse del coche
            if (cocheActual != nullptr && cocheActual->ocupado) {
                const int dx[] = { -1, 1, 0, 0 };
                const int dy[] = { 0, 0, -1, 1 };

                for (int d = 0; d < 4; ++d) {
                    int nx = x + dx[d];
                    int ny = y + dy[d];

                    if (map[ny][nx] == ' ') {
                        cjEnCoche = false;
                        cocheActual->ocupado = false;
                        cocheActual = nullptr;
                        x = nx;
                        y = ny;
                        symbol = 'v';
                        std::cout << "Te has bajado del coche.\n";
                        break;
                    }
                }
            }
        }

        Sleep(150);  // para evitar múltiples entradas por una sola pulsación
    }

    // === Atropellar peatones si va en coche ===
    if (cjEnCoche) {
        for (int i = 0; i < numPeatones; ++i) {
            Peaton& p = peatones[i];

            if (p.vivo && x == p.x && y == p.y) {
                if (p.x == bigSmoke.x && p.y == bigSmoke.y) continue;

                p.vivo = false;

                const int dx[] = { -1, 1, 0, 0 };
                const int dy[] = { 0, 0, -1, 1 };

                for (int d = 0; d < 4; ++d) {
                    int nx = x + dx[d];
                    int ny = y + dy[d];

                    if (map[ny][nx] == ' ') {
                        int cantidad = 0;
                        if (x < colMuro1)
                            cantidad = peaje1 / 6 + rand() % (maxD1 - peaje1 / 6 + 1);
                        else if (x < colMuro2)
                            cantidad = peaje2 / 10 + rand() % (maxD2 - peaje2 / 10 + 1);
                        else
                            cantidad = maxD3 / 2 + rand() % (maxD3 / 2 + 1);

                        añadirDinero(cantidad);
                        std::cout << "Has atropellat un peatón. Ha deixat $" << cantidad << " a (" << nx << "," << ny << ")\n";
                        break;
                    }
                }
            }
        }
    }

    // === Ataque cuerpo a cuerpo ===
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

                    if (p.x < colMuro1)
                        cantidad = peaje1 / 6 + rand() % (maxD1 - peaje1 / 6 + 1);
                    else if (p.x < colMuro2)
                        cantidad = peaje2 / 10 + rand() % (maxD2 - peaje2 / 10 + 1);
                    else
                        cantidad = maxD3 / 2 + rand() % (maxD3 / 2 + 1);

                    añadirDinero(cantidad);
                    añadirPeaton(crearPeaton(rows, cols, map));
                }
                break;
            }
        }

        // Big Smoke ataca si CJ está cerca
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

    for (int i = 0; i < numDinero; ++i) {
        if (!cjEnCoche && x == ::dinero[i].x && y == ::dinero[i].y) {
            this->dinero += ::dinero[i].cantidad;
            std::cout << "Has recollit $" << ::dinero[i].cantidad << ". Total: $" << this->dinero << "\n";
            eliminarDineroEn(i);
            break;
        }
    }



    // Ataques de peatones agresivos
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
            p.atacando = false;
        }
    }

    // Pago de peajes
    if (y == filaPuente) {
        if (x == colMuro1 && !peaje1Pagado) {
            if (dinero >= peaje1) {
                dinero -= peaje1;
                peaje1Pagado = true;
                map[y][x] = ' ';
                std::cout << "Peaje 1 pagado. ¡Puedes cruzar a San Fierro!\n";
                std::cin.get();
            }
            else {
                std::cout << "No tienes dinero para el peaje. Te detiene la policía.\n";
                exit(0);
            }
        }
        else if (x == colMuro2 && !peaje2Pagado) {
            if (dinero >= peaje2) {
                dinero -= peaje2;
                peaje2Pagado = true;
                map[y][x] = ' ';
                std::cout << "Peaje 2 pagado. ¡Puedes cruzar a Las Venturas!\n";
                std::cin.get();
            }
            else {
                std::cout << "No tienes dinero para el peaje. Te detiene la policía.\n";
                exit(0);
            }
        }
    }
}
