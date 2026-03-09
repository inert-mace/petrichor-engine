#include <iostream>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
// #include "SDL3_ttf/SDL_ttf.h"
#include "glad/glad.h"

int main()
{
    std::cout << "Hello world!" << std::endl;

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    return 0;
}