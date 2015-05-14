#include <darnit/darnit.h>
#include "ui/ui.h"
#include "main.h"
#include "lobby.h"
#include <protocol.h>

Lobby lobby;

static void button_callback(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget == lobby.button.back) {
		game_state(GAME_STATE_MENU);
	} else if(widget == lobby.button.join) {
		
	}
}

void lobby_init() {
	lobby.pane.pane = ui_pane_create(DISPLAY_WIDTH/2 - 200, DISPLAY_HEIGHT/2 - 300, 400, 600, lobby.vbox = ui_widget_create_vbox());
	lobby.pane.next = NULL;

	ui_vbox_add_child(lobby.vbox, lobby.label = ui_widget_create_label(gfx.font.large, "Join game"), 0);
	ui_vbox_add_child(lobby.vbox, lobby.list = ui_widget_create_listbox(gfx.font.small), 1);
	
	lobby.hbox = ui_widget_create_hbox();
	ui_vbox_add_child(lobby.hbox, lobby.button.back = ui_widget_create_button_text(gfx.font.small, "Back"), 0);
	ui_vbox_add_child(lobby.hbox, lobby.button.join = ui_widget_create_button_text(gfx.font.small, "Join"), 0);
	ui_vbox_add_child(lobby.vbox, lobby.hbox, 0);
	
	lobby.button.back->event_handler->add(lobby.button.back, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	lobby.button.join->event_handler->add(lobby.button.join, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
}
