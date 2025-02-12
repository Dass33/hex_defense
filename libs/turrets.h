#pragma once
#include <cstddef>
#include <ncurses.h>
#include <vector>
#include "coordinates.h"
#include "moving_objects.h"
#include "win_data.h"

constexpr char TOWERS_ICONS[][4] = {"!0x","/^\\",">@<"};
constexpr int TOWERS_SIZE = sizeof TOWERS_ICONS[0] / sizeof(char) -1;

class Turrets {
public:
    Turrets(const Coordinates pos);
    virtual ~Turrets() {};
    size_t get_cost();
    virtual void upgrade() = 0;
    virtual size_t attack(Enemies &enemies) = 0;
    virtual void print(WINDOW* win) const = 0;
    Coordinates get_pos();
    void round_reset() {};
    static void print_range(Win_data win_data, Coordinates& pos, size_t mode);
protected:
    size_t upgrade_cost;
    size_t damadge;
    size_t tier = 0;
    size_t attack_interval;
    Coordinates pos;
    bool is_attacking = false;
    int attack_road_index = -1;
    size_t range;
};

constexpr size_t FIRE_WALL_COST = 70;
constexpr size_t FW_ATTACK_INTERVAL = 300;
constexpr size_t FW_BASE_RANGE = 1;

class FireWall : public Turrets {
public:
    FireWall(Coordinates pos);
    ~FireWall() {};
    size_t attack(Enemies &enemies) override;
    void upgrade() override;
    void print(WINDOW* win) const override;
private:
    size_t damadge = 1;
};

constexpr size_t BLUE_TEAMER_COST = 60;
constexpr size_t BT_ATTACK_INTERVAL = 100;
constexpr size_t BT_BASE_RANGE = 4;

class Blue_teamer : public Turrets {
public:
    Blue_teamer(Coordinates pos, const std::vector<Coordinates>& road);
    ~Blue_teamer() {};
    size_t attack(Enemies &enemies) override;
    void upgrade() override;
    void print(WINDOW* win) const override;
    void round_reset();
private:
    size_t damadge = 1;
    size_t attack_idx = 0;
    bool is_attacking = false;
    size_t range = BT_BASE_RANGE;
    size_t find_enemy(Enemies& enemies);
    std::vector<size_t>road_in_range;
};

constexpr size_t ANTI_HEX_COST = 100;
constexpr size_t AH_ATTACK_INTERVAL = 200;
constexpr size_t AH_BASE_RANGE = 2;

class Anti_hex : public Turrets {
public:
    Anti_hex(Coordinates pos, const std::vector<Coordinates>& road);
    ~Anti_hex() {};
    size_t attack(Enemies &enemies) override;
    void upgrade() override;
    void print(WINDOW* win) const override;
    size_t damadge = 1;
private:
    long spawn_index = -1;
};
