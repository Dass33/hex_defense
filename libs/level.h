#pragma once
#include <cstddef>
#include <array>
#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>
#include "coordinates.h"
#include "win_data.h"

constexpr int MAX_LEVEL = 6;
constexpr char LEVEL_PATH[] = "./assets/level";
constexpr int MAX_LEVEL_WIDTH = 190, MAX_LEVEL_HEIGHT = 55;
constexpr int MIN_LEVEL_WIDTH = 60, MIN_LEVEL_HEIGHT = 16;
constexpr size_t LEVEL_HEIGHT_PADDING = 2, LEVEL_WIDTH_PADDING = 2;
// Start and End chars have to differ
constexpr char START_CHAR = '*', END_CHAR = '#';
constexpr char PLAYER_ICON[] = "@";

class Game_state;
struct Player_state;

/**
 * @class Level
 * @brief Manages game level creation, editing, and path validation
 */
class Level {
public:
    /**
     * @brief Default constructor for Level
     * 
     * Initializes level dimensions based on screen size and creates an empty tile grid
     */
    Level();

    /**
     * @brief Insert a character at specific coordinates in the level
     * 
     * @param x X-coordinate to insert the character
     * @param y Y-coordinate to insert the character
     * @param val Character to insert at the specified location
     */
    void insert(int x, int y, int val);

    /**
     * @brief Retrieve the character at specific coordinates
     * 
     * @param x X-coordinate to retrieve
     * @param y Y-coordinate to retrieve
     * @return char Character at the specified location
     * or -1 if position is out of bounds
     */
    [[nodiscard]] char at(size_t x, size_t y) const;

    /**
     * @brief Build and validate the entire level
     * 
     * Checks for start and end points, ensures a valid path exists
     * 
     * @return bool True if level is valid, false otherwise
     */
    void edit_level();

    /**
     * @brief Print the current level to a window
     * 
     * @param win ncurses WINDOW pointer to print the level
     */
    void print_level(WINDOW *win) const;

    /**
     * @brief Save the current level to a file
     * 
     * @param level_path File path to save the level
     */
    void save_level(std::string& level_path) const;

    /**
     * @brief Build and validate the entire level
     * 
     * Checks for start and end points, ensures a valid path exists
     * 
     * @return bool True if level is valid, false otherwise
     */
    bool build_level();

    /**
     * @brief Checks todo
     * 
     * Checks for start and end points, ensures a valid path exists
     * 
     * @return bool True if level is valid, false otherwise
     */
    bool clear_n_tiles(Coordinates const pos, size_t width) const;

    /**
     * @brief Load a level from a file
     * 
     * @param level Level object to populate
     * @param level_path Path to the level file
     */
    friend void get_level(Level &level, std::string level_path);
    friend void game_loop(Level& level, Game_state& init_game_state);
    friend void round_loop(Win_data& win_data,Level& level, Game_state& game_state, Coordinates& pos);
    friend void player_actions(Coordinates& pos, WINDOW* win, Level& level,
                               Player_state& player, Game_state& game_state);
    friend void place_tower(Level& level, Coordinates& pos, Game_state& game_state, Player_state player);
private:
    std::vector<char> tiles; ///< 2D grid of level tiles represented as a 1D vector
    size_t yMax, xMax;  ///< Maximum coordinates of the screen
    size_t winHeight, winWidth; ///< Height and width of the level window

    /**
     * @brief Validate and build a path through the level
     * 
     * Checks that the level has a valid path from start to end
     * 
     * @return bool True if a valid path exists, false otherwise
     */
    bool build_path();
    std::vector<Coordinates> road;  ///< Path through the level
    Coordinates road_start;     ///< Starting point of the path
    Coordinates road_end;       ///< Ending point of the path

    /**
     * @brief Find the next tile matching a specific key
     * 
     * @param key Character to search for
     * @param pos Current position
     * @param prev_pos Previous position
     * @return size_t Number of matching tiles found
     */
    size_t find_next_tile(const char key, Coordinates& pos, Coordinates& prev_pos);

    /**
     * @brief Convert a linear index to 2D coordinates
     * 
     * @param index Linear index in the tile array
     * @param pos Reference to Coordinates to store the result
     */
    void get_coordinates(const size_t index, Coordinates& pos) const;
};

/**
 * @brief Check if a level fits within the current screen
 * 
 * @param index Level index to check
 * @return bool True if level fits, false otherwise
 */
bool level_fits_on_screen(int index);

/**
 * @brief Display and handle the level editing menu
 * 
 * @param menu ncurses WINDOW pointer for the menu
 */
void level_edit_menu(WINDOW* menu);

/**
 * @brief Display and handle the level play menu
 * 
 * @param menu ncurses WINDOW pointer for the menu
 */
void level_play_menu(WINDOW* menu);
