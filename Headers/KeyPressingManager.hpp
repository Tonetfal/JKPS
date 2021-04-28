#pragma once

#include "../Headers/Settings.hpp"

struct KeyPressingManager
{
                                KeyPressingManager();
    void                        readClickedKeys();
    void                        handleEvent(sf::Event event);
    void                        clear();

    std::vector<int>            mClickedKeys;
    std::vector<bool>           mNeedToBeReleased;
};