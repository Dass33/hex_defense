#pragma once
#include <cstddef>
#include <ncurses.h>
#include <string>
#include <vector>


static constexpr int MAX_LEVEL = 6;
static constexpr char LEVEL_PATH[] = "./assets/level";
static constexpr int MAX_LEVEL_WIDTH = 190, MAX_LEVEL_HEIGHT = 55;
static constexpr int MIN_LEVEL_WIDTH = 60, MIN_LEVEL_HEIGHT = 16;
static constexpr size_t LEVEL_HEIGHT_PADDING = 2, LEVEL_WIDTH_PADDING = 2;
// Start and End chars have to differ
static constexpr char START_CHAR = '*', END_CHAR = '#';

class Level {
public:
    Level();

    void insert(int x, int y, int val);
    [[nodiscard]] char at(int x, int y) const;

    void edit_level();
    void play_level() const;
    void print_level(WINDOW *win) const;
    void save_level(std::string& level_path) const;
    bool build_level();
    friend void get_level(Level &level, std::string level_path);
private:
    std::vector<char> tiles;
    size_t yMax, xMax;
    size_t winHeight, winWidth;
    bool build_path();
    struct Coordinates{
        Coordinates(size_t x = 0, size_t y = 0) :x(x),y(y){}
        size_t x;
        size_t y;
    };
    std::vector<Coordinates> road;
    Coordinates road_start, road_end;
    size_t find_next_tile(const char key, Coordinates& pos, Coordinates& prev_pos);
    void get_coordinates(const size_t index, Coordinates& pos) const;
};

void level_edit_menu(WINDOW* menu);
void level_play_menu(WINDOW* menu);
