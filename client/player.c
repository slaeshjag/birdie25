#include <darnit/darnit.h>
#include <math.h>
#include <protocol.h>
#include <network.h>
#include "player.h"

extern long int sip;

static struct {
	int		x;
	int		y;
	int		focus_object;
} camera;


void camera_init(int focus_object) {
	camera.focus_object = focus_object;
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
	p.client.button.beam = 0;
	p.client.button.forward = 0;
	p.client.button.backward = 0;
	p.client.button.shoot = 0;

	network_send(sip, &p, sizeof(Packet));
/*
	if (mx > 0)
		angle = (M_PI * 2.) - angle;*/
	fprintf(stderr, "Angle: %lf\n", angle);
}
