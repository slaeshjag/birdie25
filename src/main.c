#include <darnit/darnit.h>

int main(int argc, char **argv) {
	d_init_custom("Birdie25", 800, 600, 0, "birdie25", NULL);

	for (;;) {
		d_render_begin();
		d_render_end();
		d_loop();
	}

	d_exit();
	return 0;
}
