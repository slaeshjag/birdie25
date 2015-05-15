#ifndef __GAME_OVER_H__
#define __GAME_OVER_H__

#include "ui/ui.h"

typedef struct GameOver GameOver;
struct GameOver {
	struct UI_PANE_LIST pane;
	UI_WIDGET *label;
	UI_WIDGET *list;
	UI_WIDGET *vbox;
	UI_WIDGET *hbox;
	struct {
		UI_WIDGET *restart;
		UI_WIDGET *quit;
	} button;
};

extern GameOver game_over;
void game_over_init();
void game_over_render();

#endif
