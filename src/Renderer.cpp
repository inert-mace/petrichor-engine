#include <iostream>
#include "Renderer.h"
#include <cmath>
#include <algorithm>

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

// old dissolve logic
/*
if(dissolve && (FragColor.a > 0.001))\n"
"   {\n"
"       ivec2 spriteTexel = ivec2(floor(uvs * vec2(spriteTextureSize)));\n"
"       spriteTexel = clamp(spriteTexel, ivec2(0), spriteTextureSize - ivec2(1));\n"
"       ivec2 maskTexel = spriteTexel + maskOffset;\n"
"       maskTexel = clamp(maskTexel, ivec2(0), dissolveMaskSize - ivec2(1));"
"       vec4 sampledMask = texelFetch(uDissolveMask, maskTexel, 0);\n"
"       if(sampledMask.r <= dissolveProgress)\n"
"       {\n"
"           discard;\n"
"       }\n"
"   }\n"
*/

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
"uniform sampler2D uDissolveMask;\n"
"uniform bool animated;\n"
"uniform bool dissolve;\n"
"uniform vec3 ambientColor;\n"
"uniform vec3 burnColor;\n"
"uniform float ambientColorIntensity;\n"
"uniform float dissolveProgress;\n"
"uniform float burnBand;\n"
"uniform ivec2 spriteTextureSize;\n"
"uniform ivec2 dissolveMaskSize;\n"
"uniform int frame;\n"
"void main()\n"
"{\n"
"   vec2 sampleUVs = FragUV;\n"
"   vec2 maskUVs = FragUV;\n"
"   if(animated)\n"
"   {\n"
"       sampleUVs = vec2((FragUV.x/4)+((1.0/4.0)*float(frame)), FragUV.y);\n"
"   }\n"
"   vec4 base = texture(uTexture, sampleUVs);\n"
"   vec3 overlay = ambientColor*ambientColorIntensity;\n"
"   base = vec4(base.r + overlay.r, base.g + overlay.g, base.b + overlay.b, base.a);\n"
"   FragColor = clamp(base, 0.0, 1.0);\n"
"   if(dissolve && (FragColor.a > 0.001))\n"
"   {\n"
"       ivec2 spriteTexel = ivec2(floor(maskUVs * vec2(spriteTextureSize)));\n"
"       spriteTexel = clamp(spriteTexel, ivec2(0), spriteTextureSize - ivec2(1));\n"
"       vec2 snappedMaskUV = (vec2(spriteTexel) + vec2(0.5)) / vec2(spriteTextureSize);\n"
"       ivec2 maskTexel = ivec2(floor(snappedMaskUV * vec2(dissolveMaskSize)));\n"
"       maskTexel = clamp(maskTexel, ivec2(0), dissolveMaskSize - ivec2(1));\n"
"       vec4 sampledMask = texelFetch(uDissolveMask, maskTexel, 0);\n"
"       if(sampledMask.r <= dissolveProgress)\n"
"       {\n"
"           discard;\n"
"       }\n"
"       if(sampledMask.r <= dissolveProgress + burnBand)\n"
"       {\n"
"           float bandFactor = 1.0 - smoothstep(0.0, burnBand, (sampledMask.r - dissolveProgress));\n"
"           FragColor.rgb = mix(FragColor.rgb, burnColor, bandFactor);\n"
"       }\n"
"   }\n"
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

    // enables depth-testing for pixels on screen; disabled in favor of sorting by z-value
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

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

    // setting the ambient color and intensity
    setAmbientColor(34, 32, 52);
    setAmbientIntensity(0.25f);

    loadTextures();

    // orthographic projection matrix for 2D rendering; maps coordinates directly to screen pixels with (0,0) at the top left
    uProjection = glm::ortho(0.0f, static_cast<float>(window.getWidth()), static_cast<float>(window.getHeight()), 0.0f, -1.0f, 1.0f); 
    // keeping model as identity for now since we don't have any transformations yet, but we'll need it later for things like moving sprites around and rotating them
    uModel = glm::mat4(1.0f);

    buildPipeline();

    glUseProgram(shaderProgram);
    // setting the projection uniform; we only really need to do this once unless window size changes
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, &uProjection[0][0]);

    // setting dissolve mask texture unit
    glUniform1i(glGetUniformLocation(shaderProgram, "uDissolveMask"), 1);
    
    createBufferObjects(vertices, sizeof(vertices), indices, sizeof(indices));
    
    addSprites();

    cacheULs();

    return 0;
}

