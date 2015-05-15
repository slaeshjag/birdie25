#include "main.h"
#include "game.h"
#include "object.h"
#include "player.h"

Game game;

void game_init() {
	
}

void game_render() {
	handle_camera();
	handle_player();
	object_draw();
	player_draw_hud();
}
