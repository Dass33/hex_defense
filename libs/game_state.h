#pragma once
#include <cstddef>
#include <ncurses.h>
#include <fstream>
#include <vector>
#include "coordinates.h"
#include "moving_objects.h"

static constexpr size_t BASE_HEALTH = 100;
static constexpr size_t BASE_ROUNDS = 5;
constexpr char DIFFICULTIES[][10] = {"Easy","Medium","Hard"};
constexpr char ROUNDS_PATH[] = "./assets/rounds";

class Level;

class Game_state {
public:
    Game_state(int difficulty, int game_mode = 0);
    void load_next_round(std::ifstream& ifs);
    bool save_state() const;
    bool validate_rounds() const;
    void print_road(WINDOW* win, std::vector<Coordinates>road) const;
    friend void game_loop(Level& level, Game_state& init_game_state);
    friend void round_loop( WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos);
private:
    Enemies enemies;
    Alies alies;
    //turrets
    size_t curr_round = 0;
    size_t rounds_count;
    size_t curr_hp;
    int difficulty;
    int game_mode;
    void game_won();
    void game_lost();
    size_t from_hex(char c) const;
};

bool select_game_mode(WINDOW* menu, Level& level);
