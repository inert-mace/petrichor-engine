#include "../include/4XTest/Battle.h"
#include "../include/4XTest/BattleEvents.h"
#include "../include/4XTest/Abilities.h"
#include <random>
#include <algorithm>

BattleLog Battle::simulateBattle(Army& army1, Army& army2)
{
    std::cout << "Simulating battle between two armies..." << std::endl;
    std::random_device rd;
    std::mt19937 rng(rd());

    BattleLog battleLog;
    battleLog.result = BattleResult::Draw; // default to draw, will update if there's a winner

    int turn = 0;
    
    {
        BattleStep step;
        if(checkVictory(army1, army2, step, battleLog)) 
        {
            battleLog.steps.push_back(step);
            return battleLog; // if there's a winner before the battle starts (e.g. one orboth armies are empty), return the battle log immediately
        }
    }

    while(turn < maxTurnsTillDraw) {
        BattleStep step;
        step.events.push_back(TurnStartedEvent{turn + 1});

        frontlineAttack(army1, army2, step, 0, 1);
        frontlineAttack(army2, army1, step, 1, 0);
        checkBothRangedAttacks(army1, army2, step, rng);
        removeDeadUnits(army1, step, 0);
        removeDeadUnits(army2, step, 1);

        if(checkVictory(army1, army2, step, battleLog)) 
        {
            battleLog.steps.push_back(step);
            return battleLog; // if there's a winner, return the battle log immediately
        }

        battleLog.steps.push_back(step);
        turn++;
    }

    return battleLog; // draw if no winner after maxTurnsTillDraw turns
}


// loop through each army's units starting from the back and check for ranged units to attack with after the frontline attacks
// since ranged units fire at the same time, we only remove units from the army after all ranged attacks have been declared, so that all ranged units get to attack even if some are killed by other ranged attacks in the same turn
void Battle::checkRangedAttacks(Army& army1, Army& army2, BattleStep& step, int targetArmyIndex, std::mt19937& rng) {
    // i starts at the back of the army and moves forward to check for ranged units, so that if multiple ranged units exist, the one furthest back attacks first
    // i > 0 prevents ranged units from attacking if they are in the frontline
    for(int i = army1.units.size() - 1; i > 0; i--)
    {
        if(army1.units[i].abilities & Ranged)
        {
            if(army2.units.size() > 0)
            {
                // ranged unit attacks frontline of opposing army
                Unit& targetUnit = army2.units[0];
                step.events.push_back(AttackDeclaredEvent{army1.units[i].name, targetUnit.name, 0, i, targetArmyIndex, 0, army1.units[i].attack, true});
                targetUnit.health -= std::max(0, army1.units[i].attack - targetUnit.armor);
                step.events.push_back(DamageAppliedEvent{targetUnit.name, targetArmyIndex, 0, std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health + std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health});
            }
        }
        if(army1.units[i].abilities & RangedVolley)
        {
            if(army2.units.size() > 0)
            {
                // ranged volley unit attacks random (mersenne twister) enemy unit 
                int targetIndex = std::uniform_int_distribution<int>(0, army2.units.size() - 1)(rng);
                Unit& targetUnit = army2.units[targetIndex];
                step.events.push_back(AttackDeclaredEvent{army1.units[i].name, targetUnit.name, 0, i, targetArmyIndex, targetIndex, army1.units[i].attack, true});
                targetUnit.health -= std::max(0, army1.units[i].attack - targetUnit.armor);
                step.events.push_back(DamageAppliedEvent{targetUnit.name, targetArmyIndex, targetIndex, std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health + std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health});
            }
        }
    }
}

void Battle::removeDeadUnits(Army& army, BattleStep& step, int armyIndex) {
    for(int i = 0; i < army.units.size(); i++)
    {
        if(army.units[i].health <= 0)
        {
            step.events.push_back(UnitDiedEvent{army.units[i].name, armyIndex, i});
        }
    }
    army.units.erase(std::remove_if(army.units.begin(), army.units.end(), [](Unit& unit){ return unit.health <= 0; }), army.units.end()); // remove dead units from army
}

void Battle::checkBothRangedAttacks(Army& army1, Army& army2, BattleStep& step, std::mt19937& rng) {
    checkRangedAttacks(army1, army2, step, 1, rng);
    checkRangedAttacks(army2, army1, step, 0, rng);
}

void Battle::frontlineAttack(Army& attackingArmy, Army& defendingArmy, BattleStep& step, int attackerArmyIndex, int targetArmyIndex) {
    if(attackingArmy.units.size() == 0 || defendingArmy.units.size() == 0) return; // if either army has no units, skip frontline attack
    int attackValue = attackingArmy.units[0].attack;
    if((attackingArmy.units[0].abilities & Ranged) || (attackingArmy.units[0].abilities & RangedVolley)) attackValue /= 2; // if unit has ranged ability, attack value is halved in frontline
    step.events.push_back(AttackDeclaredEvent{attackingArmy.units[0].name, defendingArmy.units[0].name, attackerArmyIndex, 0, targetArmyIndex, 0, attackValue, false});

    defendingArmy.units[0].health -= std::max(0, attackValue - defendingArmy.units[0].armor);

    step.events.push_back(DamageAppliedEvent{defendingArmy.units[0].name, targetArmyIndex, 0, std::max(0, attackValue - defendingArmy.units[0].armor), defendingArmy.units[0].health + std::max(0, attackValue - defendingArmy.units[0].armor), defendingArmy.units[0].health});
}

bool Battle::checkVictory(Army& army1, Army& army2, BattleStep& step, BattleLog& battleLog) {
    if(army1.units.size() == 0 && army2.units.size() == 0)
    {
        step.events.push_back(BattleEndedEvent{BattleResult::Draw});
        battleLog.result = BattleResult::Draw; // draw if both armies have no units
        return true;
    }
    else if(army1.units.size() == 0)
    {
        step.events.push_back(BattleEndedEvent{BattleResult::Army2Victory});
        battleLog.result = BattleResult::Army2Victory; // army2 wins if army1 has no units
        return true;
    }
    else if(army2.units.size() == 0)
    {
        step.events.push_back(BattleEndedEvent{BattleResult::Army1Victory});
        battleLog.result = BattleResult::Army1Victory; // army1 wins if army2 has no units
        return true;
    }
    else
    {
        return false; // no winner yet
    }
}