#include <darnit/darnit.h>
#include <math.h>
#include <protocol.h>
#include <network.h>
#include "player.h"
#include "object.h"

extern long int sip;

static struct {
	int		x;
	int		y;
	int		focus_object;
} camera;

static DARNIT_TILE *powermeter;
static DARNIT_TILESHEET *powerts;
static double power;

void camera_init(int focus_object) {
	camera.focus_object = focus_object;
}


void load_player_stuff_once() {
	powerts = d_render_tilesheet_load("res/powermeter.png", 256, 32, DARNIT_PFORMAT_RGBA8);
	powermeter = d_render_tile_new(2, powerts);
}


void handle_camera() {
	int x, y, w, h;
	DARNIT_PLATFORM plat;

	plat = d_platform_get();
	object_get_coord(camera.focus_object, &x, &y, &w, &h);
	camera.x = x + (w / 2) - (plat.screen_w / 2);
	camera.y = y + (h / 2) - (plat.screen_h / 2);

	d_render_offset(camera.x, camera.y);
}


void handle_player() {
	Packet p;
	int mx, my;
	DARNIT_MOUSE di;
	DARNIT_PLATFORM plat;
	double angle;

	plat = d_platform_get();
	di = d_mouse_get();

	// TODO: Improve direction vector calculation, assumes that player is in the center
	mx = di.x - (plat.screen_w / 2);
	my = di.y - (plat.screen_h / 2);

	if (mx == 0)
		angle = 0;
	else
		angle = ((double) my) / ((double) mx);
	angle = atan(angle);
	if (mx < 0) {
		angle = -1*angle + M_PI;
	} else {
		angle = -1*angle;
		if (angle < 0)
			angle = 2. * M_PI + angle;
	}

	p.type = PACKET_TYPE_CLIENT;
	p.client.angle = angle;
	p.client.button.beam = d_keys_get().l;
	p.client.button.forward = d_keys_get().y;
	p.client.button.backward = d_keys_get().x;
	p.client.button.shoot = d_keys_get().r;

	network_send(sip, &p, sizeof(Packet));
/*
	if (mx > 0)
		angle = (M_PI * 2.) - angle;*/
	fprintf(stderr, "Angle: %lf\n", angle);
}


int player_get() {
	return camera.focus_object;
}


int player_draw_hud() {
	power = 0.5;
	d_render_offset(0,0);
	d_render_tile_set(powermeter, 0, 0);
	d_render_tile_set(powermeter, 1, 1);
	d_render_tile_move(powermeter, 0, 0, 0);
	d_render_tile_move(powermeter, 1, power * 256, 0);
	d_render_tile_size_set(powermeter, 0, power * 256, 32);
	d_render_tile_size_set(powermeter, 1, 256 - power * 256, 32);
	d_render_tile_tilesheet_coord_set(powermeter, 0, 0, 0, power * 256, 32);
	d_render_tile_tilesheet_coord_set(powermeter, 1, power * 256, 32, 256 - power * 256, 32);
	d_render_tile_draw(powermeter, 2);
}
