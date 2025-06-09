#pragma once
// Map.h
#ifndef MAP_H
#define MAP_H

class CJ;

char** createMap(int rows, int cols);
void drawView(char** map, int rows, int cols, CJ& cj);
bool canMove(char** map, int newY, int newX, int rows, int cols);
void deleteMap(char** map, int rows);

extern int colMuro1, colMuro2, filaPuente;

#endif


