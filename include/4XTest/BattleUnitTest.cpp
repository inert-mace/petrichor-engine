#include "Battle.h"
#include "BattleEvents.h"
#include "Abilities.h"
#include "Unit.h"

void testBattle1() {
    Army army1;
    army1.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army1.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Army army2;
    army2.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Battle battle;
    BattleLog log = battle.simulateBattle(army1, army2);

    // Print the battle log
    for(const auto& step : log.steps) {
        for(const auto& event : step.events) {
            std::visit([](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, TurnStartedEvent>) {
                    std::cout << "Turn " << arg.turnNumber << " started." << std::endl;
                } else if constexpr (std::is_same_v<T, AttackDeclaredEvent>) {
                    std::cout << "Army " << arg.attackerArmy + 1 << " declared an attack on Army " << arg.targetArmy + 1 
                              << " with attack value " << arg.attackValue << "." << std::endl;
                } else if constexpr (std::is_same_v<T, DamageAppliedEvent>) {
                    std::cout << "Damage applied to Army " << arg.targetArmy + 1 << ": " 
                              << arg.damageDealt << " damage dealt. Health changed from " 
                              << arg.oldHealth << " to " << arg.newHealth << "." << std::endl;
                } else if constexpr (std::is_same_v<T, UnitDiedEvent>) {
                    std::cout << "Unit in Army " << arg.army + 1 << ", slot " << arg.slot + 1 << " died." << std::endl;
                } else if constexpr (std::is_same_v<T, BattleEndedEvent>) {
                    std::string resultStr = (arg.result == BattleResult::Draw) ? "Draw" :
                                            (arg.result == BattleResult::Army1Victory) ? "Army 1 Victory" : "Army 2 Victory";
                    std::cout << "Battle ended with result: " << resultStr << "." << std::endl;
                }
            }, event);
        }
    }   
}

void testBattle2() {
    Army army1;
    army1.addUnit(Unit("Berserker", 5, 3, 1, Brutal));
    army1.addUnit(Unit("Berserker", 5, 3, 1, Brutal));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Army army2;
    army2.addUnit(Unit("Shielder", 5, 2, 2, 0));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Battle battle;
    BattleLog log = battle.simulateBattle(army1, army2);

    // Print the battle log
    for(const auto& step : log.steps) {
        for(const auto& event : step.events) {
            std::visit([](auto&& arg){
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, TurnStartedEvent>) {
                    std::cout << "Turn " << arg.turnNumber << " started." << std::endl;
                } else if constexpr (std::is_same_v<T, AttackDeclaredEvent>) {
                    std::cout << "Army " << arg.attackerArmy + 1 << " declared an attack on Army " << arg.targetArmy + 1 
                              << " with attack value " << arg.attackValue << "." << std::endl;
                } else if constexpr (std::is_same_v<T, DamageAppliedEvent>) {
                    std::cout << "Damage applied to Army " << arg.targetArmy + 1 << ": " 
                              << arg.damageDealt << " damage dealt. Health changed from " 
                              << arg.oldHealth << " to " << arg.newHealth << "." << std::endl;
                } else if constexpr (std::is_same_v<T, UnitDiedEvent>) {
                    std::cout << "Unit in Army " << arg.army + 1 << ", slot " << arg.slot + 1 << " died." << std::endl;
                } else if constexpr (std::is_same_v<T, BattleEndedEvent>) {
                    std::string resultStr = (arg.result == BattleResult::Draw) ? "Draw" :
                                            (arg.result == BattleResult::Army1Victory) ? "Army 1 Victory" : "Army 2 Victory";
                    std::cout << "Battle ended with result: " << resultStr << "." << std::endl;
                }
            }, event);
        }
    }   
}