// overload: static texture
void Renderer::loadTexture(const std::string& filePath, const std::string& key, GLint horizontalWrapMode, GLint verticalWrapMode) {
    if(textures.find(key) != textures.end()) {
        std::cerr << "Key already exists! Skipping load." << std::endl;
        return;
    }
    SDL_Surface* loadedSurface = SDL_LoadPNG(filePath.c_str());
    if(!loadedSurface) {
        throw std::runtime_error(std::string("Failed to load texture from file path:") + SDL_GetError());
    }
    SDL_Surface* textureSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
    if(!textureSurface) {
        // making sure to destroy loadedSurface here so it doesn't leak if conversion fails
        SDL_DestroySurface(loadedSurface);
        throw std::runtime_error(std::string("Failed to convert surface to format:") + SDL_GetError());
    }
    
    // SDL_ConvertSurface creates a new surface and copies the converted surface onto it, so we need to free the old surface
    SDL_DestroySurface(loadedSurface);

    // here we get a GLuint to store a handle, which we then generate and store (the reference to) a texture in
    GLuint texture;
    glGenTextures(1, &texture);

    // putting the texture handle into our asset map
    textures.emplace(key, Texture(textureSurface->w, textureSurface->h, texture));

    // sets texture to be the active texture (commands apply to this texture; this texture is stored in the active texture unit)
    glBindTexture(GL_TEXTURE_2D, texture);

    // used to specify how pixel data is stored in memory
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // copy pixel data from SDL surface to OpenGL texture; GL_RGBA means each pixel has red, green, blue, and alpha channels, and GL_UNSIGNED_BYTE means each channel is an unsigned byte (0-255)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureSurface->w, textureSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureSurface->pixels);

    // what opengl will do when minifying and magnifying the texture
    // linear interpolates between 4 closest pixels, nearest uses closest pixel (better for pixel art)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_DestroySurface(textureSurface);
}

// overload: animated texture
void Renderer::loadTexture(const std::string& filePath, const std::string& key, GLint horizontalWrapMode, GLint verticalWrapMode, int frameX, int frameY, int frameCount, bool animated) {
    if(textures.find(key) != textures.end()) {
        std::cerr << "Key already exists! Skipping load." << std::endl;
        return;
    }
    SDL_Surface* loadedSurface = SDL_LoadPNG(filePath.c_str());
    if(!loadedSurface) {
        throw std::runtime_error(std::string("Failed to load texture from file path:") + SDL_GetError());
    }
    SDL_Surface* textureSurface = SDL_ConvertSurface(loadedSurface, SDL_PIXELFORMAT_RGBA32);
    if(!textureSurface) {
        // making sure to destroy loadedSurface here so it doesn't leak if conversion fails
        SDL_DestroySurface(loadedSurface);
        throw std::runtime_error(std::string("Failed to convert surface to format:") + SDL_GetError());
    }

    // SDL_ConvertSurface creates a new surface and copies the converted surface onto it, so we need to free the old surface
    SDL_DestroySurface(loadedSurface);

    // here we get a GLuint to store a handle, which we then generate and store (the reference to) a texture in
    GLuint texture;
    glGenTextures(1, &texture);

    // putting the texture handle into our asset map
    textures.emplace(key, Texture(textureSurface->w, textureSurface->h, frameX, frameY, frameCount, animated, texture));

    // sets texture to be the active texture (commands apply to this texture; this texture is stored in the active texture unit)
    glBindTexture(GL_TEXTURE_2D, texture);

    // used to specify how pixel data is stored in memory
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // copy pixel data from SDL surface to OpenGL texture; GL_RGBA means each pixel has red, green, blue, and alpha channels, and GL_UNSIGNED_BYTE means each channel is an unsigned byte (0-255)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureSurface->w, textureSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureSurface->pixels);

    // what opengl will do when minifying and magnifying the texture
    // linear interpolates between 4 closest pixels, nearest uses closest pixel (better for pixel art)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, horizontalWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, verticalWrapMode);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    SDL_DestroySurface(textureSurface);
}

