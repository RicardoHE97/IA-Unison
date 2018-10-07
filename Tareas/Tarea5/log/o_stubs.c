// (c) Michael Buro 1992-2002, licensed under the GNU Public License, version 2

/*
 * o_stubs.c - Notify and event callback function stubs.
 * This file was generated by `gxv' from `o.G'.
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/types.h>
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#include <xview/xv_xrect.h>
#include <xview/notify.h>
#include <xview/notice.h>
#include <xview/font.h>
#include <group.h>
#include <X11/Xos.h>


#include "main.h"
#include "o_ui.h"
#include "sys.h"
#include "playgm.h"
#include "int.h"


#define BOARD_COLOUR    "DarkSeaGreen"

#define DX		45	/* Feldgr��e */
#define DY		45
#define RXSTEIN		19	/* Steingr��e */
#define RYSTEIN  	19

#define X0		-1	/* Ecke links oben des Spielfeldes */
#define Y0		-1

#define XM0		(DX/2)	/* Steinmittelpunkt */
#define YM0		(DY/2)

#define COLOR(color)\
  { if (color == C_BLACK) {\
    gc_val.foreground = col_BLACK;\
    gc_val.background = col_WHITE;\
  } else     {\
    gc_val.foreground = col_WHITE;\
    gc_val.background = col_BLACK;\
  }\
  gc_val.function = GXcopy;\
  XChangeGC(O_display, O_gc, GCForeground | GCBackground | GCFunction, &gc_val); }



void		XCB_SelectBrett();
void		XCB_WINDOW_EXIT();
void		XCB_MENU();

bool propwin=false, inputwin=false, ladenwin=false, speichernwin=false;


uint2 closed_bits[] = {

#include "icon.h"

};


/*
 * Global object definitions.
 */
o_window_objects		*O_window;
o_popEingabe_objects		*O_popEingabe;
o_popEinstellungen_objects	*O_popEinstellungen;
o_popLaden_objects		*O_popLaden;
o_popSpeichern_objects		*O_popSpeichern;



Display	*O_display;
XID	O_xid;
GC	O_gc;
Pixmap  O_stipple;

unsigned long col_BLACK, col_WHITE, col_board;
XGCValues gc_val;

struct itimerval timer01, timer10;


static char bitmap_bits[] = { 0xaa, 0x55 };



void gcm_initialize_colors(Xv_opaque obj, char *c1, char *c2) {}


void XBeep(void) { XBell(O_display, 0); }


Notify_value XCB_Timer(void)
{
  static int n = 0;


  CB_TIMER();

  if (n <= 0) { CB_TIMER_LONG(); n = 8; } else n--;

  return NOTIFY_DONE;
}

Notify_value sigint_func(Notify_client client, int sig, Notify_signal_mode when)
{
  _abort();
  return NOTIFY_DONE;
}



Attr_attribute	INSTANCE;



#ifdef MAIN

/*
 * Instance XV_KEY_DATA key.  An instance is a set of related
 * user interface objects.  A pointer to an object's instance
 * is stored under this key in every object.  This must be a
 * global variable.
 */

main(argc, argv)
	int	argc;
	char	**argv;
{
  GC gc;
	/*
	 * Initialize XView.
	 */
	xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
	INSTANCE = xv_unique_key();


	/*
	 * Initialize user interface components.
	 * Do NOT edit the object initializations by hand.
	 */
	O_window = o_window_objects_initialize(NULL, NULL);
	O_popEinstellungen = o_popEinstellungen_objects_initialize(NULL, O_window->window);
	O_popEingabe = o_popEingabe_objects_initialize(NULL, O_window->window);

	O_popLaden = o_popLaden_objects_initialize(NULL, NULL);
	O_popSpeichern = o_popSpeichern_objects_initialize(NULL, NULL);


	/*
	 * Turn control over to XView.
	 */
	xv_main_loop(O_window->window);
	exit(0);
}

#endif



