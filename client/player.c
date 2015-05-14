#include <darnit/darnit.h>
#include "player.h"

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
	
}
