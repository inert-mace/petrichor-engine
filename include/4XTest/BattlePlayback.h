#pragma once
#include <iostream>
#include "BattleEvents.h"
#include "VariantUtils.h"

struct BattlePlayback
{
    void playBattleLog(BattleLog& log);
};

void BattlePlayback::playBattleLog(BattleLog& log) {
    for(BattleStep& step : log.steps)
    {
        for(BattleEvent& event : step.events)
        {
            // visit uses the overloaded struct from VariantUtils to handle each type of BattleEvent and print out relevant information about it
            // this is possible because BattleEvent is a std::variant of all the different event types, so we can use std::visit to handle each type with a lambda
            std::visit(overloaded{
                [](TurnStartedEvent& e) { std::cout << "Turn " << e.turnNumber << " started." << std::endl; },
                [](AttackDeclaredEvent& e) { std::cout << "Army " << e.attackerArmy + 1 << " unit " << e.attackingUnitName << " declared an attack on Army " << e.targetArmy + 1 << " unit " << e.targetUnitName << " in slot " << e.targetSlot << " with attack value " << e.attackValue << (e.ranged ? " (ranged)" : "") << "." << std::endl; },
                [](DamageAppliedEvent& e) { std::cout << "Army " << e.targetArmy + 1 << " unit " << e.targetUnitName << " in slot " << e.targetSlot << " took " << e.damageDealt << " damage (health: " << e.oldHealth << " -> " << e.newHealth << ")." << std::endl; },
                [](UnitDiedEvent& e) { std::cout << "Army " << e.army + 1 << " unit " << e.deadUnitName << " in slot " << e.slot << " died." << std::endl; },
                [](BattleEndedEvent& e) { 
                    std::string resultStr;
                    switch(e.result)
                    {
                        case BattleResult::Draw: resultStr = "Draw"; break;
                        case BattleResult::Army1Victory: resultStr = "Army 1 Victory"; break;
                        case BattleResult::Army2Victory: resultStr = "Army 2 Victory"; break;
                    }
                    std::cout << "Battle ended with result: " << resultStr << "." << std::endl;
                }
            }, event);
        }
    }
}