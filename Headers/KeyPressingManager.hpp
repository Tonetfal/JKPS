#pragma once

#include <SFML/Window/Keyboard.hpp>

#include "../Headers/Settings.hpp"

#include <vector>

struct KeyPressingManager
{
                                KeyPressingManager();
    void                        readClickedKeys(bool work);
    void                        handleEvent(sf::Event event);
    void                        clear();

    std::vector<int>            mClickedKeys;
    std::vector<bool>           mNeedToBeReleased;
};