#pragma once
#include <list>
#include <map>
#include <memory>

#include "IResourceFile.h"
#include "ResHandle.h"

class ResCache
{
public:
    ResCache(const unsigned int sizeInMb, IResourceFile *resFile);
    ~ResCache();
    
    bool Init();
    
    std::shared_ptr<ResHandle> GetHandle(Resource* r);
    // TODO Preload, Flush
    
protected:
    std::list<std::shared_ptr<ResHandle>> m_lru;
    std::map<std::string, std::shared_ptr<ResHandle>> m_resources;
    
    IResourceFile* m_file;
    
    unsigned int m_cacheSize;
    unsigned int m_allocated;
    
    std::shared_ptr<ResHandle> Find(Resource* r);
    const void* Update(std::shared_ptr<ResHandle> handle);
    std::shared_ptr<ResHandle> Load(Resource* r);
    void Free(std::shared_ptr<ResHandle> gonner);
    
    bool MakeRoom(unsigned int size);
    char* Allocate(unsigned int size);
    void FreeOneResource();
    void MemoryHasBeenFreed(unsigned int size);
};
