/*
============================================================
  Fichero: screen.c
  Creado: 30-06-2025
  Ultima Modificacion: dijous, 3 de juliol de 2025, 05:09:45
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#include "screen.h"

#define PALSIZ 9
#define KEYSIZ 8
#define CHRSIZ 39

#define CW 8 //ancho de caracter
#define CH 8 //alto de un caracter
#define CD (CW*CH) //pixels totales de un caracter

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

static Sprite character[CHRSIZ];

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
	for(u1 k=0;k<CHRSIZ;k++) {
		free(character[k].data);
	}
	x_end();
}

static X_Color coltox(Color c) {
	const double DIV=255.0;
	double r=c.r;
	double g=c.g;
	double b=c.b;
	return x_color(r/DIV,g/DIV,b/DIV);
}

void scr_background(Color c) {
	background=coltox(c);
	x_background(background);
}

void scr_era(u2 x,u2 y,u2 w,u2 h) {
	X_Point p={x,y};
	if(w==1 && h==1) {
		x_point(p,background);
	} else if(w==1) {
		X_Point d={x,y+h-1};
		x_line(p,d,background);
	} else if(h==1) {
		X_Point d={x+w-1,y};
		x_line(p,d,background);
	} else if(w*h!=0) {
		X_Point b={x,y+h-1};
		X_Point c={x+w-1,y+h-1};
		X_Point d={x+w-1,y};
		X_Point ps[]={p,b,c,d};
		x_quadrilateral(ps,background);
	}
}

void scr_clr() {
	x_clear();
}

void scr_show() {
	x_flush();
}

Color scr_col(u1 r,u1 g,u1 b) {
	return (Color){r,g,b};
}

u1 pal_new(u1* p,u1 cs) {
	void* ptr=realloc(palette,sizeof(Palette)*(palettes+1));
	if(ptr) {
		palette=(Palette*)ptr;
		*p=palettes;
		Palette* act=palette+palettes;
		cs=(cs<PALSIZ)?cs:PALSIZ;
		act->color=NULL;
		if((act->color=malloc(sizeof(X_Color)*cs))) {
			++palettes;
			act->colors=cs;
			return 1;
		}
	}
	return 0;
}

u1 pal_ins(u1 p,u1 id,Color c) {
	if(p<palettes) {
		Palette* pp=palette+p;
		if(id>0 && id<=pp->colors) {
			pp->color[id-1]=coltox(c);
			return 1;
		}
	}
	return 0;
}

u1 dat_new(u1* val,u1 ls,char* d[]) {
	u1* pval=val;
	for(u1 k=0;k<ls;k++) {
		char* ptr=d[k];
		while(*ptr!='\0') {
			if(*pval>='0' && *pval<='9') *ptr=*pval-'0';
			else *ptr=0;
			ptr++;
		}
	}
	return pval-val;
}			

static u1 spr_data_new(Sprite* s,u1 w,u1 h,u1* d) {
	if((s->data=malloc(sizeof(u1)*w*h))) {
		s->w=w;
		s->h=h;
		for(u2 k=0;k<w*h;k++) s->data[k]=d[k];
		return 1;
	}
	return 0;
}

u1 spr_new(u1* s,u1 w,u1 h,u1* d) {
	void* ptr=realloc(sprite,sizeof(Sprite)*(sprites+1));
	if(ptr && d) {
		sprite=(Sprite*) ptr;
		*s=sprites;
		Sprite* act=sprite + sprites;
		act->data=NULL;
		if(spr_data_new(act,w,h,d)) {
			++sprites;
			return 1;
		}
	}
	return 0;
}

static u1 spr_data_ins(u1* data,u1 sprw,u1 sprh,u1 p,Flip f,u1 r,u2 x,u2 y) {
	u1* ptr=data;
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

u1 spr_ins(u1 s,u1 p,Flip f,u1 r,u2 x,u2 y) {
	if(s<sprites && p<palettes && x<scrw && y<scrh && r>0) return spr_data_ins(sprite[s].data,sprite[s].w,sprite[s].h,p,f,r,x,y);
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


				
static void chrdef(u1 code,u1* data) {
	u1 datcod[CD];
	u1* ptr=datcod;
	while(ptr!=datcod+CD) *ptr++=0;
	for(u1 r=1;r<7;r++) {
		u1 val=data[r-1];
		u1 flag=1;
		for(u1 c=0;c<CW;c++) {
			datcod[c+r*CW]=(val & flag)?1:0;
			flag=flag<<1;
		}
	}
	spr_data_new(character+code,CW,CH,datcod);
}

static void chrsdef() {
	u1 data0[]={30,57,57,63,57,57};
	chrdef(0,data0);
	u1 data1[]={31,35,31,35,35,31};
	chrdef(1,data1);
	u1 data2[]={30,51,3,3,51,30};
	chrdef(2,data2);
	u1 data3[]={31,57,57,57,63,31};
	chrdef(3,data3);
	u1 data4[]={63,3,31,3,3,63};
	chrdef(4,data4);
	u1 data5[]={63,3,3,31,3,3};
	chrdef(5,data5);
	u1 data6[]={30,51,3,59,51,62};
	chrdef(6,data6);
	u1 data7[]={49,49,63,49,49,49};
	chrdef(7,data7);
	u1 data8[]={30,12,12,12,12,30};
	chrdef(8,data8);
	u1 data9[]={60,24,24,27,27,14};
	chrdef(9,data9);
	u1 data10[]={51,27,15,15,27,51};
	chrdef(10,data10);
	u1 data11[]={3,3,3,3,3,63};
	chrdef(11,data11);
	u1 data12[]={49,59,63,53,49,49};
	chrdef(12,data12);
	u1 data13[]={49,51,55,61,57,49};
	chrdef(13,data13);
	u1 data14[]={30,51,51,51,51,30};
	chrdef(14,data14);
	u1 data15[]={31,51,51,31,3,3};
	chrdef(15,data15);
	u1 data16[]={30,35,35,43,19,46};
	chrdef(16,data16);
	u1 data17[]={31,51,51,31,11,51};
	chrdef(17,data17);
	u1 data18[]={30,3,30,24,57,30};
	chrdef(18,data18);
	u1 data19[]={63,12,12,12,12,12};
	chrdef(19,data19);
	u1 data20[]={49,49,49,49,57,30};
	chrdef(20,data20);
	u1 data21[]={49,49,49,49,26,12};
	chrdef(21,data21);
	u1 data22[]={49,49,53,63,59,49};
	chrdef(22,data22);
	u1 data23[]={49,26,12,14,19,33};
	chrdef(23,data23);
	u1 data24[]={51,51,30,12,12,12};
	chrdef(24,data24);
	u1 data25[]={63,56,28,14,7,63};
	chrdef(25,data25);
	u1 data26[]={30,51,51,51,51,30};
	chrdef(26,data26);
	u1 data27[]={12,14,12,12,12,30};
	chrdef(27,data27);
	u1 data28[]={30,57,56,30,7,63};
	chrdef(28,data28);
	u1 data29[]={31,56,30,56,56,31};
	chrdef(29,data29);
	u1 data30[]={30,27,25,25,63,24};
	chrdef(30,data30);
	u1 data31[]={31,3,31,56,57,30};
	chrdef(31,data31);
	u1 data32[]={30,3,31,51,51,30};
	chrdef(32,data32);
	u1 data33[]={63,48,24,12,14,14};
	chrdef(33,data33);
	u1 data34[]={30,57,30,57,57,30};
	chrdef(34,data34);
	u1 data35[]={30,57,57,62,56,30};
	chrdef(35,data35);
	u1 data36[]={0,0,0,0,12,12};
	chrdef(36,data36);
	u1 data37[]={0,12,12,0,12,12};
	chrdef(37,data37);
	u1 data38[]={0,0,0,0,0,0};
	chrdef(38,data38);
}

static u1 charins(char c,u1 p,Flip f,u1 r,u2 x,u2 y) {
	u1 dir=38;
	if(c>='A' && c<='Z') dir=c-'A';
	else if(c>='0' && c<='9') dir=c-'0'+ 26;
	else if(c=='.') dir=36;
	else if(c==':') dir=37;
	return spr_data_ins(character[dir].data,CW,CH,p,f,r,x,y);
}

u1 str_ins(char* s,u1 p,Flip f,u1 r,s1 dx,s1 dy,u2 x,u2 y) {
	static u1 chrsini=0;
	if(!chrsini) {
		chrsdef();
		chrsini=1;
	}
	u1 ret=1;
	char* ptr=s;
	while(*ptr!='\0') {
		ret &= charins(*ptr,p,f,r,x,y);
		x+=r*CW*dx;
		y+=r*CH*dy;
		ptr++;
	}
	return ret;
}

	
		
			
	
