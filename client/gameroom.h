#ifndef __GAMEROOM_H__
#define __GAMEROOM_H__
#include "ui/ui.h"
#include <pthread.h>

typedef struct GameRoom GameRoom;
struct GameRoom {
	struct UI_PANE_LIST pane;
	UI_WIDGET *label;
	UI_WIDGET *list;
	UI_WIDGET *vbox;
	UI_WIDGET *hbox;
	struct {
		UI_WIDGET *start;
		UI_WIDGET *back;
	} button;
};

extern GameRoom gameroom;
void gameroom_network_handler();
void gameroom_init();

#endif
