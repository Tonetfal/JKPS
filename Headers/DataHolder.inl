template <typename Data, typename Identifier>
void DataHolder<Data, Identifier>::build(Identifier id)
{
    std::unique_ptr<Data> data(new Data());
    mDataMap.insert(std::make_pair(id, std::move(data))); 
}

template <typename Data, typename Identifier>
const Data& DataHolder<Data, Identifier>::get(Identifier id) const
{
    auto found = mDataMap.find(id);
    assert(found != mDataMap.end());

    return *found->second;
}

template <typename Data, typename Identifier>
Data& DataHolder<Data, Identifier>::get(Identifier id)
{
    auto found = mDataMap.find(id);
    assert(found != mDataMap.end());

    return *found->second;
}