void XInitInter(int *pargc, char **argv, PROPINFO *pi)
{
  GC gc;

	/*
	 * Initialize XView.
	 */
	xv_init(XV_INIT_ARGC_PTR_ARGV, pargc, argv, NULL);
	INSTANCE = xv_unique_key();
	
	/*
	 * Initialize user interface components.
	 * Do NOT edit the object initializations by hand.
	 */
	O_window = o_window_objects_initialize(NULL, NULL);
	O_popEinstellungen = o_popEinstellungen_objects_initialize(NULL, O_window->window);
	O_popEingabe = o_popEingabe_objects_initialize(NULL, O_window->window);
	O_popLaden = o_popLaden_objects_initialize(NULL, O_window->window);
	O_popSpeichern = o_popSpeichern_objects_initialize(NULL, O_window->window);


/*  xv_set(O_window->window, XV_X, pi->winx, XV_Y, pi->winy, NULL); */



  O_display = (Display *)xv_get(O_window->window, XV_DISPLAY);
  O_xid     = (XID)xv_get(canvas_paint_window(O_window->canBrett),
				XV_XID);
  O_gc	    =  DefaultGC(O_display, DefaultScreen(O_display));

  col_BLACK = BlackPixel(O_display, DefaultScreen(O_display)) ;
  col_WHITE = WhitePixel(O_display, DefaultScreen(O_display)) ;


/* Hintergrundmuster */

  COLOR(C_BLACK);
  O_stipple = XCreateBitmapFromData(O_display, O_xid, bitmap_bits, 8, 2);
  XSetStipple(O_display, O_gc, O_stipple);

  {
Colormap colormap;
XColor color,exact;

  colormap=XDefaultColormap(O_display, DefaultScreen(O_display));
  XAllocNamedColor(O_display,colormap,BOARD_COLOUR,&color,&exact);
  col_board = color.pixel;

  }

/* Timer Event f�r Uhren und Nachrichten �ber Filesystem */

  timer01.it_value.tv_usec = 
  timer01.it_interval.tv_usec = 100000;	/* 0.1 Sek */

  notify_set_itimer_func(
    O_window->window, XCB_Timer, ITIMER_REAL, &timer01, NULL
  );



/* Icon erzeugen */

  {
  Frame		frame;
  Icon  	icon;
  Server_image	closed_image;

  closed_image = (Server_image)xv_create(NULL, SERVER_IMAGE,
					 XV_WIDTH,	64,
					 XV_HEIGHT,	64,
					 SERVER_IMAGE_BITS, closed_bits,
					 NULL);
  icon = (Icon)xv_create(O_window->window, ICON,
			 ICON_IMAGE,	closed_image,
			 XV_X,		100,
			 XV_Y,		100,
			 NULL);

  xv_set(O_window->window, FRAME_ICON, icon, NULL);
  }


/* Ctrl-C & QUIT -> _abort() */

  notify_set_signal_func(O_window->window, sigint_func, SIGINT,  NOTIFY_SYNC);
  notify_set_signal_func(O_window->window, sigint_func, SIGQUIT, NOTIFY_SYNC);
  notify_set_signal_func(O_window->window, sigint_func, SIGHUP,  NOTIFY_SYNC);
}




void XFreeInter(void) {}


void XNotifier(void)
{
/*
 * Turn control over to XView.
 */

  xv_main_loop(O_window->window);
}




/****************** Hauptfenster ***********************/


/*
 * User-defined action for `window'.
 */
void
XCB_WINDOW_EXIT(win, event, arg, type)
	Xv_window	win;
	Event		*event;
	Notify_arg	arg;
	Notify_event_type type;
{
	o_window_objects *ip = (o_window_objects *) xv_get(win, XV_KEY_DATA, INSTANCE);
	
  CB_ENDE();
}

/*
 * Menu handler for `menu1 (Ende)'.
 */
