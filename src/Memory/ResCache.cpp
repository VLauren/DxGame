#include "ResCache.h"

ResCache::ResCache(const unsigned int sizeInMb, IResourceFile* resFile)
{
    m_cacheSize = sizeInMb * 1024 * 1024;
    m_allocated = 0;
    m_file = resFile;
}

ResCache::~ResCache()
{
    while (!m_lru.empty())
        FreeOneResource();
    // SAFE_DELETE(m_file);
}

bool ResCache::Init()
{
    bool res = false;
    if (m_file->VOpen())
    {
        // default resource loader
    }
}

std::shared_ptr<ResHandle> ResCache::GetHandle(Resource* r)
{
    std::shared_ptr<ResHandle> handle(Find(r));
    if (handle == NULL)
        handle = Load(r);
    else
        Update(handle);
    return handle;
}

std::shared_ptr<ResHandle> ResCache::Load(Resource* r)
{
    std::shared_ptr<ResHandle> handle;
    
    unsigned int rawSize = m_file->VGetRawResourceSize(*r);
    char* rawBuffer = new char[rawSize];
    
    m_file->VGetRawResource(*r, rawBuffer);
    unsigned int size = 0;
    handle = std::shared_ptr<ResHandle>(new ResHandle(*r, rawBuffer, rawSize, this));
    
    if (handle)
    {
        m_lru.push_front(handle);
        m_resources[r->m_name] = handle;
    }
    
    return handle;
}

bool ResCache::MakeRoom(unsigned int size)
{
}

char* ResCache::Allocate(unsigned int size)
{
}

void ResCache::FreeOneResource()
{
}


