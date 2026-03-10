#include "Battle.h"
#include "BattleEvents.h"
#include "Abilities.h"
#include "Army.h"
#include "Unit.h"
#include "BattlePlayback.h"

struct BattleUnitTest
{
    void testBattle1();
    void testBattle2();
};

void BattleUnitTest::testBattle1() {
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
    BattlePlayback playback;
    playback.playBattleLog(log);
}

void BattleUnitTest::testBattle2() {
    Army army1;
    army1.addUnit(Unit("Shielder", 5, 2, 2, 0));
    army1.addUnit(Unit("Shielder", 5, 2, 2, 0));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army1.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Army army2;
    army2.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army2.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army2.addUnit(Unit("Swordsman", 5, 3, 1, 0));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));
    army2.addUnit(Unit("Archer", 5, 2, 0, Ranged));

    Battle battle;
    BattleLog log = battle.simulateBattle(army1, army2);
    BattlePlayback playback;
    playback.playBattleLog(log);
}