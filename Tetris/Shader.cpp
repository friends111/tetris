#include "Shader.h"

namespace OpenGLWrapper
{
    Shader::Shader(GLenum shader_type) : shader_type(shader_type)
    {
    }

    bool Shader::create()
    {
        this->shader_pos = glCreateShader(this->shader_type);
        if (this->shader_pos == 0)
        {
            std::cerr << "[ERROR]: Shader creation failed!" << std::endl;

            return false;
        }

        return true;
    }

    bool Shader::compile(std::string shader_path)
    {
        std::ifstream shader_file(shader_path);
        if (!shader_file)
        {
            std::cerr << "[ERROR]: Shader compilation failed! Shader file \"" + shader_path + "\" does not exist." << std::endl;

            return false;
        }

        std::string shader_str((std::istreambuf_iterator<char>(shader_file)), std::istreambuf_iterator<char>());
        this->source = shader_str.c_str();

        glShaderSource(this->shader_pos, 1, &this->source, NULL);

        glCompileShader(this->shader_pos);

        GLint status;
        GLchar info_log[512];
        glGetShaderiv(this->shader_pos, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            glGetShaderInfoLog(this->shader_pos, 512, NULL, info_log);
            std::cerr << "[ERROR]: Shader compilation failed! Info log: \"" << info_log << "\"" << std::endl;

            return false;
        }

        this->compiled = true;

        return true;
    }

    bool Shader::isCompiled()
    {
        return this->compiled;
    }

    GLuint Shader::getPos()
    {
        return this->shader_pos;
    }

    const char* Shader::getSource()
    {
        return this->source;
    }

    Shader::~Shader()
    {
        glDeleteShader(this->shader_pos);
    }
}
