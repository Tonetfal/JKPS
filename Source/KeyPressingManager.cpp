#include "../Headers/KeyPressingManager.hpp"

KeyPressingManager::KeyPressingManager()
: mClickedKeys()
, mNeedToBeReleased()
{
    mClickedKeys.resize(Settings::ButtonAmount);
    mNeedToBeReleased.resize(Settings::ButtonAmount);
    clear();
}

void KeyPressingManager::readClickedKeys()
{
    const unsigned keyAmt = Settings::Keys.size();
    const unsigned btnAmt = Settings::MouseButtons.size();
    size_t i = 0;
    for (; i < keyAmt; i++)
    {
        if (sf::Keyboard::isKeyPressed(Settings::Keys[i]) && !mNeedToBeReleased[i])
            ++mClickedKeys[i];
            
        if(sf::Keyboard::isKeyPressed(Settings::Keys[i]))
            mNeedToBeReleased[i] = true;
        else
            mNeedToBeReleased[i] = false;
    }
    for (size_t j = 0; j < btnAmt; j++, i++)
    {
        if (sf::Mouse::isButtonPressed(Settings::MouseButtons[j]) && !mNeedToBeReleased[i])
            ++mClickedKeys[i];
            
        if(sf::Mouse::isButtonPressed(Settings::MouseButtons[j]))
            mNeedToBeReleased[i] = true;
        else
            mNeedToBeReleased[i] = false;
    }
}

void KeyPressingManager::resize()
{
    mClickedKeys.resize(Settings::ButtonAmount);
    mNeedToBeReleased.resize(Settings::ButtonAmount);
}

void KeyPressingManager::clear()
{
    for (auto& element : mClickedKeys)
        element = 0;
}