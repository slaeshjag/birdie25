#include <darnit/darnit.h>
#include <math.h>
#include <protocol.h>
#include <network.h>
#include "player.h"
#include "object.h"
#include "main.h"
#include <common_math.h>

struct {
	int		x;
	int		y;
	int		focus_object;
	int		home;
} camera;


static DARNIT_TILE *powermeter;
static DARNIT_TILE *velocitymeter;
static DARNIT_TILE *thrustmeter;
static DARNIT_TILESHEET *powerts;
double power;
double thrust;
double velocity;

extern struct ClientObject *obj;
extern double coordinate_scale;
bool display_tractor_beam = false;
extern DARNIT_TEXT_SURFACE *time_text;

struct {
	DARNIT_CIRCLE *ring;
	DARNIT_LINE *north;
	DARNIT_LINE *south;
	int x;
	int y;
} static compass;
static DARNIT_TEXT_SURFACE *trip;

void camera_init(int focus_object, int home) {
	camera.focus_object = focus_object;
	camera.home = home;
}


struct {
	char			*name;
	int			id;
} static playerlist[8];


void load_player_stuff_once() {
	powerts = d_render_tilesheet_load("res/powermeter.png", 256, 32, DARNIT_PFORMAT_RGBA8);
	powermeter = d_render_tile_new(2, powerts);
	velocitymeter = d_render_tile_new(2, powerts);
	thrustmeter = d_render_tile_new(2, powerts);
	compass.ring = d_render_circle_new(30, 2);
	compass.x = DISPLAY_WIDTH - 76;
	compass.y = 60;
	d_render_circle_move(compass.ring, compass.x, compass.y, 50);
	compass.north = d_render_line_new(2, 1);
	compass.south = d_render_line_new(2, 1);
	d_render_line_move(compass.north, 0, 0, 0, 0, 0);
	d_render_line_move(compass.north, 1, 0, 0, 0, 0);
	d_render_line_move(compass.south, 0, 0, 0, 0, 0);
	d_render_line_move(compass.south, 1, 0, 0, 0, 0);
	trip = d_text_surface_new(gfx.font.small, 256, 256, DISPLAY_WIDTH - 142, 128);
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
	p.client.button.shoot = d_mouse_get().lmb;
	display_tractor_beam = p.client.button.beam;

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
	char text[32];

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
	
	d_render_tile_set(velocitymeter, 0, 2);
	d_render_tile_set(velocitymeter, 1, 3);
	d_render_tile_move(velocitymeter, 0, 0, 40);
	d_render_tile_move(velocitymeter, 1, velocity * 256, 40);
	d_render_tile_size_set(velocitymeter, 0, velocity * 256, 32);
	d_render_tile_size_set(velocitymeter, 1, 256 - velocity * 256, 32);
	d_render_tile_tilesheet_coord_set(velocitymeter, 0, 0, 64, velocity * 256, 32);
	d_render_tile_tilesheet_coord_set(velocitymeter, 1, velocity * 256, 96, 256 - velocity * 256, 32);
	d_render_tile_draw(velocitymeter, 2);
	
	d_render_tile_set(thrustmeter, 0, 4);
	d_render_tile_set(thrustmeter, 1, 5);
	d_render_tile_move(thrustmeter, 0, 0, 80);
	d_render_tile_move(thrustmeter, 1, thrust * 256, 80);
	d_render_tile_size_set(thrustmeter, 0, thrust * 256, 32);
	d_render_tile_size_set(thrustmeter, 1, 256 - thrust * 256, 32);
	d_render_tile_tilesheet_coord_set(thrustmeter, 0, 0, 128, thrust * 256, 32);
	d_render_tile_tilesheet_coord_set(thrustmeter, 1, thrust * 256, 160, 256 - thrust * 256, 32);
	d_render_tile_draw(thrustmeter, 2);
	
	d_render_circle_draw(compass.ring);
	
	int pl;
	double x, y, angle;
	pl = player_get();
	x = -obj[pl].dx - obj[camera.home].dx;
	y = -obj[pl].dy - obj[camera.home].dy;
	angle = math_delta_to_angle(x, y);
		
	x = 30.0*cos(angle);
	y = 30.0*sin(angle);
	d_render_line_move(compass.north, 0, compass.x, compass.y, compass.x + x, compass.y + y);
	d_render_line_move(compass.south, 0, compass.x, compass.y, compass.x - x, compass.y - y);
	d_render_tint(255, 0, 0, 255);
	d_render_line_draw(compass.north, 1);
	d_render_tint(255, 255, 255, 255);
	d_render_line_draw(compass.south, 1);

	d_text_surface_reset(trip);
	sprintf(text, "Distance to\nhome sun: %i", (int) (DIST(obj[camera.home], obj[pl]) * 200.0));
	d_text_surface_string_append(trip, text);
	d_text_surface_draw(trip);

	d_text_surface_draw(time_text);
	
	return 1;
}
