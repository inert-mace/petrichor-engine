#pragma once
#include <string>
#include <variant>
#include <vector>

struct BattleStep {
    std::vector<BattleEvent> events;
};

struct BattleLog
{
    std::vector<BattleStep> steps;
    BattleResult result;
};

struct TurnStartedEvent
{
    int turnNumber;
};

struct AttackDeclaredEvent
{
    int attackerArmy;     // 0 or 1
    int attackerSlot;     // usually 0 for frontline
    int targetArmy;
    int targetSlot;       // usually 0 for frontline
    int attackValue;
    bool ranged;
};

struct DamageAppliedEvent
{
    int targetArmy;
    int targetSlot;
    int damageDealt;
    int oldHealth;
    int newHealth;
};

struct UnitDiedEvent
{
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