#pragma once
#include <cstddef>
#include <ncurses.h>
#include "coordinates.h"

constexpr char TOWERS_ICONS[][4] = {"!0x","/^\\",">@<"};

class Turrets {
public:
    Turrets(const Coordinates pos);
    virtual ~Turrets() {};
    size_t get_cost();
    virtual void upgrade() = 0;
    virtual void attack() = 0;
    virtual void print(WINDOW* win) const = 0;
    Coordinates get_pos();
protected:
    size_t upgrade_cost;
    size_t tier = 0;
    double attack_interval;
    Coordinates pos;
    bool is_attacking = false;
    int attack_road_index = -1;
    size_t range;
};

constexpr size_t FIRE_WALL_COST = 70;

class FireWall : public Turrets {
public:
    FireWall(Coordinates pos);
    ~FireWall() {};
    void attack() override;
    void upgrade() override;
    void print(WINDOW* win) const override;
private:
};

constexpr size_t BLUE_TEAMER_COST = 60;

class Blue_teamer : public Turrets {
public:
    Blue_teamer(Coordinates pos);
    ~Blue_teamer() {};
    void attack() override;
    void upgrade() override;
    void print(WINDOW* win) const override;
private:
};

constexpr size_t ANTI_HEX_COST = 100;

class Anti_hex : public Turrets {
public:
    Anti_hex(Coordinates pos);
    ~Anti_hex() {};
    void attack() override;
    void upgrade() override;
    void print(WINDOW* win) const override;
private:
};
