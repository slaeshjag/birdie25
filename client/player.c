#include <darnit/darnit.h>
#include <math.h>
#include <protocol.h>
#include <network.h>
#include "player.h"
#include "object.h"
#include "main.h"

struct {
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

//	fprintf(stderr, "Camera is now at %i %i\n", camera.x, camera.y);
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
//	fprintf(stderr, "Angle: %lf\n", angle);
}


int player_get() {
	return camera.focus_object;
}


void player_draw_nametag(const char *name, int color, int x, int y) {
	int font_w, font_h;

	DARNIT_TEXT_SURFACE *dt;
	DARNIT_LINE *dl;
	DARNIT_RECT *dr;

	d_render_offset(camera.x, camera.y);
	d_render_tint(0, 0, 0, 255);
	/* TODO: Look up string geometrics */
	font_h = d_font_string_geometrics(gfx.font.small, name, 1000, &font_w);

	dr = d_render_rect_new(1);
	dl = d_render_line_new(4, 1);

	d_render_rect_move(dr, 0, x, y, x + font_w + 8, y + font_h + 8);
	d_render_rect_draw(dr, 1);
	d_render_rect_free(dr);

	/* TODO: Look up player color */
	//d_render_tint();
	d_render_tint(255, 255, 255, 255);
	d_render_line_move(dl, 0, x, y, x + font_w + 8, y);
	d_render_line_move(dl, 1, x + font_w + 8, y, x + font_w + 8, y + font_h + 8);
	d_render_line_move(dl, 2, x + font_w + 8, y + font_h + 8, x, y + font_h + 8);
	d_render_line_move(dl, 3, x, y + font_h + 8, x, y);
	d_render_line_draw(dl, 4);
	d_render_line_free(dl);

	d_render_offset(camera.x, camera.y);
	/* TODO: Look up font */
	dt = d_text_surface_new(gfx.font.small, 20, 800, x+4, y+4);
	d_text_surface_string_append(dt, (char *) name);
	d_text_surface_draw(dt);
	d_text_surface_free(dt);
	d_render_offset(camera.x, camera.y);
	d_render_tint(255, 255, 255, 255);
	
	return;
}


void player_draw_icon_autoedge(int icon, int x, int y) {
	double midx, midy, dx, dy, delta, delta_for_x;
	if (camera.x - x > 0 && camera.x - x < d_platform_get().screen_w)
		if (camera.y - y > 0 && camera.y < d_platform_get().screen_h)
			// Icon is within the screen, don't draw it
			return;
	midx = (double) d_platform_get().screen_w / 2 + camera.x;
	midy = (double) d_platform_get().screen_h / 2 + camera.y;

	dx = midx - x;
	dy = midy - y;

	if (dx == 0) {
		if (dy < 0) {
			d_render_tile_blit(object_get_icons(), icon, x, camera.y);
		} else {
			d_render_tile_blit(object_get_icons(), icon, x, camera.y + d_platform_get().screen_h - 32);
		}
	} else if (dy == 0) {
		if (dx < 0) {
			d_render_tile_blit(object_get_icons(), icon, camera.x, y);
		} else {
			d_render_tile_blit(object_get_icons(), icon, camera.x + d_platform_get().screen_w - 32, y);
		}
	} else {
		delta = dx/dy;
		delta_for_x = ((double) d_platform_get().screen_w) / ((double) d_platform_get().screen_h);
		if (delta_for_x > delta)  { // Intersection will be on the Y-axis */
			double intersection;
			intersection = (double) d_platform_get().screen_w / delta;
			if (dx < 0) {
				intersection *= -((double) d_platform_get().screen_w) / 2.;
				d_render_tile_blit(object_get_icons(), icon, camera.x, intersection);
			} else {
				intersection *= ((double) d_platform_get().screen_w) / 2.;
				d_render_tile_blit(object_get_icons(), icon, camera.x + d_platform_get().screen_w - 32, intersection);
			}
		} else {	// Intersection on X-axis
			double intersection;
			intersection = (double) d_platform_get().screen_h / delta;
			if (dy < 0) {
				intersection *= -((double) d_platform_get().screen_h) / 2.;
				d_render_tile_blit(object_get_icons(), icon, intersection, camera.y);
			}
		}
			
			
			// TODO: Finish this function
			
	}
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

	return 1;
}