Menu_item
XCB_ENDE(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_ENDE();
		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


void XDestroy(void)
{
  xv_destroy(O_window->window);
}



/*
 * Done callback function for `popSpeichern'.
 */
void
XCB_SPEICHERN_DONE(frame)
	Frame		frame;
{
	xv_set(frame, XV_SHOW, false, NULL);
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_SPEICHERN_AUS();
}


/*
 * Notify callback function for `textLadenName'.
 */
Panel_setting
XCB_TEXT_LADEN(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popLaden_objects *ip = (o_popLaden_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	char *	value = (char *) xv_get(item, PANEL_VALUE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_SPIEL_LADEN((char *) xv_get(O_popLaden->textLadenName, PANEL_VALUE));

	return panel_text_notify(item, event);
}


/*
 * Notify callback function for `butSpielLaden'.
 */
void
XCB_SPIEL_LADEN(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popLaden_objects *ip = (o_popLaden_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */


  CB_SPIEL_LADEN((char *) xv_get(O_popLaden->textLadenName, PANEL_VALUE));
}


/*
 * Notify callback function for `textSpeichernName'.
 */
Panel_setting
XCB_TEXT_SPEICHERN(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popSpeichern_objects *ip = (o_popSpeichern_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	char *	value = (char *) xv_get(item, PANEL_VALUE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_SPIEL_SPEICHERN((char *) xv_get(O_popSpeichern->textSpeichernName, PANEL_VALUE));

  return panel_text_notify(item, event);
}



/*
 * Notify callback function for `butSpielSpeichern'.
 */
void
XCB_SPIEL_SPEICHERN(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popSpeichern_objects *ip = (o_popSpeichern_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_SPIEL_SPEICHERN((char *) xv_get(O_popSpeichern->textSpeichernName, PANEL_VALUE));

}


/*
 * Menu handler for `menu1 (Laden...)'.
 */
Menu_item
XCB_LADEN(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_LADEN_EIN();

		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Menu handler for `menu1 (Speichern...)'.
 */
Menu_item
XCB_SPEICHERN(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_SPEICHERN_EIN();

		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Done callback function for `popLaden'.
 */
void
XCB_LADEN_DONE(frame)
	Frame		frame;
{
	xv_set(frame, XV_SHOW, false, NULL);
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_LADEN_AUS();
}



/******************** Laden & Speichern ***********************/

/* Ladefenster �ffnen */

void XLadenEin(void)
{
  if (!ladenwin) {

    XGetProps(&propinfo);

    xv_set(O_popLaden->popLaden, 
  	   XV_X, propinfo.winx+propinfo.ladenwinx, 	
	   XV_Y, propinfo.winy+propinfo.ladenwiny, 
	   NULL);
    xv_set(O_popLaden->popLaden, 
	   FRAME_CMD_PUSHPIN_IN, true, NULL);
  }

  xv_set(O_popLaden->popLaden, XV_SHOW, true, NULL);

  ladenwin = true;
}


/* und wieder schlie�en */

void XLadenAus(void)
{
  if (ladenwin) {

    xv_set(O_popLaden->popLaden, 
		FRAME_CMD_PUSHPIN_IN, false, NULL);
    xv_set(O_popLaden->popLaden, XV_SHOW, false, NULL);
    ladenwin = false; 
  }
}


/* Speicherfenster �ffnen */

void XSpeichernEin(void)
{
  if (!speichernwin) {

    XGetProps(&propinfo);

    xv_set(O_popSpeichern->popSpeichern, 
  	   XV_X, propinfo.winx+propinfo.speichernwinx, 	
	   XV_Y, propinfo.winy+propinfo.speichernwiny, 
	   NULL);
    xv_set(O_popSpeichern->popSpeichern, 
	   FRAME_CMD_PUSHPIN_IN, true, NULL);
  }

  xv_set(O_popSpeichern->popSpeichern, XV_SHOW, true, NULL);

  speichernwin = true;
}


/* und wieder schlie�en */

void XSpeichernAus(void)
{
  if (speichernwin) {

    xv_set(O_popSpeichern->popSpeichern, 
		FRAME_CMD_PUSHPIN_IN, false, NULL);
    xv_set(O_popSpeichern->popSpeichern, XV_SHOW, false, NULL);
    speichernwin = false; 
  }
}

/*
 * Menu handler for `menu1 (Zur�ck bis Anfang)'.
 */
Menu_item
XCB_ZURUECK_ZUM(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_ZURUECK_ZUM();

		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Menu handler for `menu1 (Vor bis Ende)'.
 */
Menu_item
XCB_VOR_ZUM(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_VOR_ZUM();

		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Menu handler for `menu1 (Tabelle)'.
 */
Menu_item
XCB_Tabelle(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

  CB_TABELLE();
		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Menu handler for `menu1 (Einstellungen...)'.
 */
Menu_item
XCB_EinstellungenEin(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

CB_EINSTELLUNGEN_EIN();

		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}


/*
 * Menu handler for `menu1 (Eingabe...)'.
 */
Menu_item
XCB_EingabeEin(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	o_window_objects * ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
		break;

	case MENU_DISPLAY_DONE:
		break;

	case MENU_NOTIFY:
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

CB_EINGABE_EIN();
		break;

	case MENU_NOTIFY_DONE:
		break;
	}
	return item;
}



/*
 * Notify callback function for `butVor'.
 */
void
XCB_Vor(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_VOR();
}


/*
 * Notify callback function for `butZurueck'.
 */
void
XCB_Zurueck(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_ZURUECK();
}


/*
 * Notify callback function for `butNeu'.
 */
void
XCB_Neu(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_NEU();
}


/*
 * Event callback function for `canBrett'.
 */
Notify_value
o_window_canBrett_event_callback(win, event, arg, type)
	Xv_window	win;
	Event		*event;
	Notify_arg	arg;
	Notify_event_type type;
{
	o_window_objects *ip = (o_window_objects *) xv_get(xv_get(win, CANVAS_PAINT_CANVAS_WINDOW), XV_KEY_DATA, INSTANCE);
	

	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	if (event_action(event) == ACTION_SELECT)
	{
		XCB_SelectBrett(win, event, arg, type);
	}
	
	/* gxv_end_connections */

	return notify_next_event_func(win, (Notify_event) event, arg, type);
}







/*
 * Repaint callback function for `canBrett'.
 */
void
XCB_RepaintBrett(canvas, paint_window, rects)
	Canvas		canvas;
	Xv_window	paint_window;
	Rectlist	*rects;
{
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  XSpielbrett();
}




/*
 * User-defined action for `canBrett'.
 */
void
XCB_SelectBrett(win, event, arg, type)
	Xv_window	win;
	Event		*event;
	Notify_arg	arg;
	Notify_event_type type;
{
	o_window_objects *ip = (o_window_objects *) xv_get(xv_get(win, CANVAS_PAINT_CANVAS_WINDOW), XV_KEY_DATA, INSTANCE);
	

  if (event_is_down(event)) { 
    CB_ZUG((event->ie_locy / DY + 1)*10 + event->ie_locx / DX + 1);

  }	
}

/*
 * Notify callback function for `butEinstellungen'.
 */
void
XCB_Einstellungen(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip  = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
        
        
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

}

/*
 * Notify callback function for `butWeiter'.
 */
void
XCB_Weiter(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_WEITER();
}


/*
 * Notify callback function for `butStop'.
 */
void
XCB_Stop(item, event)
	Panel_item	item;
	Event		*event;
{
	o_window_objects *ip = (o_window_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_STOP();
}





/******************** Einstellungen ***********************/

/* Einstellungsfenster �ffnen */

void XEinstellungenEin(void)
{
  if (!propwin) {

    XGetProps(&propinfo);

    xv_set(O_popEinstellungen->popEinstellungen, 
  	   XV_X, propinfo.winx+propinfo.propwinx, 	
	   XV_Y, propinfo.winy+propinfo.propwiny, 
	   NULL);
    xv_set(O_popEinstellungen->popEinstellungen, 
	   FRAME_CMD_PUSHPIN_IN, true, NULL);
  }

  xv_set(O_popEinstellungen->popEinstellungen, XV_SHOW, true, NULL);

  propwin = true;

  XSetProps(&propinfo);
}


/* und wieder schlie�en */

void XEinstellungenAus(void)
{
  if (propwin) {

    XGetProps(&propinfo); 
    xv_set(O_popEinstellungen->popEinstellungen, 
		FRAME_CMD_PUSHPIN_IN, false, NULL);
    xv_set(O_popEinstellungen->popEinstellungen, XV_SHOW, false, NULL);
    propwin = false; 
  }
}



/*
 * Notify callback function for `butBenutzen'.
 */
void
XCB_EINST_BEN(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEinstellungen_objects *ip = (o_popEinstellungen_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_EINST_BEN();
}

/*
 * Notify callback function for `butSpeichern'.
 */
void
XCB_EINST_SPEI(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEinstellungen_objects *ip = (o_popEinstellungen_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

  CB_EINST_SPEI();
}

/*
 * Notify callback function for `butEinstellungenAbbrechen'.
 */
void
XCB_EINST_ABBR(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEinstellungen_objects *ip = (o_popEinstellungen_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

  CB_EINSTELLUNGEN_AUS();
}




/*
 * Notify callback function for `setMoegl'.
 */
void
XCB_Moegl(item, value, event)
	Panel_item	item;
	int		value;
	Event		*event;
{
	o_popEinstellungen_objects *ip = (o_popEinstellungen_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	short	i;
	
	for (i = 0; i < 1; i++) {
		if (value & 01)
		
		/* gxv_start_connections DO NOT EDIT THIS SECTION */

		/* gxv_end_connections */

		value >>= 1;
	}

  CB_MOEGL();
}


/*
 * Notify callback function for `butWechseln'.
 */
void
XCB_EWechseln(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEinstellungen_objects *ip = (o_popEinstellungen_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_WECHSELN();
}


/*
 * Done callback function for `popEinstellungen'.
 */
void
XCB_EinstellungenDone(frame)
	Frame		frame;
{
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_EINSTELLUNGEN_AUS();
}







/************************ Eingabefenster ******************************/



void XEingabeEin(void) {

  if (!inputwin) {

    XGetProps(&propinfo);

    xv_set(O_popEingabe->popEingabe, 
	   XV_X, propinfo.winx+propinfo.inputwinx, 	
	   XV_Y, propinfo.winy+propinfo.inputwiny, 
	   NULL);
    xv_set(O_popEingabe->popEingabe, FRAME_CMD_PUSHPIN_IN, true, NULL);
  }

  xv_set(O_popEingabe->popEingabe, XV_SHOW, true, NULL);

  inputwin = true;

  XSetProps(&propinfo);
}


/* und wieder schlie�en */

void XEingabeAus(void) 
{
printf("AUS %d\n", inputwin);

  if (inputwin) {

    XGetProps(&propinfo);    

    xv_set(O_popEingabe->popEingabe, FRAME_CMD_PUSHPIN_IN, false, NULL);
    xv_set(O_popEingabe->popEingabe, XV_SHOW, false, NULL);

    inputwin = false;

  }
}



/*
 * Notify callback function for `setAmZug'.
 */
void
XCB_AmZug(item, value, event)
	Panel_item	item;
	int		value;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

}

/*
 * Notify callback function for `setSetzen'.
 */
void
XCB_Setzmodus(item, value, event)
	Panel_item	item;
	int		value;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

}


/*
 * Notify callback function for `butDrehen'.
 */
void
XCB_Drehen(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_DREHEN();
}


/*
 * Notify callback function for `butSpiegeln'.
 */
void
XCB_Spiegeln(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_SPIEGELN();
}


/*
 * Notify callback function for `butFuellen'.
 */
void
XCB_Fuellen(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_FUELLEN();
}


/*
 * Notify callback function for `butGrund'.
 */
void
XCB_Grund(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_GRUND();
}


/*
 * Notify callback function for `butZugfolge'.
 */
void
XCB_Zugfolge(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

  CB_ZUGFOLGE();
}


/*
 * Notify callback function for `textfield1'.
 */
Panel_setting
XCB_Zugnummer(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	char *	value = (char *) xv_get(item, PANEL_VALUE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

	return panel_text_notify(item, event);
}


/*
 * Notify callback function for `butEingabeOk'.
 */
void
XCB_EingabeOk(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_EINGABE_OK();
}


/*
 * Notify callback function for `butEingabeAbbruch'.
 */
void
XCB_EingabeAbbruch(item, event)
	Panel_item	item;
	Event		*event;
{
	o_popEingabe_objects *ip = (o_popEingabe_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */
	/* gxv_end_connections */

  CB_EINGABE_AUS();
}


/*
 * Done callback function for `popEingabe'.
 */
void
XCB_EingabeDone(frame)
	Frame		frame;
{
	
	/* gxv_start_connections DO NOT EDIT THIS SECTION */

	/* gxv_end_connections */

  CB_EINGABE_AUS();
}




/*********************** Allgemein *************************************/


static void CenterString(char *s, int len, char *t)
{
  int a, i, j;

  if ((a=strlen(s)) > len) Error("String zu lang in CenterString");

  FOR (i, (len-a)/2) t[i] = ' ';

  FOR (j, a) t[i++] = s[j];

  for (; i < len; i++) t[i] = ' ';

  t[len] = 0;
}



#define BLABLA_LEN	25


void XmsgBlaBla(char *s)
{
  char t[BLABLA_LEN+1];
  int  i;

  CenterString(s, BLABLA_LEN, t);

  xv_set(O_window->msgBlaBla, PANEL_LABEL_STRING, t, 0) ;
}      


void XClearBlaBla(void) { XmsgBlaBla(""); }




void XmsgBW(void) {}

 
void XmsgSteine(int AnzB, int AnzW)
{
  char s[10];

  sprintf(s, "%2d", AnzB);
  xv_set(O_window->msgSteineB, PANEL_LABEL_STRING, s, 0) ;

  sprintf(s, "%2d", AnzW);
  xv_set(O_window->msgSteineW, PANEL_LABEL_STRING, s, 0) ;

  if (AnzB+AnzW == 1) 	sprintf(s, " 1 Stein ",  AnzB+AnzW);
  else 			sprintf(s, "%2d Steine", AnzB+AnzW);

  xv_set(O_window->msgSteine, PANEL_LABEL_STRING, s, 0) ;
}


#define ZEIT_LEN	9

void XmsgZeiten(int ZeitB, int ZeitW)
{
  char s[ZEIT_LEN+1], t[ZEIT_LEN+1];


  if (ZeitB < 0)  { 

    sprintf(s, "-%4d:%.2d",  -ZeitB / 60, -ZeitB % 60);

  } else {

    sprintf(s, " %4d:%.2d",  ZeitB / 60, ZeitB % 60);

  }

  CenterString(s, ZEIT_LEN, t);
  xv_set(O_window->msgZeitB, PANEL_LABEL_STRING, t, 0) ;



  if (ZeitW < 0)  { 

    sprintf(s, "-%4d:%.2d",  -ZeitW / 60, -ZeitW % 60);

  } else {

    sprintf(s, " %4d:%.2d",  ZeitW / 60, ZeitW % 60);

  }

  CenterString(s, ZEIT_LEN, t);
  xv_set(O_window->msgZeitW, PANEL_LABEL_STRING, t, 0) ;
}



void XmsgZug(SFPOS PosB, SFPOS PosW)
{
  char s[100];


  sKoorAus(s, PosB); 
  xv_set(O_window->msgZugB, PANEL_LABEL_STRING, s, 0) ;

  sKoorAus(s, PosW); 
  xv_set(O_window->msgZugW, PANEL_LABEL_STRING, s, 0) ;
}


void XSetZurueck	(bool f)  { 
  xv_set(O_window->butZurueck, PANEL_INACTIVE, !f, NULL); }

void XSetVor    	(bool f)  { 
  xv_set(O_window->butVor, PANEL_INACTIVE, !f, NULL); }

void XSetWeiter 	(bool f)  { 
  xv_set(O_window->butWeiter, PANEL_INACTIVE, !f, NULL); }

void XSetStop   	(bool f)  { 
  xv_set(O_window->butStop, PANEL_INACTIVE, !f, NULL); }

void XSetNeu    	(bool f)  { 
  xv_set(O_window->butNeu, PANEL_INACTIVE, !f, NULL); }

void XSetZurueckZum(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_ZURUECK_ZUM, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}

void XSetVorZum(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_VOR_ZUM, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}

void XSetLaden(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_LADEN, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}

void XSetSpeichern(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_SPEICHERN, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}

void XSetEingabe(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_EingabeEin, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}

void XSetEinstellungen(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_EinstellungenEin, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}


void XSetTabelle(bool f)  
{
  Menu_item item;
  
  item = (Menu_item) 
    xv_find(xv_get(O_window->butOptionen, PANEL_ITEM_MENU), MENUITEM,
            MENU_GEN_PROC, XCB_Tabelle, NULL);

  xv_set(item, MENU_INACTIVE, !f, NULL);
}


void XSetProps(PROPINFO *pi)
{
  if (propwin) {

    xv_set(O_popEinstellungen->setSpielerB, PANEL_VALUE, pi->SpielerB, NULL);
    xv_set(O_popEinstellungen->setSpielerW, PANEL_VALUE, pi->SpielerW, NULL);

    xv_set(O_popEinstellungen->textRestzeitB, PANEL_VALUE, pi->RestzeitB, NULL);
    xv_set(O_popEinstellungen->textRestzeitW, PANEL_VALUE, pi->RestzeitW, NULL);

    xv_set(O_popEinstellungen->setMoegl, PANEL_VALUE, pi->MoeglAnzeig, NULL);
    xv_set(O_popEinstellungen->setBeep,  PANEL_VALUE, pi->Beep,   NULL);
  }

  if (inputwin) {

    xv_set(O_popEingabe->setAmZug, PANEL_VALUE, pi->AmZug, NULL);
    xv_set(O_popEingabe->setSetzModus, PANEL_VALUE, pi->SetzModus, NULL);
  }
}



void XGetProps(PROPINFO *pi)
{
  if (propwin) {

    pi->SpielerB = xv_get(O_popEinstellungen->setSpielerB, PANEL_VALUE);
    pi->SpielerW = xv_get(O_popEinstellungen->setSpielerW, PANEL_VALUE);

    pi->RestzeitB = xv_get(O_popEinstellungen->textRestzeitB, PANEL_VALUE);
    pi->RestzeitW = xv_get(O_popEinstellungen->textRestzeitW, PANEL_VALUE);

    pi->MoeglAnzeig = xv_get(O_popEinstellungen->setMoegl, PANEL_VALUE);
    pi->Beep        = xv_get(O_popEinstellungen->setBeep,  PANEL_VALUE);
  }

  if (inputwin) {

    pi->AmZug = xv_get(O_popEingabe->setAmZug, PANEL_VALUE);
    pi->SetzModus = xv_get(O_popEingabe->setSetzModus, PANEL_VALUE);
  }


/* Fensterkoordinaten holen */


  pi->winx = xv_get(O_window->window, XV_X);
  pi->winy = xv_get(O_window->window, XV_Y);


  if (propwin) { 

    pi->propwinx = xv_get(O_popEinstellungen->popEinstellungen, XV_X);
    pi->propwiny = xv_get(O_popEinstellungen->popEinstellungen, XV_Y);
  }

  if (inputwin) { 

    pi->inputwinx = xv_get(O_popEingabe->popEingabe, XV_X);
    pi->inputwiny = xv_get(O_popEingabe->popEingabe, XV_Y);
  }
}




static void DrawLine(int x0, int y0, int x1, int y1, int col)
{
  XSetFillStyle(O_display, O_gc, FillSolid);

  COLOR(col); 
  XDrawLine(O_display, O_xid, O_gc, x0, y0, x1, y1);
}


void XLeer(SFPOS Pos)
{
  int x, y;


  x = X0 + (Pos % 10 - 1) * DX; 
  y = Y0 + (Pos / 10 - 1) * DY;

#if 0

  COLOR(C_BLACK);
  XSetFillStyle(O_display, O_gc, FillOpaqueStippled); 

#else

  gc_val.foreground = col_board;
  gc_val.background = col_BLACK;
  gc_val.function = GXcopy;
  XChangeGC(O_display, O_gc, GCForeground | GCBackground | GCFunction, &gc_val);

#endif

  XFillRectangle(O_display, O_xid, O_gc, x+1, y+1, DX-1, DY-1);

}



void XStein(SFPOS Pos, int col)
{
  int x, y;


  XLeer(Pos);

  XSetFillStyle(O_display, O_gc, FillSolid);



/* Mittelpunkt */

  x = X0 + XM0 + (Pos % 10 - 1) * DX; 
  y = Y0 + YM0 + (Pos / 10 - 1) * DY;


/* Stein */

  COLOR(col);
  XFillArc(O_display, O_xid, O_gc, 
		x-RXSTEIN+1, y-RYSTEIN+1, 2*RXSTEIN, 2*RYSTEIN, 0, 360*64);

/* Rand invers */

  COLOR(1-col);
  XDrawArc(O_display, O_xid, O_gc, 
		x-RXSTEIN+1, y-RYSTEIN+1, 2*RXSTEIN, 2*RYSTEIN, 0, 360*64);

}



void XX(SFPOS Pos, int Farbe)
{
  int x, y;


  x = X0 + XM0 + (Pos % 10 - 1) * DX; 
  y = Y0 + YM0 + (Pos / 10 - 1) * DY;

  DrawLine(x-RXSTEIN/4,   y-RYSTEIN/4, x+RXSTEIN/4,   y+RYSTEIN/4, Farbe);
  DrawLine(x-RXSTEIN/4-1, y-RYSTEIN/4, x+RXSTEIN/4-1, y+RYSTEIN/4, Farbe);
  DrawLine(x-RXSTEIN/4,   y+RYSTEIN/4, x+RXSTEIN/4,   y-RYSTEIN/4, Farbe);
  DrawLine(x-RXSTEIN/4-1, y+RYSTEIN/4, x+RXSTEIN/4-1, y-RYSTEIN/4, Farbe);
}



void XZahl(int Zahl, SFPOS Pos, int col)
{
  int		   x, y;
  char		   s[100];
  Xv_Font	   font;
  Font_string_dims dims;


  font = (Xv_Font) xv_get(O_window->window, XV_FONT);

  COLOR(col);

  x = X0 + XM0 + (Pos % 10 - 1) * DX; 
  y = Y0 + YM0 + (Pos / 10 - 1) * DY;

  sprintf(s, "%d", Zahl);

  xv_get(font, FONT_STRING_DIMS, s, &dims);

  XDrawString(O_display, O_xid, O_gc, 
	x-dims.width/2, y+dims.height/2, s, strlen(s));
}



void XFlushGfx(void) { XFlush(O_display); }



void XSfModus(bool Modus)
{
  int i, col;


  if (Modus) col = C_BLACK; else col = C_WHITE;

  for (i = 1; i < 8; i++) {
    DrawLine(X0, Y0+i*DY, X0+8*DX, Y0+i*DY, col);
    DrawLine(X0+i*DX, Y0, X0+i*DX, Y0+8*DY, col);
  }

}


/* systemabh. Spielfeld zeichnen */

void XSpielbrett(void)
{
  int    i;
  char   s[100];
  SPFELD Sf;			/* Dummy */


/* Gitter */

  for (i = 1; i < 8; i++) {
    DrawLine(X0, Y0+i*DY, X0+8*DX, Y0+i*DY, C_BLACK);
    DrawLine(X0+i*DX, Y0, X0+i*DX, Y0+8*DY, C_BLACK);
  }



/* aktuelles Spielfeld erstellen */

  GfxSfAus(&Sf, BLACK, GFX_REFRESH);
}





void XDefProps(PROPINFO *pi)
{
  pi->winx      =  320; pi->winy      = 150;
  pi->propwinx  =  100;	pi->propwiny  = 100;
  pi->inputwinx =  100;	pi->inputwiny = 100;
  pi->ladenwinx =  100; pi->ladenwiny = 100;
  pi->speichernwinx = 100; pi->speichernwiny = 100;
  
}


void XPassenNotice(void)
{
    notice_prompt(O_window->window, NULL,
      NOTICE_MESSAGE_STRINGS, "Leider gibt es keine Zugm�glichkeit mehr!", NULL,
      NOTICE_NO_BEEPING, true,
      NOTICE_BUTTON_YES, "Passen",
      NULL);
}


bool XUeberschreibenNotice(void)
{
    return notice_prompt(O_window->window, NULL,
      NOTICE_MESSAGE_STRINGS, "Die Datei existiert schon!", NULL,
      NOTICE_NO_BEEPING, true,
      NOTICE_BUTTON_YES, "�berschreiben",
      NOTICE_BUTTON_NO,  "Abbrechen",
      NULL);
}


void XLesefehlerNotice(void)
{
   notice_prompt(O_window->window, NULL,
      NOTICE_MESSAGE_STRINGS, "Datei korrupt oder Lesefehler!", NULL,
      NOTICE_NO_BEEPING, true,
      NOTICE_BUTTON_YES, "Weiter",
      NULL);
}



void XSchreibfehlerNotice(void)
{
    notice_prompt(O_window->window, NULL,
      NOTICE_MESSAGE_STRINGS, "Schreibfehler!", NULL,
      NOTICE_NO_BEEPING, true,
      NOTICE_BUTTON_YES, "Weiter",
      NULL);
}


void XNichtDaNotice(void)
{
    notice_prompt(O_window->window, NULL,
      NOTICE_MESSAGE_STRINGS, "Die Datei gibt es nicht!", NULL,
      NOTICE_NO_BEEPING, true,
      NOTICE_BUTTON_YES, "Weiter",
      NULL);
}
