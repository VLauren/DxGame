#pragma once
#include "Resource.h"

class ResHandle
{
    friend class ResCache;
    
public:
    ResHandle(Resource& resource, char* buffer, unsigned int size, ResCache* pResCache);
    virtual ~ResHandle();
    
    unsigned int Size() const { return m_size; }
    char* Buffer() const { return m_buffer; }
    char* WritableBuffer() { return m_buffer; }
    
protected:
    Resource m_resource;
    char* m_buffer;
    unsigned int m_size;
    ResCache* m_pResCache;
};
