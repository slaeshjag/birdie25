#ifndef __GAMEROOM_H__
#define __GAMEROOM_H__

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
void gameroom_init();

#endif
