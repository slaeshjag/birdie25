#include <darnit/darnit.h>
#include "ui/ui.h"
#include "main.h"
#include "gameroom.h"
#include <protocol.h>

GameRoom gameroom;

static void button_callback(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget == gameroom.button.back) {
		game_state(GAME_STATE_LOBBY);
	} else if(widget == gameroom.button.start) {
		
	}
}

void gameroom_init() {
	gameroom.pane.pane = ui_pane_create(DISPLAY_WIDTH/2 - 200, DISPLAY_HEIGHT/2 - 300, 400, 600, gameroom.vbox = ui_widget_create_vbox());
	gameroom.pane.next = NULL;

	ui_vbox_add_child(gameroom.vbox, gameroom.label = ui_widget_create_label(gfx.font.large, "Players in game"), 0);
	ui_vbox_add_child(gameroom.vbox, gameroom.list = ui_widget_create_listbox(gfx.font.small), 1);
	
	gameroom.hbox = ui_widget_create_hbox();
	ui_vbox_add_child(gameroom.hbox, gameroom.button.back = ui_widget_create_button_text(gfx.font.small, "Back"), 0);
	ui_vbox_add_child(gameroom.hbox, gameroom.button.start = ui_widget_create_button_text(gfx.font.small, "Start game"), 0);
	ui_vbox_add_child(gameroom.vbox, gameroom.hbox, 0);
	
	gameroom.button.back->event_handler->add(gameroom.button.back, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	gameroom.button.start->event_handler->add(gameroom.button.start, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
}