void Renderer::render()
{
    // clear the screen with a solid color (black in this case)
    glClearColor(ambientColor.r+ambientColor.r*ambientColorIntensity, ambientColor.g+ambientColor.g*ambientColorIntensity, ambientColor.b+ambientColor.b*ambientColorIntensity, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // using our shader
    glUseProgram(shaderProgram);

    // sorting if render order is marked dirty
    if(renderOrderDirty) {
        std::sort(spriteList.begin(), spriteList.end(), [](const Sprite& a, const Sprite& b) { return a.z > b.z; });
        renderOrderDirty = false;
    }
    for(size_t i = 0; i < spriteList.size(); i++) {

        glm::vec3 scalingVector;
        glm::vec3 translationVector;
        std::string key;

        key = spriteList[i].textureKey;
        const Texture& t = textures.at(key);
        float width = (float)t.w;
        float height = (float)t.h;
        scalingVector = glm::vec3(spriteList[i].scaleX*width*pixelScale, spriteList[i].scaleY*height*pixelScale, spriteList[i].scaleZ);
        float snappedX = ((int)(std::round(spriteList[i].x) / pixelScale)) * pixelScale;
        float snappedY = ((int)(std::round(spriteList[i].y) / pixelScale)) * pixelScale;
        translationVector = glm::vec3(snappedX, snappedY, spriteList[i].z);
    
        // start with identity
        uModel = glm::mat4(1.0f);

        // scale -> rotate -> transform
        // rightmost matrix acts first which is why the following two lines are in that order
        uModel = glm::translate(uModel, translationVector);
        uModel = glm::scale(uModel, scalingVector);

        // set model uniform
        glUniformMatrix4fv(UL_uModel, 1, GL_FALSE, &uModel[0][0]);

        // using texture unit 0
        glUniform1i(UL_uTexture, 0);

        // get animation info from sprite
        glUniform1i(UL_animated, textures.at(key).animated);
        glUniform1i(UL_frame, textures.at(key).currFrame-1);

        // get dissolve info
        glUniform1i(UL_dissolve, spriteList[i].dissolve);
        glUniform1f(UL_dissolveProgress, spriteList[i].dissolveProgress);
        glUniform2i(UL_dissolveMaskSize, textures.at("dissolve").w, textures.at("dissolve").h);

        // get dissolve burn info
        glUniform1f(UL_burnBand, burnBand);
        glUniform3f(UL_burnColor, burnColor.r, burnColor.g, burnColor.b);

        // sends proper width and height (full texture if static, frame if animated) for dissolve
        int spritePixelWidth = t.animated ? t.frameW : t.w;
        int spritePixelHeight = t.animated ? t.frameH : t.h;
        glUniform2i(UL_spriteTextureSize, spritePixelWidth, spritePixelHeight);

        // send ambient color info
        glUniform3f(UL_ambientColor, ambientColor.r, ambientColor.g, ambientColor.b);
        glUniform1f(UL_ambientColorIntensity, ambientColorIntensity);

        // using the VAO for vertex attribute state and VBO/EBO bindings
        glBindVertexArray(VAO);

        // activate the texture unit that the texture wants to bind to
        glActiveTexture(GL_TEXTURE0);

        // where our texture is :)
        glBindTexture(GL_TEXTURE_2D, textures.at(key).id);

        // our wonderful draw call
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // using 6 vertices, draw 2 triangles from bound EBO, with unsigned int indices, starting at offset 0 in EBO
    }
}

void Renderer::setAmbientColor(int r, int g, int b) {
    ambientColor = glm::vec3(r/255.0f, g/255.0f, b/255.0f);
}

void Renderer::setAmbientIntensity(float i) {
    ambientColorIntensity = i;
}

void Renderer::cacheULs() {
    if(shaderProgram)
    {
        UL_uModel = glGetUniformLocation(shaderProgram, "uModel");
        UL_uTexture = glGetUniformLocation(shaderProgram, "uTexture");
        UL_animated = glGetUniformLocation(shaderProgram, "animated");
        UL_frame = glGetUniformLocation(shaderProgram, "frame");
        UL_dissolve = glGetUniformLocation(shaderProgram, "dissolve");
        UL_dissolveProgress = glGetUniformLocation(shaderProgram, "dissolveProgress");
        UL_dissolveMaskSize = glGetUniformLocation(shaderProgram, "dissolveMaskSize");
        UL_spriteTextureSize = glGetUniformLocation(shaderProgram, "spriteTextureSize");
        UL_ambientColor = glGetUniformLocation(shaderProgram, "ambientColor");
        UL_ambientColorIntensity = glGetUniformLocation(shaderProgram, "ambientColorIntensity");
        UL_burnColor = glGetUniformLocation(shaderProgram, "burnColor");
        UL_burnBand = glGetUniformLocation(shaderProgram, "burnBand");
    }
}

void Renderer::addSprites() {
    // this stuff should live in the engine or ECS or wherever else, not the renderer
    Sprite sprite = Sprite(632.0f, 344.0f, 0.5f, 1.0f, 1.0f, 1.0f, "knight", false);
    Sprite sprite2 = Sprite(400.0f, 344.0f, 0.5f, 1.0f, 1.0f, 1.0f, "hunter", false);
    Sprite sprite3 = Sprite(800.0f, 344.0f, 0.5f, 1.0f, 1.0f, 1.0f, "wizard", false);
    Sprite sprite4 = Sprite(632.0f, 344.0f, 0.0f, 1.0f, 1.0f, 1.0f, "slash", false);
    //Sprite background = Sprite(570.0f, 328.0f, 1.0f, 1.0f, 1.0f, 1.0f, "arena_mountain", false);
    Sprite titleScreen = Sprite(640.0f, 360.0f, 1.0f, 1.0f, 1.0f, 1.0f, "title", false);

    // adding sprites to our spritelist
    spriteList.push_back(sprite);
    spriteList.push_back(sprite2);
    spriteList.push_back(sprite3);
    spriteList.push_back(sprite4);
    spriteList.push_back(titleScreen);
    renderOrderDirty = true;
}

void Renderer::loadTextures() {
    loadTexture("../assets/knight.png", "knight", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    loadTexture("../assets/hunter.png", "hunter", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    loadTexture("../assets/wizard.png", "wizard", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    loadTexture("../assets/slashanim.png", "slash", GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, 16, 16, 4, true);
    loadTexture("../assets/dissolve_gradient.png", "dissolve", GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE);
    loadTexture("../assets/arena_mountain.png", "arena_mountain", GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE);
    loadTexture("../assets/title_screen.png", "title", GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE);
    
    // binding dissolve to texture unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures.at("dissolve").id);
}

void Renderer::buildPipeline() {
    // compile and link shaders
    GLint success;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // turns source code into shader object, which can then be linked into a shader program; the 1 is the number of strings in the array (we only have one string), and the nullptr is an optional array of string lengths (if nullptr, strings are assumed to be null-terminated)
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

    // compiles source code into machine code that can be executed by the GPU; we link after this to create a shader program that can be used for rendering
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        throw std::runtime_error(std::string("Vertex shader failed to compile!"));
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        throw std::runtime_error(std::string("Fragment shader failed to compile!"));
    }

    // create shader program; shaderProgram is the handle to said program (it's a reference, like basically everything else in OpenGL with the GLuint type)
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // these are already in the shader program, so we can delete these; their compiled machine code in the shader program won't be affected
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    std::cout << "Vertex shader and fragment shader compiled and linked into shader program with ID " << shaderProgram << std::endl;
}

void Renderer::createBufferObjects(const GLfloat* vertices, size_t vertexBytes, const GLuint* indices, size_t indexBytes) {
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
    glBufferData(GL_ARRAY_BUFFER, vertexBytes, vertices, GL_STATIC_DRAW);

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO to prevent accidental changes to it
    glBindVertexArray(0); // unbind VAO to prevent accidental changes to it
}