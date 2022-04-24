#pragma once
#include <GL/glew.h>
#include <iostream>
#include <vector>

namespace OpenGLWrapper
{
	class TextureObject
	{
	private:
		const GLenum texture_type = GL_TEXTURE_2D;
		GLint level = 0;	//mipmap level
		GLint internal_format = GL_R;	//GL_RGB8, GL_RGBA8, etc.
		GLsizei width = 0;
		GLsizei height = 0;
		const GLint border = 0;
		GLenum format = GL_RED;	//GL_RED, GL_RGBA, GL_BGRA, etc.
		GLenum type = GL_BYTE;	//GL_UNSIGNED_SHORT_4_4_4_4, etc.
		GLuint id = 0;
		GLenum min_filter = GL_NEAREST;	//GL_NEAREST, GL_LINEAR
		GLenum mag_filter = GL_NEAREST;	//GL_NEAREST, GL_LINEAR
		GLenum wrap_s = GL_CLAMP_TO_EDGE;	//GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, GL_REPEAT
		GLenum wrap_t = GL_CLAMP_TO_EDGE;	//GL_CLAMP_TO_EDGE, GL_MIRRORED_REPEAT, GL_REPEAT
		bool is_created = false;
	public:
		TextureObject();
		bool create();
		void bind();
		void release();
		void destroy();
		bool setData(GLint level, const void* data);	//must be called, only once, after all other properties are set
		template<class DataType> bool setData(GLint level, const std::vector<DataType>& data);
		void setPixelInfo(GLint internal_format, GLenum format, GLenum type);
		void setSize(GLsizei width, GLsizei height);
		void setMinMagFilters(GLenum min_filter, GLenum mag_filter);
		void setWrapModes(GLenum wrap_s, GLenum wrap_t);
		GLint getLevel();
		GLenum getFormat();
		GLint getInternalFormat();
		GLint getType();
		GLsizei getWidth();
		GLsizei getHeight();
		GLenum getMinFilter();
		GLenum getMagFilter();
		GLenum getWrapS();
		GLenum getWrapT();
		bool isCreated();
		~TextureObject();
	};

	template<class DataType>
	inline bool TextureObject::setData(GLint level, const std::vector<DataType>& data)
	{
		return this->setData(level, static_cast<const void*>(data.data()));
	}
}
