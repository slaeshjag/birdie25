#ifndef __MENU_H__
#define __MENU_H__

#include "ui/ui.h"

typedef struct Menu Menu;
struct Menu {
	struct UI_PANE_LIST pane;
	UI_WIDGET *vbox;
	struct {
		UI_WIDGET *title;
		UI_WIDGET *author;
	} label;
	struct {
		UI_WIDGET *host;
		UI_WIDGET *join;
		UI_WIDGET *quit;
	} button;
};

typedef struct MenuHelp MenuHelp;
struct MenuHelp {
	struct UI_PANE_LIST pane;
	UI_WIDGET *vbox;
	struct {
		UI_WIDGET *title;
		UI_WIDGET *help;
		UI_WIDGET *web;
	} label;
};

typedef struct SelectName SelectName;
struct SelectName{
	struct UI_PANE_LIST pane;
	UI_WIDGET *vbox;
	UI_WIDGET *hbox;
	UI_WIDGET *label;
	UI_WIDGET *entry;
	struct {
		UI_WIDGET *ok;
		UI_WIDGET *quit;
	} button;
	
};

extern Menu menu;
extern SelectName select_name;

void menu_init();
void menu_render();

#endif
