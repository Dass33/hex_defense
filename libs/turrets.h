#pragma once
#include <cstddef>
#include <ncurses.h>
#include <vector>
#include "coordinates.h"
#include "level.h"
#include "moving_objects.h"
#include "win_data.h"

constexpr char TOWERS_ICONS[][4] = {"!0x","/^\\",">@<"};
constexpr int TOWERS_SIZE = sizeof TOWERS_ICONS[0] / sizeof(char) -1;
constexpr double SELL_RECUPERATION = 0.6;

struct Base_tower {
    constexpr Base_tower(size_t c, size_t i, size_t r)
    : cost(c), attack_interval(i), range(r){}
    size_t cost;
    size_t attack_interval;
    size_t range;
};

constexpr Base_tower FIRE_WALL(80, 280, 1);
constexpr Base_tower BLUE_TEAMER(60, 100, 4);
constexpr Base_tower ANTI_HEX(130,200,2);
constexpr Base_tower BASE_TOWERS[] = {BLUE_TEAMER, FIRE_WALL, ANTI_HEX};

class Turrets {
public:
    explicit Turrets(Coordinates pos);
    virtual ~Turrets() = default;
    virtual size_t attack(Mv_objects &enemies) = 0;
    virtual void print(WINDOW* win) const = 0;
    virtual bool upgrade(Coordinates& pos, size_t& money, const std::vector<Coordinates>&road) = 0;
    [[nodiscard]] virtual size_t get_sell_value() const = 0;
    [[nodiscard]] virtual size_t get_upgrade_cost() const = 0;
    Coordinates get_pos();
    void round_reset() {};
    static void print_range(Win_data win_data, const Coordinates& pos, size_t mode);
protected:
    size_t upgrade_cost;
    int damage;
    size_t tier = 0;
    size_t attack_interval;
    Coordinates pos;
    bool is_attacking = false;
    int attack_road_index = -1;
    size_t range;
};

class FireWall : public Turrets {
public:
    FireWall(Coordinates pos);
    ~FireWall() {};
    size_t attack(Mv_objects &enemies) override;
    void print(WINDOW* win) const override;
    bool upgrade(Coordinates& pos, size_t& mone, const std::vector<Coordinates>&road) override;
    [[nodiscard]] size_t get_sell_value() const override;
    [[nodiscard]] size_t get_upgrade_cost() const override;
private:
    int damage = 1;
    size_t upgrade_cost = FIRE_WALL.cost /2;
};

class Blue_teamer : public Turrets {
public:
    Blue_teamer(Coordinates pos, const std::vector<Coordinates>& road);
    ~Blue_teamer() {};
    size_t attack(Mv_objects &enemies) override;
    void print(WINDOW* win) const override;
    bool upgrade(Coordinates& pos, size_t& mone,
        const std::vector<Coordinates>&road) override;
    [[nodiscard]] size_t get_sell_value() const override;
    [[nodiscard]] size_t get_upgrade_cost() const override;
    void round_reset();
private:
    int damage = 1;
    size_t attack_idx = 0;
    bool is_attacking = false;
    size_t range = BLUE_TEAMER.range;
    size_t find_enemy(Mv_objects& enemies);
    std::vector<size_t>road_in_range;
    size_t upgrade_cost = BLUE_TEAMER.cost /2;
};

class Anti_hex : public Turrets {
public:
    Anti_hex(Coordinates pos, const std::vector<Coordinates>& road);
    ~Anti_hex() {};
    size_t attack(Mv_objects &mv_objects) override;
    void print(WINDOW* win) const override;
    bool upgrade(Coordinates& pos, size_t& mone, const std::vector<Coordinates>&road) override;
    [[nodiscard]] size_t get_sell_value() const override;
    [[nodiscard]] size_t get_upgrade_cost() const override;
private:
    int damage = 1;
    long spawn_index = -1;
    [[nodiscard]] long spawn_collision(const Mv_objects &mv_objects) const;
    size_t upgrade_cost = ANTI_HEX.cost /2;
};
