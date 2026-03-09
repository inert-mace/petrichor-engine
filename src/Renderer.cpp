#include <iostream>
#include "Renderer.h"

Renderer::Renderer()
{
    std::cout << "Renderer constructor called" << std::endl;
}

Renderer::~Renderer()
{
    std::cout << "Renderer destructor called" << std::endl;
}

int Renderer::init()
{
    std::cout << "Renderer init called" << std::endl;
    return 0;
}