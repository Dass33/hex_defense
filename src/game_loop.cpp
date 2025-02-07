#include "../libs/level.h"
#include "../libs/game_state.h"
#include <ncurses.h>
//#include <chrono>
//#include <thread>

#define curr_round game_state.curr_round
#define rounds_count game_state.rounds_count

void round_loop( WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos) {
    int c = -1;

    bool running = true;
        for (;running;) {
            c = wgetch(play_win);
            switch (c) {
                case KEY_UP:
                    pos.y--;
                    if (pos.y < 1)
                        pos.y = level.winHeight - 2;
                    break;
                case KEY_DOWN:
                    pos.y++;
                    if (pos.y > level.winHeight - 2)
                        pos.y = 1;
                    break;
                case KEY_RIGHT:
                    pos.x++;
                    if (pos.x > level.winWidth - 2)
                        pos.x = 1;
                    break;
                case KEY_LEFT:
                    pos.x--;
                    if (pos.x < 1)
                        pos.x = level.winWidth - 2;
                    break;
                case 'q':
                    running = false;
                    break;
                case ERR:
                default:
                    break;
            }
            printw("%ld", game_state.curr_hp);

            box(play_win, 0, 0);
            level.print_level(play_win);
            mvwprintw(play_win, pos.y, pos.x, "@");
            wrefresh(play_win);
        }
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
    
    for (;curr_round < rounds_count ;curr_round++) {
        round_loop(play_win, level, game_state, pos);
    }

    wclear(play_win);
    wrefresh(play_win);
}
