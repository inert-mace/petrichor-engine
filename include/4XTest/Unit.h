#pragma once
#include <string>

struct Unit
{
    std::string name;
    int health;
    int attack;
    int armor;
    uint32_t abilities;

    Unit(std::string n, int h, int a, int d, uint32_t ab) : name(n), health(h), attack(a), armor(d), abilities(ab) {}
};