/*
============================================================
  Fichero: xvideo.c
  Creado: 20-03-2024
  Ultima Modificacion: divendres, 23 de maig de 2025, 11:39:23
  oSCAR jIMENEZ pUIG                                       
============================================================
*/

#include "xvideo.h"

X_Color X_BLACK=0;
X_Color X_WHITE=0;

unsigned int X_WINW=0;
unsigned int X_WINH=0;
static Display* display=NULL;
static Colormap colormap;
static Window window;
static Pixmap virtual;
static GC graphic;
static X_Color background;
static int min_key_code=0;
static int max_key_code=0;

int x_init(unsigned int w,unsigned int h) {
	X_WINW=w;
	X_WINH=h;
	int screenum=0;
	display=XOpenDisplay(0);
	if(display) {
		int screennum=XDefaultScreen(display);
		colormap=XDefaultColormap(display,screenum);
		X_BLACK=x_color(0,0,0);
		X_WHITE=x_color(1,1,1);
		window=XCreateSimpleWindow(display,RootWindow(display,screennum),0,0,X_WINW,X_WINH,0,0,0);
		XWindowAttributes xwa;
		XGetWindowAttributes(display,window,&xwa);
		virtual=XCreatePixmap(display,window,X_WINW,X_WINH,xwa.depth);
		static XGCValues gv;
		gv.line_width=1;
		gv.foreground=X_WHITE;
		gv.background=background=X_BLACK;
		graphic=XCreateGC(display,window,GCForeground|GCBackground|GCLineWidth,&gv);
		XSelectInput(display,window,StructureNotifyMask);
		XMapWindow(display,window);
		XEvent event;
		int mapped=0;
		while(XEventsQueued(display,QueuedAlready) || !mapped) {
			XNextEvent(display,&event);
			mapped=(!mapped)?(event.type==MapNotify):1;
		}
		XDisplayKeycodes(display,&min_key_code,&max_key_code);
		XSetForeground(display,graphic,gv.background);
		XFillRectangle(display,virtual,graphic,0,0,X_WINW,X_WINH);
		x_flush();
		return 1;
	}
	return 0;
}

void x_end() {
	XUnmapWindow(display,window);
	XDestroyWindow(display,window);
	XFreePixmap(display,virtual);
	XFreeColormap(display,colormap);
	XFreeGC(display,graphic);
	XEvent e;
	while(XEventsQueued(display,QueuedAlready)) {
			XNextEvent(display,&e);
	}
	XCloseDisplay(display);
}

X_Color x_color(double r,double g,double b) {
	const double FAC=65535;
	XColor xc;
	xc.flags=DoRed|DoGreen|DoBlue;
	xc.red=FAC*r;
	xc.green=FAC*g;
	xc.blue=FAC*b;
	XAllocColor(display,colormap,&xc);
	return xc.pixel;
}

void x_point(X_Point p,X_Color c) {
	XSetForeground(display,graphic,c);
	XDrawPoint(display,virtual,graphic,p.x,p.y);
}

void x_line(X_Point p,X_Point q,X_Color c) {
	XSetForeground(display,graphic,c);
	XDrawLine(display,virtual,graphic,p.x,p.y,q.x,q.y);
}

void x_square(X_Point b,unsigned int d,X_Color c) {
	XSetForeground(display,graphic,c);
	XFillRectangle(display,virtual,graphic,b.x,b.y,d,d);
}

void x_quadrilateral(X_Point* pos,X_Color col) {
	const unsigned char EDGES=4;
	XSetForeground(display,graphic,col);
	XFillPolygon(display,virtual,graphic,pos,EDGES,Convex,CoordModeOrigin);
}

void x_background(X_Color c) {
	background=c;
	x_clear();
}

void x_clear() {
	XSetForeground(display,graphic,background);
	XFillRectangle(display,virtual,graphic,0,0,X_WINW,X_WINH);
}

void x_flush() {
	XCopyArea(display,virtual,window,graphic,0,0,X_WINW,X_WINH,0,0);
	while(XPending(display)==0);
	XFlush(display);
}

int x_inkey(KeySym* keysym) {
	const int KCC=1;
	const unsigned char POS=0;
	int return_type=0;
	XEvent event;
	long event_mask=KeyPressMask|KeyReleaseMask;
	XSelectInput(display,window,event_mask);
	if(XPending(display)>0 && XCheckMaskEvent(display,event_mask,&event)) {
		int counter=0;
		XKeyEvent kevent=event.xkey;
		if(kevent.keycode>=min_key_code && kevent.keycode+KCC-1<=max_key_code) {
			KeySym* ks=XGetKeyboardMapping(display,kevent.keycode,KCC,&counter);
			if(POS<KCC*counter) {
				return_type=(kevent.type==KeyPress)?1:(kevent.type==KeyRelease)?-1:0;
				*keysym=*(ks+POS);
			}
			XFree(ks);
		}
	}
	return return_type;
}

char* x_keystr(KeySym keysym) {
	return XKeysymToString(keysym);
}

