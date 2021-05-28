//
// Created by JS on 24/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/GLBuffer.hpp"

namespace en
{
    GLBuffer GLBuffer::INVALID = GLBuffer();

    GLBuffer::GLBuffer(unsigned int type, unsigned int usage, unsigned int size, void* data)
    {
        type_ = type;

        glGenBuffers(1, &handle_);
        if (data)
        {
            glBindBuffer(type, handle_);
            glBufferData(type, size, data, usage);
        }
    }

    GLBuffer::~GLBuffer()
    {
    }

    void GLBuffer::Bind() const
    {
        glBindBuffer(type_, handle_);
    }

    void GLBuffer::Delete() const
    {
        glDeleteBuffers(1, &handle_);
    }

    GLBuffer::GLBuffer()
    {
    }
}
