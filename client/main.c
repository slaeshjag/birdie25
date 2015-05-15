#include <darnit/darnit.h>
#include "object.h"
#include <protocol.h>
#include <network.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include "ui/ui.h"
#include "game.h"
#include "main.h"
#include "menu.h"
#include "lobby.h"
#include "gameroom.h"
#include "gameover.h"

unsigned long sip;
void *server_main(void *);
bool we_are_hosting_a_game;

Gfx gfx;
GameState gamestate;

char player_name[NAME_LEN_MAX];

void (*state_render[GAME_STATES])()={
	[GAME_STATE_MENU] = menu_render,
	[GAME_STATE_GAME] = game_render,
	[GAME_STATE_SELECT_NAME] = NULL,
};

void (*state_network_handler[GAME_STATES])()={
	[GAME_STATE_GAMEROOM] = gameroom_network_handler,
	[GAME_STATE_LOBBY] = lobby_network_handler,
};

struct UI_PANE_LIST *gamestate_pane[GAME_STATES];

extern int objs;

void restart_to_menu(const char *name) {
	// This is ugly :D
	char buf[4096];
	sprintf(buf, "%s", d_fs_exec_path());
	sprintf(buf, "%s", basename(buf));
	if(name)
		execl(d_fs_exec_path(), buf, name, NULL);
	else
		execl(d_fs_exec_path(), buf, NULL);
}


void game_state(GameState state) {
	int i;
	Packet pack;
	//Game state destructors
	switch(gamestate) {
		case GAME_STATE_GAME:
			/*ui_event_global_remove(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_remove(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_remove(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_remove(game_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_remove(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);*/
			pthread_cancel(game.thread);
			break;
		case GAME_STATE_MENU:
			//ui_event_global_remove(menu_buttons, UI_EVENT_TYPE_BUTTONS);
		case GAME_STATE_SELECT_NAME:
		case GAME_STATE_HOST:
		case GAME_STATE_LOBBY:
			break;
		case GAME_STATE_GAMEROOM:;
			break;
		GAME_STATE_GAME_OVER:
		case GAME_STATE_QUIT:
		
		case GAME_STATES:
			break;
	}
	//Game state constructors
	switch(state) {
		case GAME_STATE_GAME:
			/*ui_event_global_add(game_view_mouse_click, UI_EVENT_TYPE_MOUSE_PRESS);
			ui_event_global_add(game_view_mouse_move, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_mouse_draw, UI_EVENT_TYPE_MOUSE_ENTER);
			ui_event_global_add(game_view_buttons, UI_EVENT_TYPE_BUTTONS);
			ui_event_global_add(game_view_key_press, UI_EVENT_TYPE_KEYBOARD_PRESS);*/
			// NOTE: Don't remove this! It leaks RAM, but prevents segfault in OpenGL
			for (i = 0; i < objs; i++)
				object_init_object(i, 64);
			
			for(i = 0; i < 8; i++)
				object_init_object(0, 64 + i);
			
			object_init_object(0, 74);
			pthread_create(&game.thread, NULL, object_thread, NULL);
			#ifndef __DEBUG__
			//d_input_grab();
			#endif
			break;
		case GAME_STATE_MENU:
			//ui_event_global_add(menu_buttons, UI_EVENT_TYPE_BUTTONS);
		case GAME_STATE_SELECT_NAME:
			ui_selected_widget = select_name.entry;
			break;
		case GAME_STATE_LOBBY:
			gameroom.button.start->enabled = false;
			we_are_hosting_a_game = false;
			ui_listbox_clear(lobby.list);
			break;
		case GAME_STATE_HOST:
			we_are_hosting_a_game = true;
			server_start();
			pack.type = PACKET_TYPE_LOBBY;
			pack.lobby.begin = 1;
			printf("player name is %s\n", player_name);
			memcpy(pack.lobby.name, player_name, NAME_LEN_MAX);
			network_send(sip = network_local_ip(), &pack, sizeof(Packet));
			gameroom.button.start->enabled = true;
			state = GAME_STATE_GAMEROOM;
		case GAME_STATE_GAMEROOM:
			ui_listbox_clear(gameroom.list);
		case GAME_STATE_GAME_OVER:
		case GAME_STATE_QUIT:
			d_input_release();
		
		case GAME_STATES:
			break;
	}
	
	gamestate=state;
}

int main(int argc, char **argv) {
	int i;
	Packet pl, pl2;
	Packet ps;
	char font_path[4096];
	DARNIT_INPUT_MAP inputmap;
	
	d_init_custom("Jymdsjepp", DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, "birdie25", NULL);
	
	inputmap = d_keymapping_get();
	inputmap.l = TPW_KEY_SPACE;
	d_keymapping_set(inputmap);
	
	sprintf(font_path, "%s", d_fs_exec_path());
	sprintf(font_path, "%s/font.ttf", dirname(font_path));
	gfx.font.large = d_font_load(font_path, 48, 256, 256);
	gfx.font.small = d_font_load(font_path, 16, 256, 256);
	
	ui_init(4);
	menu_init();
	lobby_init();
	gameroom_init();
	game_init();
	game_over_init();
	
	gamestate_pane[GAME_STATE_MENU] = &menu.pane;
	gamestate_pane[GAME_STATE_SELECT_NAME] = &select_name.pane;
	gamestate_pane[GAME_STATE_LOBBY] = &lobby.pane;
	gamestate_pane[GAME_STATE_GAMEROOM] = &gameroom.pane;
	gamestate_pane[GAME_STATE_GAME_OVER] = &game_over.pane;
	
	signal(SIGINT, d_quit); //lol
	network_init(PORT);
	
	d_cursor_show(1);
	load_player_stuff_once();
	
	if(argc > 1) {
		snprintf(player_name, NAME_LEN_MAX, "%s", argv[1]);
		game_state(GAME_STATE_MENU);
	} else
		game_state(GAME_STATE_SELECT_NAME);
	while(gamestate!=GAME_STATE_QUIT) {
		//server_loop(d_last_frame_time());
		
		/*if(gamestate>=GAME_STATE_LOBBY)
			client_check_incoming();*/
		
		if(state_network_handler[gamestate])
			state_network_handler[gamestate]();
		
		d_render_begin();
		d_render_blend_enable();
		if(state_render[gamestate])
			state_render[gamestate]();
		
		if(gamestate_pane[gamestate])
			ui_events(gamestate_pane[gamestate], 1);
			
		d_render_end();
		d_loop();
	}

	d_quit();
	return 0;
}
