#include <darnit/darnit.h>


int main(int argc, char **argv) {
	int i;

	d_init_custom("Jymdsjepp^wArs", 1280, 720, 0, "birdie25", NULL);

	object_init(4);
	for (i = 0; i < 4; i++) {
		object_init_object(i, 1);
		object_update(i, 200.0 * i, 360, 0);
	}

	for (;;) {
		d_render_begin();
		object_draw();
		d_render_end();
		d_loop();
	}
}
