#include "OpenGLWindow.h"

OpenGLWindow* OpenGLWindow::instance = nullptr;

OpenGLWindow::OpenGLWindow()
{
    OpenGLWindow::instance = this;
}

void OpenGLWindow::init(int* argc, char** argv)
{
    glfwSetErrorCallback(onGlfwError);

    if (!this->initGlfw())
    {
        std::cerr << "[ERROR]: GLFW init failed!" << std::endl;
        return;
    }

    glfwSwapInterval(0);
    //glutReshapeFunc(resize);
    glfwSetKeyCallback(this->window, onGlfwKeyPress);

    if (!this->initShaders())
    {
        std::cerr << "[ERROR]: Shaders init failed!" << std::endl;
        throw std::runtime_error("Shaders init failed!");
    }

    if (!this->initOpenGLSettings())
    {
        std::cerr << "[ERROR]: OpenGL settings init failed!" << std::endl;
        throw std::runtime_error("OpenGL settings init failed!");
    }

    if (!this->initSoundEngine())
    {
        std::cerr << "[ERROR]: IrrKlang Sound Engine init failed!" << std::endl;
        throw std::runtime_error("IrrKlang Sound Engine init failed!");
    }

    if (!this->initFontRenderer())
    {
        std::cerr << "[ERROR]: FontRenderer init failed!" << std::endl;
        throw std::runtime_error("FontRenderer init failed!");
    }

    if (!this->initWindowIcon())
    {
        std::cerr << "[ERROR]: Window icon init failed!" << std::endl;
    }

    this->tetris = std::make_unique<Tetris::Tetris>(this->window_width, this->window_height, this->sound_engine, this->resources_path + this->sounds_sub_path, this->font_renderer);
    this->setUniformValue("tetris_board_top_boundary", this->window_height - this->tetris->getBoardTopBoundary() - 1.0f);   //height - boundary - 1px

    this->sound_engine->setSoundVolume(this->sound_volume);

    this->mainLoop();
}

OpenGLWindow::~OpenGLWindow()
{
    glfwDestroyWindow(this->window);

    if (this->sound_engine != nullptr)
    {
        this->sound_engine->drop();
    }

    glfwTerminate();
}

