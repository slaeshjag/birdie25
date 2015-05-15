#include "main.h"
#include "game.h"
#include "object.h"
#include "player.h"

Game game;

static DARNIT_TILESHEET *sheet;
static DARNIT_TILEMAP *map;

struct ClientObject *obj;

void game_init() {
	int i;
	sheet = d_render_tilesheet_load("res/stars.png", 256, 256, DARNIT_PFORMAT_RGBA8);
	map = d_tilemap_new(56321458, sheet, 0xFFFF, 32, 32);
	for(i = 0; i < 32*32; i++) {
		map->data[i] = 1;
	}
	d_tilemap_recalc(map);
}

void game_render() {
	double x, y;
	int pl;
	pl = player_get();
	x = obj[pl].dx;
	y = obj[pl].dy;
	d_tilemap_camera_move(map, 128*32 + 20.0*x, 128*32 + 20.0*y);
	d_tilemap_draw(map);
	handle_camera();
	handle_player();
	object_draw();
	pre_simulation_draw();
	player_draw_hud();
}
