#include "ShaderProgram.h"

namespace OpenGLWrapper
{
    ShaderProgram::ShaderProgram()
    {
        this->shader_program_pos = glCreateProgram();
    }

    ShaderProgram::ShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path)
    {
        this->shader_program_pos = glCreateProgram();
        this->init(vertex_shader_path, fragment_shader_path);
    }

    template<class ShaderClass>
    bool ShaderProgram::loadShader(std::string path, ShaderClass** shader_object)
    {
        if (*shader_object != nullptr)
        {
            delete* shader_object;
        }

        *shader_object = new ShaderClass();

        if (!((*shader_object)->create()))
        {
            return false;
        }

        if (!((*shader_object)->compile(path)))
        {
            return false;
        }

        glAttachShader(this->shader_program_pos, (*shader_object)->getPos());

        return true;
    }

    bool ShaderProgram::loadVertexShader(std::string path)
    {
        return this->loadShader<VertexShader>(path, &this->vertex_shader);
    }

    bool ShaderProgram::loadFragmentShader(std::string path)
    {
        return this->loadShader<FragmentShader>(path, &this->fragment_shader);
    }

    bool ShaderProgram::init(std::string vertex_shader_path, std::string fragment_shader_path)
    {
        if (!this->loadVertexShader(vertex_shader_path))
        {
            std::cerr << "[ERROR]: VertexShader init failed!" << std::endl;

            return false;
        }

        if (!this->loadFragmentShader(fragment_shader_path))
        {
            std::cerr << "[ERROR]: FragmentShader init failed!" << std::endl;

            return false;
        }

        const int shaders_count = 2;
        GLsizei shaders_attached = 0;
        GLuint shaders[shaders_count];
        glGetAttachedShaders(this->getPos(), shaders_count, &shaders_attached, shaders);
        if (shaders_attached != shaders_count)
        {
            std::cerr << "[ERROR]: ShaderProgram did not attach correct amount of shaders! Shaders attached: " << shaders_attached << " out of " << shaders_count << std::endl;

            return false;
        }

        glBindAttribLocation(this->shader_program_pos, 0, "vertex_position");

        glLinkProgram(this->shader_program_pos);

        this->deleteShaders();

        GLint status;
        GLchar info_log[512];
        glGetProgramiv(this->getPos(), GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            glGetProgramInfoLog(this->shader_program_pos, 512, nullptr, info_log);
            std::cerr << "[ERROR]: ShaderProgram link failed! Info log: \"" << info_log << "\"" << std::endl;

            return false;
        }

        return true;
    }

    bool ShaderProgram::deleteVertexShader()
    {
        if (this->vertex_shader != nullptr)
        {
            delete this->vertex_shader;

            return true;
        }

        std::cerr << "[WARNING]: Vertex shader could not be deleted (is nullptr)." << std::endl;

        return false;
    }

    bool ShaderProgram::deleteFragmentShader()
    {
        if (this->fragment_shader != nullptr)
        {
            delete this->fragment_shader;

            return true;
        }

        std::cerr << "[WARNING]: Fragment shader could not be deleted (is nullptr)." << std::endl;

        return false;
    }

    bool ShaderProgram::deleteShaders()
    {
        return this->deleteVertexShader() && this->deleteFragmentShader();
    }

    void ShaderProgram::setUniformLocation(std::string uniform_name)
    {
        if (this->uniform_locations.find(uniform_name) == this->uniform_locations.end())
        {
            this->uniform_locations[uniform_name] = glGetUniformLocation(this->shader_program_pos, uniform_name.c_str());
        }
    }

    GLuint ShaderProgram::getPos()
    {
        return this->shader_program_pos;
    }

    void ShaderProgram::setUniformValue(std::string uniform_name, glm::mat4x4 value)
    {
        this->setUniformLocation(uniform_name);
        glUniformMatrix4fv(uniform_locations[uniform_name], 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniformValue(std::string uniform_name, glm::vec4 value)
    {
        this->setUniformLocation(uniform_name);
        glUniform4fv(uniform_locations[uniform_name], 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniformValue(std::string uniform_name, float value)
    {
        this->setUniformLocation(uniform_name);
        glUniform1f(uniform_locations[uniform_name], value);
    }

    void ShaderProgram::setUniformValue(std::string uniform_name, int value)
    {
        this->setUniformLocation(uniform_name);
        glUniform1i(uniform_locations[uniform_name], value);
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(this->shader_program_pos);
    }
}
