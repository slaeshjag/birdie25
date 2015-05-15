#ifndef __LOBBY_H__
#define __LOBBY_H__

typedef struct Lobby Lobby;
struct Lobby {
	struct UI_PANE_LIST pane;
	UI_WIDGET *label;
	UI_WIDGET *list;
	UI_WIDGET *vbox;
	UI_WIDGET *hbox;
	struct {
		UI_WIDGET *join;
		UI_WIDGET *back;
	} button;
};

extern Lobby lobby;
void lobby_init();
void lobby_network_handler();

#endif
