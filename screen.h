/*
============================================================
  Fichero: screen.h
  Creado: 30-06-2025
  Ultima Modificacion: dijous, 3 de juliol de 2025, 04:46:03
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#include <stdlib.h>
#include <time.h>

#include "xvideo.h"

typedef unsigned char u1;
typedef unsigned short u2;
typedef signed char s1;

typedef enum {NON,VER,HOR} Flip;

typedef struct {
	u1 r,g,b;
} Color;

int scr_init(u2 w,u2 h);
//iniciamos el modo pantalla

void scr_end();
//liberamos pantalla

void scr_background(Color color);
//cambia el color del fondo

void scr_era(u2 x,u2 y,u2 w,u2 h);
//se borra una zona de la pantalla

vpid scr_clr();
//borrado de toda la pantalla

void scr_show();
//hace un flush para mostrar la pantalla

Color scr_col(u1 r,u1 g,u1 b);
//definimos un color

u1 pal_new(u1* palette,u1 colors);
//inicia una nueva paleta, de nueve colores como maximo, devuelve el identificador
//colors se refiere al numero de colores que tendra la paleta

u1 pal_ins(u1 palette,u1 id,Color color);
//se guarda un color en la paleta ya definida (id=1 to 9)

u1 spr_new(u1* sprite,u1 w,u1 h,u1* data);
//crea un sprite de una cierta dimension, devuelve el identificador

u1 spr_ins(u1 sprite,u1 palette,Flip flip,u1 ratio,u2 x,u2 y);
//pone un sprite en una posicion de la pantalla

u1 key_set(u1 flag,KeySym ks);
//asocia una tecla a un valor de flag (de 1 a 128), maximo 8.

s1 key_chk();
//lee el teclado y establece las banderas, devuelve 1 si hay algun press o release

u1 key_isp(u1 flag);
//devuelve si el flag se corresponde a las teclas pulsadas

void key_unp(u1 flag);
//desconecta una tecla pulsada

void scr_pau(double time);
//hacemos una pausa

int scr_rnd(int a,int b);
//numero aleatorio entre a y b, ambos incluidos

u1 str_ins(char* string,u1 palette,Flip flip,u1 ratio,s1 dx,s1 dy,u2 x,u2 y);
//insercion de una cadena
//	string: cadena
//	palette: paleta utilizada, el 1 es el fondo
//	flip: tipo del flip
//	ratio: ratio de las letras
//	x,y: posicion de la pantalla
//	dx,dy: avance de cada letra



