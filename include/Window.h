#pragma once

class Window
{
    public:
        Window();
        ~Window();

        // Creates the SDL window and initializes any necessary resources
        int init();
};