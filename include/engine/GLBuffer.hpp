//
// Created by JS on 24/05/2021.
//

#ifndef CGFIRE_GLBUFFER_HPP
#define CGFIRE_GLBUFFER_HPP

namespace en
{
    class GLBuffer
    {
    public:
        static GLBuffer INVALID;

        GLBuffer(unsigned int type, unsigned int usage, unsigned int size, void* data);
        ~GLBuffer();

        void Bind() const;
        void Delete() const;

    private:
        unsigned int handle_;
        unsigned int type_;

        GLBuffer();
    };
}

#endif //CGFIRE_GLBUFFER_HPP
