#pragma once

class Engine
{
public:
    Engine();
    ~Engine();

    int init();
    void run();
    void clean();
};