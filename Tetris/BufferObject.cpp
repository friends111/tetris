#include "BufferObject.h"

namespace OpenGLWrapper
{
    BufferObject::BufferObject()
    {
    }

    bool BufferObject::create()
    {
        glGenBuffers(1, &this->id);

        if (this->id == 0)
        {
            this->ready = false;

            return false;
        }

        this->ready = true;

        return true; //glIsBuffer(this->id) == GL_TRUE;
    }

    bool BufferObject::isReady()
    {
        return this->ready;
    }

    void BufferObject::allocate(const void* data, int data_size)
    {
        glBufferData(this->type, data_size, data, this->usage);
    }

    void BufferObject::allocate(int data_size)
    {
        glBufferData(this->type, data_size, 0, this->usage);
    }

    void BufferObject::write(int offset, const void* data, int data_size)
    {
        glBufferSubData(this->type, offset, data_size, data);
    }

    bool BufferObject::bind()
    {
        if (this->id == 0)
        {
            return false;
        }

        glBindBuffer(this->type, this->id);

        return true;
    }

    void BufferObject::release()
    {
        glBindBuffer(this->type, 0);
    }

    void BufferObject::destroy()
    {
        glDeleteBuffers(1, &this->id);
    }

    void BufferObject::setUsage(GLenum usage)
    {
        this->usage = usage;
    }

    BufferObject::~BufferObject()
    {
        this->destroy();
    }
}
