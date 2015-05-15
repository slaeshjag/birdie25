#include <darnit/darnit.h>
#include "ui/ui.h"
#include "main.h"
#include "gameover.h"
#include <protocol.h>
#include <network.h>
#include <stdio.h>
#include <libgen.h>

GameOver game_over;

static void button_callback(UI_WIDGET *widget, unsigned int type, UI_EVENT *e) {
	if(widget == game_over.button.restart) {
		restart_to_menu(player_name);
	} else if(widget == game_over.button.quit) {
		d_quit();
	}
}

void game_over_init() {
	game_over.pane.pane = ui_pane_create(DISPLAY_WIDTH/2 - 200, DISPLAY_HEIGHT/2 - 200, 400, 400, game_over.vbox = ui_widget_create_vbox());
	game_over.pane.next = NULL;

	ui_vbox_add_child(game_over.vbox, game_over.label = ui_widget_create_label(gfx.font.large, "Game over!"), 0);
	ui_vbox_add_child(game_over.vbox, ui_widget_create_label(gfx.font.small, "This round's score:"), 0);
	ui_vbox_add_child(game_over.vbox, game_over.list = ui_widget_create_listbox(gfx.font.small), 1);
	
	game_over.hbox = ui_widget_create_hbox();
	ui_vbox_add_child(game_over.hbox, game_over.button.quit = ui_widget_create_button_text(gfx.font.small, "Quit"), 0);
	ui_vbox_add_child(game_over.hbox, game_over.button.restart = ui_widget_create_button_text(gfx.font.small, "Main menu"), 0);
	ui_vbox_add_child(game_over.vbox, game_over.hbox, 0);
	
	game_over.button.quit->event_handler->add(game_over.button.quit, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
	game_over.button.restart->event_handler->add(game_over.button.restart, button_callback, UI_EVENT_TYPE_UI_WIDGET_ACTIVATE);
}
