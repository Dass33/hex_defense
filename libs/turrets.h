#pragma once
#include <cstddef>
#include "coordinates.h"

class Turrets {
public:
    size_t get_cost();
    virtual void upgrade();
    virtual void attack();
protected:
    size_t cost;
    double attack_interval;
    Coordinates pos;
    bool is_attacking;
    size_t attack_road_index;
    size_t range;
};

class FireWall : public Turrets{};
class Blue_teamer : public Turrets{};
class Anti_hex : public Turrets {};
