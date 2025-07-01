/*
============================================================
  Fichero: xvideo.h
  Creado: 20-03-2024
  Ultima Modificacion: divendres, 23 de maig de 2025, 11:37:53
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#ifndef XVIDEO_H
#define XVIDEO_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef XPoint X_Point;
typedef long unsigned int X_Color;

extern unsigned int X_WINW,X_WINH;
extern X_Color X_BLACK,X_WHITE;

int x_init(unsigned int w,unsigned int h);
//inicio de x dando las dimensiones w,h de la pantalla

void x_end();
//libera el espacio de x

X_Color x_color(double red,double green,double blue);
//definicion de un color a partir de valores RGB entre [0,1].

void x_point(X_Point point,X_Color color);
//se coloca un punto en una posicoin de color determinado

void x_line(X_Point point_a,XPoint point_b,X_Color color);
//se hace linea desde a a b de color determinado

void x_square(X_Point base,unsigned int dimension,X_Color color);
//se hace un cuadrado a partir del punto base (extremo izquierdo superior de lado dimension) el cuadrado esta relleno

void x_quadrilateral(X_Point* points,X_Color color);
//se hace un quadrangulo con los puntos de la definicion con color determinado

void x_background(X_Color color);
//cambia el background

void x_clear();
//limpia la pantalla

void x_flush();
//hace flush sacando todo lo de la pantalla

int x_inkey(KeySym* keysym);
//lectura de las teclas, retorna 1 si hay pulsacion o -1 si se deja de pulsar, 0 si no hay accion

char* x_keystr(KeySym keysym);
//pasa una tecla a una cadena

#endif /* XVIDEO_H */
