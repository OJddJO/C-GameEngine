#include "game.h"

void update(void *game);
void draw(void *game);
void event_handler(SDL_Event event, void *game);

static void create_hitboxes();

int main(int argc, char *argv[]) {
    engine_init("TinyWar", WIN_W, WIN_H, FPS);
    load_font("assets/font.ttf", 32, "font_32");
    load_font("assets/font.ttf", 64, "font_64");

    load_audio("audio/start.ogg", "start");
    load_audio("audio/click.ogg", "click");
    load_audio("audio/tie.ogg", "tie");
    load_audio("audio/win.ogg", "win");

    window_resizable(false);
    window_fullscreen(false);
    set_manual_update(true);
    set_background_color((Color){23, 15, 71, 255});

    Game *game = (Game *)malloc(sizeof(Game));
    if (game == NULL) {
        fprintf(stderr, "[GAME] Failed to allocate memory for game\n");
        exit(1);
    }
    init_game(game);

    create_hitboxes();

    play_audio_by_name("start", -1);
    engine_run(update, draw, event_handler, game);

    destroy_all_objects();
    destroy_all_textures();
    destroy_all_templates();
    close_all_audios();
    close_all_fonts();
    engine_quit();

    return 0;
}

static void create_hitboxes() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            char name[10];
            sprintf(name, "hitbox_%d_%d", i, j);
            create_hitbox(name, i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE);
        }
    }
}

void update(void *_game) {
    Game *game = _game;
    game->winner = check_winner(game);
}

void draw(void *_game) {
    Game *game = _game;
    if (game->winner == 0) {
        //draw grid
        for (int i = 0; i < 4; i++) {
            draw_line_thick(0, i * TILE_SIZE, WIN_W, i * TILE_SIZE, (Color){66, 50, 166, 255}, 5);
        }
        for (int i = 0; i < 4; i++) {
            draw_line_thick(i * TILE_SIZE, 0, i * TILE_SIZE, WIN_H, (Color){66, 50, 166, 255}, 5);
        }
        //draw X and O
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (game->matrix[i][j] == 1) {
                    draw_text("font_64", "X", i * TILE_SIZE + TILE_SIZE / 2, j * TILE_SIZE + TILE_SIZE / 2, (Color){255, 255, 255, 255}, CENTER);
                } else if (game->matrix[i][j] == 2) {
                    draw_text("font_64", "O", i * TILE_SIZE + TILE_SIZE / 2, j * TILE_SIZE + TILE_SIZE / 2, (Color){255, 255, 255, 255}, CENTER);
                }
            }
        }
    } else {
        char text[20];
        if (game->winner == -1) {
            sprintf(text, "It's a draw!");
            play_audio_by_name("tie", -1);
        } else {
            sprintf(text, "Player %d wins!", game->winner);
            play_audio_by_name("win", -1);
        }
        draw_text("font_32", text, WIN_W / 2, WIN_H / 2, (Color){255, 255, 255, 255}, CENTER);
    }
}

void event_handler(SDL_Event event, void *_game) {
    Game *game = _game;
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (game->winner == 0) {
                int x, y;
                get_mouse_position(&x, &y);
                int i = x / TILE_SIZE;
                int j = y / TILE_SIZE;
                char name[10];
                sprintf(name, "hitbox_%d_%d", i, j);
                if (object_is_hovered_by_name(name)) {
                    play_audio_by_name("click", -1);
                    game->matrix[i][j] = game->current_player;
                    game->current_player = game->current_player == 1 ? 2 : 1;
                    game->turn++;
                    destroy_object_by_name(name);
                    manual_update();
                }
            } else {
                destroy_all_objects();
                init_game(game);
                create_hitboxes();
                manual_update();
            }
    }
}