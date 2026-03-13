#include <iostream>
#include "4XTest/BattleUnitTest.h"
#include "Engine.h"

int main()
{
    std::cout << "Hello world!" << std::endl;

    BattleUnitTest test;
    test.testBattle1();
    std::cout << "Enter any button to continue..." << std::endl;
    std::cin.get();

    test.testBattle2();
    std::cout << "Testing complete, running engine" << std::endl;
    std::cin.get(); // wait for user input before closing the console window
    {
        Engine engine;
        engine.run();
    } // ensures engine goes out of scope and calls all of its destructors before SDL_Quit()
    SDL_Quit();
    
    return 0;
}