#include "../Headers/KeyPressingManager.hpp"

KeyPressingManager::KeyPressingManager()
: mClickedKeys()
, mNeedToBeReleased()
{
    mClickedKeys.resize(Settings::KeyAmount);
    mNeedToBeReleased.resize(Settings::KeyAmount);
    clear();
}

void KeyPressingManager::readClickedKeys()
{
    if (!Settings::IsChangeable)
    {
        for (size_t i = 0; i < Settings::KeyAmount; i++)
        {
            if (sf::Keyboard::isKeyPressed(Settings::Keys[i]) && !mNeedToBeReleased[i])
                ++mClickedKeys[i];
                
            if(sf::Keyboard::isKeyPressed(Settings::Keys[i]))
                mNeedToBeReleased[i] = true;
            else
                mNeedToBeReleased[i] = false;
        }
    }
}

void KeyPressingManager::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease)
        || sf::Keyboard::isKeyPressed(Settings::KeyToDecrease))
        {
            mClickedKeys.resize(Settings::KeyAmount);
            mNeedToBeReleased.resize(Settings::KeyAmount);
        }
    }
}

void KeyPressingManager::clear()
{
    for (auto& element : mClickedKeys)
        element = 0;
}