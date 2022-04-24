#pragma once
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include "Triangle.h"
#include "Tetris.h"
#include <irrKlang.h>
#include "TextureObject.h"
#include "TexturedQuadrilateral.h"
#include "FontRenderer.h"

class OpenGLWindow
{
	using clock = std::chrono::high_resolution_clock;
	using fsec = std::chrono::duration<float>;
public:
	OpenGLWindow();
	void init(int* argc = nullptr, char** argv = nullptr);
	static OpenGLWindow* instance;
	~OpenGLWindow();
private:
	irrklang::ISoundEngine* sound_engine = nullptr;	//irrklang conflicts with shared_ptr etc. memory handling, so raw pointer it is
	std::shared_ptr<ShaderProgram> shader_program;
	std::shared_ptr<FontRenderer> font_renderer;
	TextureObject font_texture;
	std::vector<unsigned char> font_texture_data;
	const int window_width = 1024;
	const int window_height = 768;
	const int opengl_major_version = 3;
	const int opengl_minor_version = 3;
	const double target_fps = 70.0;
	const float sound_volume = 0.03f;
	clock::time_point last_render;
	const std::string window_name = "Tetris";
	const std::string resources_path = "resources/";
	const std::string sounds_sub_path = "sounds/";
	GLFWwindow* window = nullptr;
	std::shared_ptr<Tetris::Tetris> tetris = nullptr;
	bool initGlfw();
	bool initShaders();
	bool initOpenGLSettings();
	bool initSoundEngine();
	bool initFontRenderer();
	bool initWindowIcon();
	template<class T> void setUniformValue(std::string uniform_name, T value);
	void updateProjectionMatrix();
	static void onGlfwError(int error, const char* description);
	void mainLoop();
	void logicLoop();
	void render();
	static void onGlfwKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
};

template<class T>
inline void OpenGLWindow::setUniformValue(std::string uniform_name, T value)
{
	if (this->shader_program != nullptr)
	{
		this->shader_program->setUniformValue(uniform_name, value);
	}
}
