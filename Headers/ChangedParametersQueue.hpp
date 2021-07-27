#pragma once

#include "LogicalParameter.hpp"

#include <queue>
#include <memory>


class ChangedParametersQueue
{
    public:
        void push(const std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> &pair);
        std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> pop();
        bool isEmpty() const;

    
    private:
        std::queue<std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>>> mQueue;
};