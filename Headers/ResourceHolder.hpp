#pragma once

#include <string>
#include <memory>
#include <map>
#include <stdexcept>
#include <cassert>

template <typename Resource, typename Identifier>
class ResourceHolder
{
    public:
        typedef std::unique_ptr<Resource> Ptr;


    public:
        void                        loadFromFile(Identifier id, const std::string& path);
        void                        loadFromMemory(Identifier id
                                                , const void* data
                                                , std::size_t sizeInBytes);
        Resource&                   get(Identifier id) const;

    
    private:
        void                        insertResource(Identifier id, Ptr resource);


    private:
        std::map<Identifier, Ptr>   mResourceMap;
};

#include "ResourceHolder.inl"