#pragma once
#include <GL/glew.h>
#include <iostream>

namespace OpenGLWrapper
{
	class VertexArrayObject
	{
	private:
		bool ready = false;
		GLuint id = 0;
		unsigned int current_attrib_offset = 0;
		unsigned int current_attrib_index = 0;
	public:
		VertexArrayObject();
		bool create();
		bool isReady();
		void bind();
		void release();
		void destroy();
		void addAttribute(GLint component_size, GLenum data_type, GLboolean normalized, GLsizei stride, unsigned int data_size);
		void modifyAttribute(GLuint attribute_index, GLint component_size, GLenum data_type, GLboolean normalized, GLsizei stride, unsigned int attribute_offset);
		~VertexArrayObject();
	};
}
