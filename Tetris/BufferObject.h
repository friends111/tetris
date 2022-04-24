#pragma once
#include <GL/glew.h>
#include <vector>

namespace OpenGLWrapper
{
	class BufferObject
	{
	protected:
		GLuint id = 0;
		GLenum usage = GL_STATIC_DRAW;
		GLenum type = GL_ARRAY_BUFFER;
		bool ready = false;
	public:
		BufferObject();
		bool create();
		bool isReady();
		void allocate(const void* data, int data_size);
		template<class DataType> void allocate(const std::vector<DataType>& data);
		void allocate(int data_size);
		void write(int offset, const void* data, int data_size);
		template<class DataType> void write(int offset, const std::vector<DataType>& data);
		bool bind();
		void release();
		void destroy();
		void setUsage(GLenum usage);	//before allocate()
		~BufferObject();
	};

	template<class DataType>
	inline void BufferObject::allocate(const std::vector<DataType>& data)
	{
		glBufferData(this->type, data.size() * sizeof(DataType), static_cast<const void*>(data.data()), this->usage);
	}

	template<class DataType>
	inline void BufferObject::write(int offset, const std::vector<DataType>& data)
	{
		glBufferSubData(this->type, offset, data.size() * sizeof(DataType), static_cast<const void*>(data.data()));
	}
}
