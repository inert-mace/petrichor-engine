#pragma once
#include <string>
#include <variant>
#include <vector>

struct TurnStartedEvent
{
    int turnNumber;
};

struct AttackDeclaredEvent
{
    std::string attackingUnitName;
    std::string targetUnitName;
    int attackerArmy;     // 0 or 1
    int attackerSlot;     // usually 0 for frontline
    int targetArmy;       // 0 or 1, (usually) opposite of attackerArmy
    int targetSlot;       // usually 0 for frontline
    int attackValue;
    bool ranged;
};

struct DamageAppliedEvent
{
    std::string targetUnitName;
    int targetArmy;
    int targetSlot;
    int damageDealt;
    int oldHealth;
    int newHealth;
};

struct UnitDiedEvent
{
    std::string deadUnitName;
    int army;
    int slot;
};

enum class BattleResult
{
    Draw,
    Army1Victory,
    Army2Victory
};

struct BattleEndedEvent
{
    BattleResult result;
};

using BattleEvent = std::variant<TurnStartedEvent, AttackDeclaredEvent, DamageAppliedEvent, UnitDiedEvent, BattleEndedEvent>;

struct BattleStep {
    std::vector<BattleEvent> events;
};

struct BattleLog
{
    std::vector<BattleStep> steps;
    BattleResult result;
};