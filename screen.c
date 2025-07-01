/*
============================================================
  Fichero: screen.c
  Creado: 30-06-2025
  Ultima Modificacion: dimarts, 1 de juliol de 2025, 06:33:24
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#include "screen.h"

#define PALSIZ 9
#define KEYSIZ 8

typedef signed char s1;

typedef struct {
	u1 colors;
	X_Color* color;
} Palette;

typedef struct {
	u1 w,h;
	u1* data;
} Sprite;

typedef struct {
	u1 sprite;
	u1 palette;
	u2 x,y;
} Graphic;

typedef KeySym Key[KEYSIZ];

static u2 scrw=0;
static u2 scrh=0;

static X_Color background;

static Palette* palette=NULL;
static u1 palettes=0;

static Sprite* sprite=NULL;
static u1 sprites=0;

static Key key;
static u1 keypress=0;

int scr_init(u2 w,u2 h) {
	if(x_init(w,h)) {
		background=X_BLACK;
		scrw=w;
		scrh=h;
		return 1;
	}
	return 0;
}

void scr_end() {
	for(u1 k=0;k<palettes;k++) {
		free(palette[k].color);
	}
	free(palette);
	palettes=0;
	for(u1 k=0;k<sprites;k++) {
		free(sprite[k].data);
	}
	free(sprite);
	sprites=0;
	x_end();
}

void scr_background(X_Color c) {
	background=c;
	x_background(background);
}

void scr_show() {
	x_flush();
}

u1 pal_new(u1* p,u1 cs) {
	void* ptr=realloc(palette,sizeof(Palette)*(palettes+1));
	if(ptr) {
		palette=(Palette*)ptr;
		*p=palettes;
		Palette* act=palette+palettes;
		cs=(cs<PALSIZ)?cs:PALSIZ;
		if((act->color=malloc(sizeof(X_Color)*cs))) {
			++palettes;
			act->colors=cs;
			return 1;
		}
	}
	return 0;
}

u1 pal_ins(u1 p,u1 id,X_Color c) {
	if(p<palettes) {
		Palette* pp=palette+p;
		if(id>0 && id<=pp->colors) {
			pp->color[id-1]=c;
			return 1;
		}
	}
	return 0;
}

u1 spr_new(u1* s,u1 w,u1 h,u1* d) {
	void* ptr=realloc(sprite,sizeof(Sprite)*(sprites+1));
	if(ptr && d) {
		sprite=(Sprite*) ptr;
		*s=sprites;
		Sprite* act=sprite + sprites;
		if((act->data=malloc(sizeof(u1)*w*h))) {
			++sprites;
			act->w=w;
			act->h=h;
			for(u2 k=0;k<w*h;k++) {
				act->data[k]=d[k];
			}
			return 1;
		}
	}
	return 0;
}

u1 spr_ins(u1 s,u1 p,Flip f,u1 r,u2 x,u2 y) {
	if(s<sprites && p<palettes && x<scrw && y<scrh && r>0) {
		u1* ptr=sprite[s].data;
		u1 sprw=sprite[s].w;
		u1 sprh=sprite[s].h;
		u2 i,j;
		i=j=0;
		s1 di,dj;
		di=dj=1;
		u2 ei,ej;
		ei=sprw;
		ej=sprh;
		if(f & VER) {
			j=sprh-1;
			dj=-1;
			ej=-1;
		}
		if(f & HOR) {
			i=sprw-1;
			di=-1;
			ei=-1;
		}
		for(u2 cj=j;cj!=ej;cj+=dj) {
			for(u2 ci=i;ci!=ei;ci+=di) {
				u1 v=*ptr++;
				if(v>0) {
					X_Color col=palette[p].color[v-1];
					if(r==1) {
						X_Point p={x+ci,y+cj};
						x_point(p,col);
					} else {
						X_Point p={x+ci*r,y+cj*r};
						x_square(p,r,col);
					}
				}
			}
		}
		return 1;
	}
	return 0;
}

u1 key_set(u1 flag,KeySym ks) {
	u1 flg=1;
	for(u1 k=0;k<8;k++) {
		if((flag & (~flg))==0) {
			key[k]=ks;
			return 1;
		}
	}
	return 0;
}

s1 key_chk() {
	KeySym ks;
	s1 stat=0;
	if((stat=x_inkey(&ks))) {
		KeySym* ptr=key;
		while(ptr!=key+KEYSIZ) {
			if(*ptr!=0 && *ptr==ks) {
				u1 nf=1<<(ptr-key);
				if(stat==1) keypress|=nf;
				else keypress&=(~nf);
			}
			ptr++;
		}
	}
	return stat;
}
				
u1 key_isp(u1 flag) {
	return ((flag & keypress)==flag)?1:0;
}

void key_unp(u1 flag) {
	keypress &=(~flag);
}

void scr_pau(double t) {
	clock_t end=clock()+t*CLOCKS_PER_SEC;
	while(clock()<end);
}

int scr_rdn(int a,int b) {
	static u1 init=0;
	if(!init) {
		srand(time(NULL));
		init=1;
	}
	int max=(a>b)?a:b;
	int min=(a>b)?b:a;
	int dif=max-min+1;
	if(dif!=0) return min+(rand()%dif);
	else return 0;
}
		
			
	
