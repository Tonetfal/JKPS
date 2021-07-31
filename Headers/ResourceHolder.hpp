#pragma once

#include <iostream>
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
        bool                        loadFromFile(Identifier id, const std::string& path);
        void                        loadFromMemory(Identifier id
                                                , const void* data
                                                , std::size_t sizeInBytes);
        Resource&                   get(Identifier id) const;

        void                        clear();

    
    private:
        void                        insertResource(Identifier id, Ptr resource);


    private:
        std::map<Identifier, Ptr>   mResourceMap;
};

#include "ResourceHolder.inl"