/*
============================================================
  Fichero: main.c
  Creado: 30-06-2025
  Ultima Modificacion: dimarts, 1 de juliol de 2025, 06:34:42
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#include <stdio.h>

#include "screen.h"

int main() {
	scr_init(400,400);
	X_Color a=x_color(0,0,0.5);
	X_Color b=x_color(0,0,0.75);
	X_Color c=X_WHITE;
	u1 np=0;
	key_set(1,XK_Escape);
	if(pal_new(&np,3)) {
		pal_ins(np,1,c);
		pal_ins(np,2,b);
		pal_ins(np,3,a);
		u1 sd[]={0,0,2,2,2,3,0,0,0,1,2,2,2,3,3,0,1,1,2,2,2,3,3,3,1,1,2,2,3,3,3,3,1,2,2,2,3,3,3,3,2,2,2,3,3,3,3,3,0,2,2,3,3,3,3,0,0,0,2,3,3,3,0,0};
		u1 ns=0;
		if(spr_new(&ns,8,8,sd)) {
			spr_ins(ns,np,HOR|VER,4,100,100);
			scr_show();
			while(!key_chk() || !key_isp(1));
		} else return 2;
	} else return 1;
	scr_end();
	return 0;
}
