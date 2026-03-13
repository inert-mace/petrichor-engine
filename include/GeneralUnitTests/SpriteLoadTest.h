// unused unit test
/* #pragma once
#include "Window.h"
#include "Renderer.h"
#include "Sprite.h"

class SpriteLoadTest
{
    public:
        void testSpriteLoading();
};

SpriteLoadTest::testSpriteLoading() {
    Renderer renderer;
    Window window;
    window.init(1280, 720);
    renderer.init(window);

    Sprite sprite;
    Texture knightTexture;
    knightTexture.id = renderer.texture; // use the texture loaded in the renderer for testing
    knightTexture.w = 64; // placeholder width, should be set to actual texture width
    knightTexture.h = 64; // placeholder height, should be set to actual texture height
    sprite.texture = &knightTexture;
    sprite.x = 0.0f;
    sprite.y = 0.0f;

    // render loop for testing
    bool shouldQuit = false;
    while(true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            {
                shouldQuit = true;
                break;
            }
        }
        if(shouldQuit) break;

        renderer.render();
        window.swapBuffers();
    }
} */