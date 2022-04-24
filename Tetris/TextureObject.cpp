#include "TextureObject.h"

namespace OpenGLWrapper
{
	TextureObject::TextureObject()
	{
	}

	bool TextureObject::create()
	{
		if (this->is_created == false)
		{
			glGenTextures(1, &this->id);

			if (this->id == 0)
			{
				this->is_created = false;
				return false;
			}

			this->is_created = true;
			return true;
		}

		return false;
	}

	void TextureObject::bind()
	{
		glBindTexture(this->texture_type, this->id);
	}

	void TextureObject::release()
	{
		glBindTexture(this->texture_type, 0);
	}

	void TextureObject::destroy()
	{
		glDeleteTextures(1, &this->id);
	}

	bool TextureObject::setData(GLint level, const void* data)
	{
		if (!this->isCreated())
		{
			return false;
		}

		this->level = level;

		this->bind();

		glTexParameteri(this->texture_type, GL_TEXTURE_WRAP_T, this->wrap_t);
		glTexParameteri(this->texture_type, GL_TEXTURE_WRAP_S, this->wrap_s);
		glTexParameteri(this->texture_type, GL_TEXTURE_MAG_FILTER, this->mag_filter);
		glTexParameteri(this->texture_type, GL_TEXTURE_MIN_FILTER, this->min_filter);
		glTexImage2D(this->texture_type, this->level, this->internal_format, this->width, this->height, this->border, this->format, this->type, data);

		this->release();

		return true;
	}

	void TextureObject::setPixelInfo(GLint internal_format, GLenum format, GLenum type)
	{
		this->internal_format = internal_format;
		this->format = format;
		this->type = type;
	}

	void TextureObject::setSize(GLsizei width, GLsizei height)
	{
		this->width = width;
		this->height = height;
	}

	void TextureObject::setMinMagFilters(GLenum min_filter, GLenum mag_filter)
	{
		this->min_filter = min_filter;
		this->mag_filter = mag_filter;
	}

	void TextureObject::setWrapModes(GLenum wrap_s, GLenum wrap_t)
	{
		this->wrap_s = wrap_s;
		this->wrap_t = wrap_t;
	}

	GLint TextureObject::getLevel()
	{
		return this->level;
	}

	GLenum TextureObject::getFormat()
	{
		return this->format;
	}

	GLint TextureObject::getInternalFormat()
	{
		return this->internal_format;
	}

	GLint TextureObject::getType()
	{
		return this->type;
	}

	GLsizei TextureObject::getWidth()
	{
		return this->width;
	}

	GLsizei TextureObject::getHeight()
	{
		return this->height;
	}

	GLenum TextureObject::getMinFilter()
	{
		return this->min_filter;
	}

	GLenum TextureObject::getMagFilter()
	{
		return this->mag_filter;
	}

	GLenum TextureObject::getWrapS()
	{
		return this->wrap_s;
	}

	GLenum TextureObject::getWrapT()
	{
		return this->wrap_t;
	}

	bool TextureObject::isCreated()
	{
		return this->is_created;
	}

	TextureObject::~TextureObject()
	{
		this->destroy();
	}
}
