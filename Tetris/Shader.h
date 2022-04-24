#pragma once
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <iostream>

namespace OpenGLWrapper
{
    class Shader
    {
    private:
        virtual void virtualDummy() = 0;
    protected:
        const GLenum shader_type;
        const GLchar* source = "";
        bool compiled = false;
        GLuint shader_pos = 0;
    public:
        Shader(GLenum shader_type);
        bool create();
        bool compile(std::string shader_path);
        bool isCompiled();
        GLuint getPos();
        const char* getSource();
        ~Shader();
    };
}