bool OpenGLWindow::initGlfw()
{
    if (!glfwInit())
    {
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->opengl_major_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->opengl_minor_version);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
    int major, minor, revision;
    glfwGetVersion(&major, &minor, &revision);
    std::cout << "OpenGL version used: " << (int)major << "." << (int)minor << "." << (int)revision << std::endl;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(this->window_width, this->window_height, this->window_name.c_str(), NULL, NULL);
    if (this->window == nullptr)
    {
        std::cerr << "[ERROR]: GLFW window creation failed!" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(this->window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "[ERROR]: GLEW init failed!" << std::endl;
        return false;
    }

    return true;
}

bool OpenGLWindow::initShaders()
{
    this->shader_program = std::make_shared<OpenGLWrapper::ShaderProgram>();
    if (this->shader_program->init(this->resources_path + "vertex_shader.vert", this->resources_path + "fragment_shader.frag"))
    {
        glUseProgram(this->shader_program->getPos());

        return true;
    }

    return false;
}

bool OpenGLWindow::initOpenGLSettings()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    //glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);

    updateProjectionMatrix();
    //this->shader_program->setUniformValue("projection_matrix", glm::mat4x4(1.0f));
    this->shader_program->setUniformValue("model_matrix", glm::mat4x4(1.0f));
    this->shader_program->setUniformValue("color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    this->shader_program->setUniformValue("cut_off", 0);
    this->shader_program->setUniformValue("is_block", 0);
    this->shader_program->setUniformValue("tetris_board_top_boundary", 0.0f);
    this->shader_program->setUniformValue("is_ghost", 0);
    this->shader_program->setUniformValue("inverse", 0);
    this->shader_program->setUniformValue("textured", 0);
    this->shader_program->setUniformValue("tex1", 0);

    return true;
}

bool OpenGLWindow::initSoundEngine()
{
    this->sound_engine = irrklang::createIrrKlangDevice();

    if (this->sound_engine == nullptr)
    {
        return false;
    }

    return true;
}

bool OpenGLWindow::initFontRenderer()
{
    if (!this->font_texture.create())
    {
        return false;
    }

    this->font_texture.bind();
    this->font_texture.setSize(128, 128);
    this->font_texture.setPixelInfo(GL_RGBA8, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV);

    std::ifstream texture_stream(this->resources_path + "font_128x128.data", std::ios::binary);
    if (!texture_stream)
    {
        return false;
    }

    this->font_texture_data = std::vector<unsigned char>(std::istreambuf_iterator<char>(texture_stream), {});
    this->font_texture.setData(0, this->font_texture_data);

    this->font_renderer = std::make_shared<FontRenderer>();

    if (!this->font_renderer->init(std::make_shared<OpenGLWrapper::TextureObject>(this->font_texture)))
    {
        return false;
    }

    return true;
}

bool OpenGLWindow::initWindowIcon()
{
    GLFWimage icons[5] = {
        {8, 8, nullptr},
        {16, 16, nullptr },
        {32, 32, nullptr },
        {64, 64, nullptr },
        {128, 128, nullptr },
    };

    std::ifstream ico_8_stream(this->resources_path + "tetris_icon_8.data", std::ios::binary);
    std::ifstream ico_16_stream(this->resources_path + "tetris_icon_16.data", std::ios::binary);
    std::ifstream ico_32_stream(this->resources_path + "tetris_icon_32.data", std::ios::binary);
    std::ifstream ico_64_stream(this->resources_path + "tetris_icon_64.data", std::ios::binary);
    std::ifstream ico_128_stream(this->resources_path + "tetris_icon_128.data", std::ios::binary);

    if (!ico_8_stream || !ico_16_stream || !ico_32_stream || !ico_64_stream || !ico_128_stream)
    {
        return false;
    }

    std::vector<unsigned char> ico_8_data(std::istreambuf_iterator<char>(ico_8_stream), {});
    std::vector<unsigned char> ico_16_data(std::istreambuf_iterator<char>(ico_16_stream), {});
    std::vector<unsigned char> ico_32_data(std::istreambuf_iterator<char>(ico_32_stream), {});
    std::vector<unsigned char> ico_64_data(std::istreambuf_iterator<char>(ico_64_stream), {});
    std::vector<unsigned char> ico_128_data(std::istreambuf_iterator<char>(ico_128_stream), {});

    icons[0].pixels = ico_8_data.data();
    icons[1].pixels = ico_16_data.data();
    icons[2].pixels = ico_32_data.data();
    icons[3].pixels = ico_64_data.data();
    icons[4].pixels = ico_128_data.data();

    glfwSetWindowIcon(this->window, 5, icons);

    return true;
}

void OpenGLWindow::updateProjectionMatrix()
{
    glViewport(0, 0, this->window_width, this->window_height);
    glm::mat4x4 projection_matrix = glm::ortho(0.0f, (float)this->window_width, (float)this->window_height, 0.0f, -1.0f, 1.0f);
    this->setUniformValue("projection_matrix", projection_matrix);
}

void OpenGLWindow::onGlfwError(int error, const char* description)
{
    std::cerr << "[ERROR][GLFW]: " << error << " - " << description << std::endl;
}

void OpenGLWindow::mainLoop()
{
    using namespace std::chrono_literals;
    const double hz = 1.0 / this->target_fps;
    std::chrono::duration<double, std::milli> hz_d(hz);
    
    while (!glfwWindowShouldClose(this->window))
    {
        clock::time_point now = clock::now();
        fsec diff = now - this->last_render;

        //glfwPollEvents();
        this->tetris->process();

        if (diff.count() < hz)
        {
            continue;
        }

        this->last_render = now;

        this->render();

        glfwSwapBuffers(this->window);
        //glFlush();    //glfwSwapBuffers implies glFlush, no need to call it manually
    }

    this->tetris->stop();
}

void OpenGLWindow::render()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->tetris->draw(this->shader_program);

    this->font_renderer->renderText(this->shader_program, "MADE BY " + *FontRenderer::SpecialSymbol::IDYLOS, { this->window_width - 5, this->window_height - 5 }, 1.0f, { 1, 1, 1, 1 }, FontRenderer::TextAlignment::BOTTOM_RIGHT);
}

void OpenGLWindow::onGlfwKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //std::cout << "key: " << key << ", scancode: " << scancode << ", action: " << action << ", mods: " << mods << std::endl;

    OpenGLWindow* _this = OpenGLWindow::instance;

    if (action == GLFW_PRESS)
    {
        _this->tetris->handleKeyPress(scancode);
    }
    else if (action == GLFW_RELEASE)
    {
        _this->tetris->handleKeyRelease(scancode);
    }
    else    //GLFW_REPEAT
    {
        //don't care
    }
}
