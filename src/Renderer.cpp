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

int Renderer::init(Window& window)
{
    std::cout << "Renderer init called" << std::endl;

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set the viewport to match the window size
    glViewport(0, 0, window.getWidth(), window.getHeight());

    // enables blending for transparency; without this alpha blending won't work and textures with transparency will render with black backgrounds
    glEnable(GL_BLEND);

    // sets the blending function for the above (how the source and destination colors are combined)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return 0;
}

void Renderer::render()
{
    // clear the screen with a solid color (black in this case)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // swap the front and back buffers to display the rendered frame
    SDL_GL_SwapWindow(SDL_GL_GetCurrentWindow());
    // Note: In a more complete implementation, you would also render your game objects here before swapping the buffers
}