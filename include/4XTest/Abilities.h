#pragma once

enum Ability
{
    Brutal = 1 << 0, // every third attack from frontline deals double damage.
    Ranged = 1 << 1, // if unit is not frontline, attacks opposing frontline after allied frontline attacks. halves attack value when in frontline.
    RangedVolley = 1 << 2, // attacks a random enemy unit after frontline attacks. halves attack value in frontline. this can stack with Ranged for a second attack if not in frontline.
    Assassin = 1 << 3, // attacks backline instead of frontline. if no backline, attacks frontline as normal.
    Charge = 1 << 4, // after frontline dies, moves to front and attacks if not already in frontline. done in order of backline to unit behind frontline, so if multiple units have charge, the one furthest back moves first.
    Siege = 1 << 5  , // attacks also damage units directly behind and in front of target (if they exist) at 50% damage.
    Commander = 1 << 5, // when it attacks, triggers Charge in all allied units with Charge.
    Fearsome = 1 << 6, // when it attacks, attacked unit swaps places with allied backline unit if they exist.
};