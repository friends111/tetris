#include "VertexArrayObject.h"

namespace OpenGLWrapper
{
	VertexArrayObject::VertexArrayObject()
	{
	}

	bool VertexArrayObject::create()
	{
		glGenVertexArrays(1, &this->id);

		if (this->id == 0)
		{
			this->ready = false;

			return false;
		}

		this->ready = true;

		return true; //glIsVertexArray(this->id) == GL_TRUE;
	}

	bool VertexArrayObject::isReady()
	{
		return this->ready;
	}

	void VertexArrayObject::bind()
	{
		glBindVertexArray(this->id);
	}

	void VertexArrayObject::release()
	{
		glBindVertexArray(0);
	}

	void VertexArrayObject::destroy()
	{
		glDeleteVertexArrays(1, &this->id);
	}

	void VertexArrayObject::addAttribute(GLint component_size, GLenum data_type, GLboolean normalized, GLsizei stride, unsigned int data_size)
	{
		glVertexAttribPointer(this->current_attrib_index, component_size, data_type, normalized, stride, (void*)(this->current_attrib_offset));
		glEnableVertexAttribArray(this->current_attrib_index);

		this->current_attrib_index++;
		this->current_attrib_offset += data_size;
	}

	void VertexArrayObject::modifyAttribute(GLuint attribute_index, GLint component_size, GLenum data_type, GLboolean normalized, GLsizei stride, unsigned int attribute_offset)
	{
		glVertexAttribPointer(attribute_index, component_size, data_type, normalized, stride, (void*)(attribute_offset));
	}

	VertexArrayObject::~VertexArrayObject()
	{
		this->destroy();
	}
}
