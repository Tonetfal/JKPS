#include "../Headers/ChangedParametersQueue.hpp"


void ChangedParametersQueue::push(const std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &pair)
{
    mQueue.push(pair);
}

std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> ChangedParametersQueue::pop()
{
    auto pair = mQueue.back();
    mQueue.pop();
    return pair;
}

bool ChangedParametersQueue::isEmpty() const
{
    return mQueue.empty();
}
