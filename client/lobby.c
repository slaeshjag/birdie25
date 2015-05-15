#include <darnit/darnit.h>
#include "ui/ui.h"
#include "main.h"
#include "lobby.h"
#include <protocol.h>
#include <network.h>

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

void lobby_network_handler() {
	UI_PROPERTY_VALUE v;
	Packet pack;
	char name[256];
	unsigned long ip;
	int i;
	
	if(!network_poll())
		return;
	ip = network_recv(&pack, sizeof(Packet));
	if(pack.type != PACKET_TYPE_LOBBY)
		return;
	if(pack.lobby.begin == 3) {
		v = lobby.list->get_prop(lobby.list, UI_LISTBOX_PROP_SIZE);
		for(i = 0; i < v.i; i++) {
			if(strtoul(ui_listbox_get(lobby.list, i), NULL, 10) == ip)
				return;
		}
		sprintf(name, "%lu: %s", ip, pack.lobby.name);
		ui_listbox_add(lobby.list, name);
	}
}
