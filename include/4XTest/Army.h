#pragma once
#include "Unit.h"
#include <vector>

class Army
{
    public:
        std::vector<Unit> units;
        int maxSize = 5;

        // Adds a unit to the army if there is space
        void addUnit(const Unit& unit)
        {
            if(units.size() < maxSize)
            {
                // copies the parameter unit into the army's units vector
                // while the parameter is const, the copy in the vector is not const and can be modified by the battle simulation
                units.push_back(unit);
            }
        }
};