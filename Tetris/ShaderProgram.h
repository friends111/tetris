#pragma once
#include <string>
#include "VertexShader.h"
#include "FragmentShader.h"
#include <GL/glew.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace OpenGLWrapper
{
    class ShaderProgram
    {
    private:
        VertexShader* vertex_shader = nullptr;
        FragmentShader* fragment_shader = nullptr;
        GLuint shader_program_pos = 0;
        std::map<std::string, int> uniform_locations;
        bool deleteVertexShader();
        bool deleteFragmentShader();
        bool deleteShaders();
        void setUniformLocation(std::string uniform_name);
    public:
        ShaderProgram();
        ShaderProgram(std::string vertex_shader_path, std::string fragment_shader_path);
        template<class ShaderClass> bool loadShader(std::string path, ShaderClass** shader_object);
        bool loadVertexShader(std::string path);
        bool loadFragmentShader(std::string path);
        bool init(std::string vertex_shader_path, std::string fragment_shader_path);
        GLuint getPos();
        void setUniformValue(std::string uniform_name, glm::mat4x4 value);
        void setUniformValue(std::string uniform_name, glm::vec4 value);
        void setUniformValue(std::string uniform_name, float value);
        void setUniformValue(std::string uniform_name, int value);
        ~ShaderProgram();
    };
}
