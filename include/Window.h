#pragma once

class Window
{
    public:
        Window();
        ~Window();

        // Creates the SDL window, sets attributes, and creates the OpenGL context
        int init(int width, int height);
        int getWidth() const { return _width; }
        int getHeight() const { return _height; }
    private:
        int _width;
        int _height;
};