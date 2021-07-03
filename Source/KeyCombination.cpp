#include "../Headers/KeyCombination.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>


KeyCombination::KeyCombination(Keys k1, Keys k2, Keys k3)
{
    mKeys.push_back(k1);
    if (k2 != Keys::Unknown)
        mKeys.push_back(k2);
    if (k3 != Keys::Unknown)
        mKeys.push_back(k3);
}

    #include <iostream>
bool KeyCombination::isCombinationPressed(const KeyCombination &combination)
{
    for (auto key : combination.mKeys)
    {
        if (key < static_cast<Keys>(sf::Keyboard::KeyCount))
        {
            if (!sf::Keyboard::isKeyPressed(convertKeysToKey(key)))
                return false;
        }
        else
        {
            if (!sf::Mouse::isButtonPressed(convertKeysToButton(key)))
                return false;
        }
    }
    return true;
}

bool KeyCombination::operator==(const KeyCombination &combination) const
{
    for (auto key1 : combination.mKeys)
    {
        bool isEqual;
        for (auto key2 : mKeys)
        {
            if (key1 == key2)
            {
                isEqual = true;
                break;
            }
        }
        if (!isEqual)
            return false;
    }
    return true;
}

bool KeyCombination::operator<(const KeyCombination &combination) const
{
    return mKeys[0] < combination.mKeys[0];
}

bool KeyCombination::operator==(Keys key) const
{
    return mKeys[0] == key;
}
