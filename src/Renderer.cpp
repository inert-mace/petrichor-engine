#include <iostream>
#include "Renderer.h"
#include <cmath>

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

const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 pos;\n"
"layout (location = 1) in vec2 UV;\n"
"out vec2 FragUV;\n"
"uniform mat4 uProjection;\n"
"uniform mat4 uModel;\n"
"void main()\n"
"{\n"
"   gl_Position = uProjection * uModel * vec4(pos.x, pos.y, pos.z, 1.0);\n"
"   FragUV = UV;\n"
"}\n";

const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"in vec2 FragUV;\n"
"uniform sampler2D uTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(uTexture, FragUV);\n"
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
    for (auto const& [key, textureID] : textures) {
        glDeleteTextures(1, &textureID.id);
    }
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

    // unit square
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,  // top right
    };

    // tells OpenGL how to make triangles out of the vertex data
    GLuint indices[] = {
        0, 1, 2, // first triangle (bottom left, bottom right, top left)
        1, 2, 3  // second triangle (bottom right, top left, top right)
    };

    SDL_Surface* loadedSurface = SDL_LoadPNG("../assets/knight.png");    
    SDL_Surface* knightSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);

    // used to have knightSurface = SDL_ConvertSurface(knightSurface, SDL_PIXELFORMAT_RGBA32)
    // but because ConvertSurface creates a new surface and copies the old one into it, we need to free
    // the original loaded surface first
    SDL_DestroySurface(loadedSurface);


    GLuint texture;
    // generate texture, add its reference to the textures map, and bind texture
    glGenTextures(1, &texture);
    textures.emplace("knight", Texture(knightSurface->w, knightSurface->h, texture));
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

    // orthographic projection matrix for 2D rendering; maps coordinates directly to screen pixels with (0,0) at the top left
    uProjection = glm::ortho(0.0f, static_cast<float>(window.getWidth()), static_cast<float>(window.getHeight()), 0.0f, -1.0f, 1.0f); 
    // keeping model as identity for now since we don't have any transformations yet, but we'll need it later for things like moving sprites around and rotating them
    uModel = glm::mat4(1.0f);

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

    glUseProgram(shaderProgram);
    // setting the projection uniform; we only really need to do this once unless window size changes
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, &uProjection[0][0]);

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
    
    // for testing; creating a sample sprite and then adding it to our spriteList (should live in the engine or ECS or wherever else, not the renderer)
    Sprite sprite = Sprite(632.0f, 344.0f, 1.0f, 1.0f, 1.0f, "knight");

    // adding the sprite to our spritelist
    spriteList.push_back(sprite);
    if(spriteList.size() > 0) std::cout << "Sprite pushed back successfully" << std::endl;
    return 0;
}

void Renderer::render()
{
    // placeholder; will take the first sprite; later implementation will loop through sprite list
    glm::vec3 scalingVector;
    glm::vec3 translationVector;
    std::string key;
    if(spriteList.size() > 0)
    {
        key = spriteList[0].textureKey;
        const Texture& t = textures.at(key);
        float width = (float)t.w;
        float height = (float)t.h;
        scalingVector = glm::vec3(spriteList[0].scaleX*width*pixelScale, spriteList[0].scaleY*height*pixelScale, spriteList[0].scaleZ);

        float snappedX = ((int)(std::round(spriteList[0].x) / pixelScale)) * pixelScale;
        float snappedY = ((int)(std::round(spriteList[0].y) / pixelScale)) * pixelScale;
        translationVector = glm::vec3(snappedX, snappedY, 0.0f);
        // std::cout << "Sprite at (" << snappedX << " x, " << snappedY << " y)" << std::endl;
    }
    else
    {
        key = "knight";
        scalingVector = glm::vec3(1.0f);
        translationVector = glm::vec3(0.0f);
    }

    // clear the screen with a solid color (black in this case)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // using our shader
    glUseProgram(shaderProgram);
    
    // start with identity
    uModel = glm::mat4(1.0f);

    // scale -> rotate -> transform
    // rightmost matrix acts first which is why the following two lines are in that order
    uModel = glm::translate(uModel, translationVector);
    uModel = glm::scale(uModel, scalingVector);

    // set model uniform
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, &uModel[0][0]);

    // using texture unit 0
    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    // using the VAO for vertex attribute state and VBO/EBO bindings
    glBindVertexArray(VAO);

    // even though by default textures are bound to texture unit 0, this is still good practice
    // after all, might be using multiple textures per shader in the future
    glActiveTexture(GL_TEXTURE0);

    // where our funny little knight texture is :)
    glBindTexture(GL_TEXTURE_2D, textures.at(key).id);

    // our wonderful draw call
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // using 6 vertices, draw 2 triangles from bound EBO, with unsigned int indices, starting at offset 0 in EBO
}