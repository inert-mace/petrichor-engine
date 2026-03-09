#include "Battle.h"
#include "BattleEvents.h"
#include "Abilities.h"
#include <random>

Battle::Battle()
{
    std::cout << "Battle constructor called" << std::endl;
}

Battle::~Battle()
{
    std::cout << "Battle destructor called" << std::endl;
}

BattleLog Battle::simulateBattle(Army& army1, Army& army2)
{
    std::cout << "Simulating battle between two armies..." << std::endl;
    std::random_device rd;
    std::mt19937 rng(rd());

    BattleLog battleLog;
    battleLog.result = BattleResult::Draw; // default to draw, will update if there's a winner

    int turn = 0;
    while(turn < maxTurnsTillDraw) {
        BattleStep step;
        step.events.push_back(TurnStartedEvent{turn + 1});
        if(army1.units.size() == 0 && army2.units.size() == 0)
        {
            battleLog.result = BattleResult::Draw; // draw if both armies have no units
            return battleLog;
        }
        if(army1.units.size() == 0)
        {
            battleLog.result = BattleResult::Army2Victory; // army2 wins if army1 has no units
            return battleLog;
        }
        if(army2.units.size() == 0)
        {
            battleLog.result = BattleResult::Army1Victory; // army1 wins if army2 has no units
            return battleLog;
        }
        Unit frontline1 = army1.units.front();
        Unit frontline2 = army2.units.front();

        step.events.push_back(AttackDeclaredEvent{0, 0, 1, 0, frontline1.attack, false});
        step.events.push_back(AttackDeclaredEvent{1, 0, 0, 0, frontline2.attack, false});

        frontline1.health -= std::max(0, frontline2.attack - frontline1.armor);
        frontline2.health -= std::max(0, frontline1.attack - frontline2.armor);

        step.events.push_back(DamageAppliedEvent{0, 0, std::max(0, frontline2.attack - frontline1.armor), frontline1.health + std::max(0, frontline2.attack - frontline1.armor), frontline1.health});
        step.events.push_back(DamageAppliedEvent{1, 0, std::max(0, frontline1.attack - frontline2.armor), frontline2.health + std::max(0, frontline1.attack - frontline2.armor), frontline2.health});

        if(frontline1.health <= 0)
        {
            army1.units.erase(army1.units.begin()); // remove dead unit from army1
            step.events.push_back(UnitDiedEvent{0, 0});
        }
        else
        {
            army1.units[0] = frontline1; // update frontline unit in army1
        }
        if(frontline2.health <= 0)
        {
            army2.units.erase(army2.units.begin()); // remove dead unit from army2
            step.events.push_back(UnitDiedEvent{1, 0});
        }
        else
        {
            army2.units[0] = frontline2; // update frontline unit in army2
        }

        turn++;
    }

    return battleLog; // draw if no winner after maxTurnsTillDraw turns
}


// loop through each army's units starting from the back and check for ranged units to attack with after the frontline attacks
// since ranged units fire at the same time, we only remove units from the army after all ranged attacks have been declared, so that all ranged units get to attack even if some are killed by other ranged attacks in the same turn
void checkRangedAttacks(Army& army1, Army& army2, BattleStep& step, int targetArmyIndex, std::mt19937& rng) {
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
                step.events.push_back(AttackDeclaredEvent{0, i, targetArmyIndex, 0, army1.units[i].attack, true});
                targetUnit.health -= std::max(0, army1.units[i].attack - targetUnit.armor);
                step.events.push_back(DamageAppliedEvent{targetArmyIndex, 0, std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health + std::max(0, (army1.units[i].attack / 2) - targetUnit.armor), targetUnit.health});
            }
        }
        else if(army1.units[i].abilities & RangedVolley)
        {
            if(army2.units.size() > 0)
            {
                // ranged volley unit attacks random (mersenne twister) enemy unit 
                int targetIndex = std::uniform_int_distribution<int>(0, army2.units.size() - 1)(rng);
                Unit& targetUnit = army2.units[targetIndex];
                step.events.push_back(AttackDeclaredEvent{0, i, targetArmyIndex, targetIndex, army1.units[i].attack, true});
                targetUnit.health -= std::max(0, army1.units[i].attack - targetUnit.armor);
                step.events.push_back(DamageAppliedEvent{targetArmyIndex, targetIndex, std::max(0, army1.units[i].attack - targetUnit.armor), targetUnit.health + std::max(0, (army1.units[i].attack / 2) - targetUnit.armor), targetUnit.health});
            }
        }
    }
}

void checkDeadUnits(Army& army, BattleStep& step, int armyIndex) {
    // check for dead units and remove them from the army after all attacks have been declared, so that all units get to attack even if some are killed by other attacks in the same turn
    for(int i = 0; i < army.units.size(); i++)
    {
        if(army.units[i].health <= 0)
        {
            army.units.erase_if([&](const Unit& unit){ return &unit == &army.units[i]; }); // remove dead unit from army
            step.events.push_back(UnitDiedEvent{armyIndex, i});
        }
    }
}

void checkBothRangedAttacks(Army& army1, Army& army2, BattleStep& step, std::mt19937& rng) {
    checkRangedAttacks(army1, army2, step, 1, rng);
    checkRangedAttacks(army2, army1, step, 0, rng);
}