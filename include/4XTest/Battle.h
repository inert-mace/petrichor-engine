#pragma once
#include <iostream>
#include <random>
#include "Army.h"
#include "BattleEvents.h"

class Battle
{
public:
    // Simulates a battle between two armies and returns the result as a BattleLog containing the sequence of events and the final outcome
    BattleLog simulateBattle(Army& army1, Army& army2);
    void checkRangedAttacks(Army& army1, Army& army2, BattleStep& step, int targetArmyIndex, std::mt19937& rng);
    void checkBothRangedAttacks(Army& army1, Army& army2, BattleStep& step, std::mt19937& rng);
    void removeDeadUnits(Army& army, BattleStep& step, int armyIndex);
    void frontlineAttack(Army& attackingArmy, Army& defendingArmy, BattleStep& step, int attackerArmyIndex, int targetArmyIndex);
    bool checkVictory(Army& army1, Army& army2, BattleStep& step, BattleLog& battleLog);

    int maxTurnsTillDraw = 100;
};