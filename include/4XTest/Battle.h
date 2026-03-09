#pragma once
#include <iostream>
#include "Army.h"
#include "BattleEvents.h"

class Battle
{
public:
    Battle();
    ~Battle();

    // Simulates a battle between two armies and returns the result as a BattleLog containing the sequence of events and the final outcome
    BattleLog simulateBattle(Army& army1, Army& army2);

    int maxTurnsTillDraw = 100;
};