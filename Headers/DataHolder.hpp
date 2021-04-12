#pragma once

#include <map>
#include <memory>
#include <stdexcept>
#include <cassert>

template <typename Data, typename Identifier>
class DataHolder
{
    public:
        typedef std::unique_ptr<Data> Ptr;


    public:
        void                        build(Identifier id);
        Data&                       get(Identifier id);


    private:
        std::map<Identifier, Ptr>   mDataMap; 
};

#include "DataHolder.inl"