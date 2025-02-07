#pragma once
#include <cstddef>
#include <ncurses.h>
#include "coordinates.h"

static constexpr size_t BASE_HEALTH = 100;
static constexpr size_t BASE_ROUNDS = 5;
constexpr char DIFFICULTIES[][10] = {"Easy","Medium","Hard"};

class Level;

class Game_state {
public:
    Game_state(int difficulty, int game_mode = 0);
    void start_next_round();
    bool save_state() const;
    friend void game_loop(Level& level, Game_state& init_game_state);
    friend void round_loop( WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos);

private:
    void game_won();
    void game_lost();
    //enemies
    //turrets
    size_t curr_round = 0;
    size_t rounds_count;
    size_t curr_hp;
    int difficulty;
    int game_mode;
};

bool select_game_mode(WINDOW* menu, Level& level);
