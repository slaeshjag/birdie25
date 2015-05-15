#ifndef __INPUT_H__
#define	__INPUT_H__

#include <stdbool.h>

enum ClientInputKeys {
	CLIENT_INPUT_KEY_THRUST = 01,
	CLIENT_INPUT_KEY_ANTITHRUST = 02,
	CLIENT_INPUT_KEY_SHOOT = 04,
	CLIENT_INPUT_KEY_RELEASE = 010,
};

typedef struct PlayerList PlayerList;
struct PlayerList{
	char			*name;
	int			id;
	int			score;
};

extern PlayerList playerlist[8];

int player_get();
void camera_init(int focus_object, int home);
void handle_camera();
void handle_player();
int player_draw_hud();
#endif
