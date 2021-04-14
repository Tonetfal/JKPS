template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::loadFromFile(Identifier id, const std::string& path)
{
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(path))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + path);
    
    insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::loadFromMemory(Identifier id
                                                , const void* data
                                                , std::size_t sizeInBytes)
{
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromMemory(data, sizeInBytes))
        throw std::runtime_error("ResourceHolder::load - Failed to load default resource");
    
    insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
    auto found = mResourceMap.find(id);
    assert(found != mResourceMap.end());

    return *found->second;
}

template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
{
    auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
    assert(inserted.second);
}
