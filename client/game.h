#ifndef __GAME_H__

void game_render();
void game_init();

typedef struct Game Game;
struct Game {
	pthread_t thread;
};

extern Game game;

#endif
