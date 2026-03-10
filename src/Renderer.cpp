#include <iostream>
#include "Renderer.h"
#include "glm/glm.hpp"

/*const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n";
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.2f, 0.8f, 0.5f, 1.0f);\n"
"}\n";*/

// upgraded from first shader; vertexes now have UV coordinates which correspond to texture coordinates
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aUV;\n"
"out vec2 vUV;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vUV = aUV;\n"
"}\n";

// upgraded from first shader; fragment shader now samples from a texture using the UV coordinates passed from the vertex shader
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"in vec2 vUV;\n"
"uniform sampler2D uTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(uTexture, vUV);\n"
"}\n";

Renderer::Renderer()
{
    std::cout << "Renderer constructor called" << std::endl;
}

Renderer::~Renderer()
{
    std::cout << "Renderer destructor called" << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture);
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

    // a bunch of points in an array that define vertices for our shape
    // this one describes four vertices with UVs and 3D coords
    // we'll use this as two tris by defining an index buffer next
    GLfloat vertices[] = {
        -0.1125f, -0.2f, 0.0f, 0.0f, 1.0f, // bottom left
        0.1225f, -0.2f, 0.0f, 1.0f, 1.0f, // bottom right
        -0.1125f, 0.6f, 0.0f, 0.0f, 0.0f,  // top left
        0.1225f, 0.6f, 0.0f, 1.0f, 0.0f,  // top right
    };

    // tells OpenGL how to make triangles out of the vertex data
    GLuint indices[] = {
        0, 1, 2, // first triangle (bottom left, bottom right, top left)
        1, 2, 3  // second triangle (bottom right, top left, top right)
    };

    SDL_Surface* knightSurface = SDL_LoadPNG("../assets/knight.png");
    knightSurface = SDL_ConvertSurface(knightSurface, SDL_PIXELFORMAT_RGBA32); // convert surface to RGBA format since that's what OpenGL expects; this also ensures the surface has an alpha channel for transparency if it didn't already

    // generate and bind texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // used to specify how pixel data is stored in memory
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // copy pixel data from SDL surface to OpenGL texture; GL_RGBA8 means each pixel has red, green, blue, and alpha channels, and GL_UNSIGNED_BYTE means each channel is an unsigned byte (0-255)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, knightSurface->w, knightSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, knightSurface->pixels);

    // what opengl will do when minifying and magnifying the texture
    // linear interpolates between 4 closest pixels, nearest uses closest pixel (better for pixel art)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_DestroySurface(knightSurface); // we can free the surface after copying its data to the GPU

    // compile and link shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // turns source code into shader object, which can then be linked into a shader program; the 1 is the number of strings in the array (we only have one string), and the nullptr is an optional array of string lengths (if nullptr, strings are assumed to be null-terminated)
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    // compiles source code into machine code that can be executed by the GPU; we link after this to create a shader program that can be used for rendering
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // create shader program; shaderProgram is the handle to said program (it's a reference, like basically everything else in OpenGL with the GLuint type)
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // these are already in the shader program, so we can delete these; their compiled machine code in the shader program won't be affected
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // time for VAO and VBO; since sending stuff from the CPU to the GPU is slow, we put data like vertex data into a VBO (vertex buffer object) stored in GPU memory
    // VAO (vertex array object) stores state needed to specify vertex data, like attribute pointers and which VBOs to use
    // also need an EBO (element buffer object) to store our index data for indexed drawing, which lets us reuse vertices for multiple triangles without duplicating vertex data

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // set current VAO to the one we just generated, so that subsequent vertex attribute calls will be stored in this VAO
    glBindVertexArray(VAO);
    // set current VBO to the one we just generated, so that subsequent buffer calls will use this VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copy vertex data into the VBO; GL_ARRAY_BUFFER is the type of buffer we're copying to
    // we use sizeof(vertices) to set aside enough space for all the vertex data, then pass in our actual vertex data
    // STREAM: modified once, used a few times; STATIC: modified a few times, used many times; DYNAMIC: modified many times, used many times 
    // DRAW: for drawing, COPY: for copying, READ: for reading
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // enables the vertex attribute at index 0, tells OpenGL that there are 5 values per vertex (floats x y z u v), 
    // that the data is tightly packed (so it can move to the next vertex by just moving 5 floats forward in the buffer), 
    // and that the position data starts at the beginning of the buffer (offset of 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // for UV coords we use location 1 (defined in our vertex shader) to point to the UV data/the UV's vec2
    // still use 5*sizeof(float) for the stride since each vertex still has 5 floats total, but the offset is now 3 floats instead of 0 since the UV data comes after the position data in our vertex array
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // UV data starts after the first 3 floats (x y z), so offset is 3 floats
    glEnableVertexAttribArray(1);

    // set current EBO to the one we just generated, so that subsequent element buffer calls will use this EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    

    std::cout << "Vertex shader and fragment shader compiled and linked into shader program with ID " << shaderProgram << std::endl;
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO to prevent accidental changes to it
    glBindVertexArray(0); // unbind VAO to prevent accidental changes to it
    
    return 0;
}

void Renderer::render()
{
    // clear the screen with a solid color (black in this case)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // using 6 vertices, draw 2 triangles from bound EBO, with unsigned int indices, starting at offset 0 in EBO
}