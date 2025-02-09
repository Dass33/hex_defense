#include "../libs/level.h"
#include "../libs/game_state.h"
#include "../libs/moving_objects.h"
#include <chrono>
#include <cstddef>
#include <fstream>
#include <ncurses.h>
#include <thread>
//#include <chrono>

#define curr_round game_state.curr_round
#define rounds_count game_state.rounds_count

void round_loop(WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos) {
    nodelay(play_win, true);
    size_t max_enemies = game_state.enemies.vec.size();
    size_t curr_enemies = 0;

    auto last_enemy_move = std::chrono::steady_clock::now();
    std::chrono::duration<double>enemy_interval(0.1);
    while (game_state.enemies.enemies_left) {
        int input = wgetch(play_win);
        switch (input) {
            case ERR:
            break;
            case KEY_UP:
                pos.y--;
                if (pos.y < 1) pos.y = level.winHeight -2;
            break;
            case KEY_DOWN:
                pos.y++;
                if (pos.y > level.winHeight -2) pos.y = 1;
            break;
            case KEY_RIGHT:
                pos.x++;
                if (pos.x > level.winWidth -2) pos.x = 1;
            break;
            case KEY_LEFT:
                pos.x--;
                if (pos.x < 1) pos.x = level.winWidth -2;
            break;
            default: break;
        }
        level.print_level(play_win);
        mvprintw(1, 0, "Enemies left: %02ld", game_state.enemies.enemies_left);
        mvprintw(2, 0, "hp: %03ld", game_state.curr_hp);
        

        auto now = std::chrono::steady_clock::now();
        if (now -last_enemy_move > enemy_interval) {
            last_enemy_move = now;
            if (curr_enemies < max_enemies) curr_enemies++;
            game_state.curr_hp -= game_state.enemies.update(curr_enemies, level.road.size() -1);
        }
        game_state.print_road(play_win, level.road);
        mvwprintw(play_win, pos.y, pos.x, "@");

        refresh();
        wrefresh(play_win);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    nodelay(play_win, false);
}

void game_loop(Level& level, Game_state& game_state) {

    int win_y_start = (level.yMax-level.winHeight)/2;
    int win_x_start = (level.xMax-level.winWidth)/2;
    WINDOW *play_win = newwin(level.winHeight, level.winWidth, win_y_start, win_x_start);
    keypad(play_win, true);

    Coordinates pos(1,1);

    level.print_level(play_win);
    mvwprintw(play_win,1,1,"@");
    wrefresh(play_win);

    std::ifstream rounds_file(ROUNDS_PATH);
    
    for (;curr_round < rounds_count ;curr_round++) {
        mvprintw(0, 0, "Round %ld", curr_round);
        refresh();
        game_state.load_next_round(rounds_file);
        round_loop(play_win, level, game_state, pos);
        game_state.enemies.vec.clear();
    }

    rounds_file.close();

    wclear(play_win);
    wrefresh(play_win);
}
