#pragma once
#include <cstddef>
#include <ncurses.h>
#include <fstream>
#include <vector>
#include "coordinates.h"
#include "moving_objects.h"
#include "turrets.h"

static constexpr size_t BASE_HEALTH = 100;
static constexpr size_t BASE_ROUNDS = 5;
constexpr size_t TICK_LENGTH_MS = 10;
constexpr double ENEMY_INTERVAL = 0.05 * TICK_LENGTH_MS;
constexpr char DIFFICULTIES[][10] = {"Easy","Medium","Hard"};
constexpr char ROUNDS_PATH[] = "./assets/rounds";
constexpr size_t STARTING_MONEY = 240;
constexpr size_t ROUND_BONUS = 80;
enum Player_modes { basic, blue_teamer, fire_wall, anti_hex};

class Level;
class Player_state;

class Game_state {
public:
    Game_state(int difficulty, int game_mode = 0);
    ~Game_state();
    void load_next_round(std::ifstream& ifs);
    bool save_state() const;
    bool validate_rounds() const;
    void print_road(WINDOW* win, std::vector<Coordinates>road) const;
    bool turret_collides(Coordinates& pos);
    friend void game_loop(Level& level, Game_state& init_game_state);
    friend void round_loop( WINDOW* play_win,Level& level, Game_state& game_state, Coordinates& pos);
    friend void player_actions(Coordinates& pos, WINDOW* win, Level& level,
                               Player_state& player, Game_state& game_state);
    friend void place_tower(Level& level, Coordinates& pos, Game_state& game_state, Player_state player);
private:
    Enemies enemies;
    Alies alies;
    std::vector<Turrets*> turrets;
    size_t curr_round = 0;
    size_t rounds_count;
    size_t curr_hp;
    size_t money;
    int difficulty;
    int game_mode;
    void game_won();
    void game_lost();
    size_t from_hex(char c) const;
};

bool select_game_mode(WINDOW* menu, Level& level);
