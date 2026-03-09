#pragma once
#include "Unit.h"
#include <vector>

class Army
{
    public:
        Army();
        ~Army();

        std::vector<Unit> units;
        int maxSize = 5;

        void addUnit(Unit& unit)
        {
            if(units.size() < maxSize)
            {
                units.push_back(unit);
            }
        }
};