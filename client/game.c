#include "main.h"
#include "game.h"
#include "object.h"
#include "player.h"

Game game;

void game_init() {
	
}

extern struct {
	int		x;
	int		y;
	int		focus_object;
} camera;

void game_render() {
	handle_camera();
	handle_player();
	object_draw();
	pre_simulation_draw();
	object_draw_world_border();
	player_draw_hud();
}
