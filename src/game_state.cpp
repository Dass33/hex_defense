#include "../libs/game_state.h"
#include "../libs/level.h"

bool select_game_mode(WINDOW* menu, Level& level) {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    wclear(menu);
    box(menu, 0, 0);
    int input = ' ';

    mvwprintw(menu,0,2,"Game mode");

    long dff_count = sizeof DIFFICULTIES / sizeof(char[10]);
    for (int i = 0; i < dff_count;i++) {
        mvwprintw(menu, i+2,2,"%s", DIFFICULTIES[i]);
    }

    mvwprintw(menu, dff_count+3,2,"Choose difficulty. (1-3)");

    for(;;) {
        mvprintw(yMax-1,1,"Last input: %c ", input);
        input = wgetch(menu);
        if (input == 'q') return false;
        if (input - '0' <= dff_count && input - '0' >= 1) break;
    }
    clear();

    int difficulty = input - '0';
    Game_state init_game_state(difficulty);
    game_loop(level, init_game_state);
    return true;
}

Game_state::Game_state(int difficulty, int game_mode)
    :difficulty(difficulty), game_mode(game_mode) {
    curr_hp = BASE_HEALTH / difficulty;
    rounds_count = BASE_ROUNDS * difficulty * difficulty;
}
