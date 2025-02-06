#pragma once
#include <ncurses.h>
#include <string>
#include <vector>


static constexpr int MAX_LEVEL = 6;
static constexpr char LEVEL_PATH[] = "./assets/level";
static constexpr int MAX_LEVEL_WIDTH = 190, MAX_LEVEL_HEIGHT = 55;
static constexpr int MIN_LEVEL_WIDTH = 60, MIN_LEVEL_HEIGHT = 16;
static constexpr size_t LEVEL_HEIGHT_PADDING = 2, LEVEL_WIDTH_PADDING = 2;

class Level {
public:
    Level();

    void insert(int x, int y, int val);
    [[nodiscard]] char at(int x, int y) const;

    void edit_level();
    void print_level(WINDOW *win) const;
    void save_level(std::string& level_path) const;
    friend void get_level(Level &level, std::string level_path);
private:
        std::vector<char> tiles;
        size_t yMax, xMax;
        size_t winHeight, winWidth;
};

void level_menu(WINDOW* menu);
