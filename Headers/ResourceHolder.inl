template <typename Resource, typename Identifier>
void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& path)
{
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(path))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + path);
    
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
