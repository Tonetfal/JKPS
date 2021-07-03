#include "../Headers/Mode.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

Mode::State Mode::mState(Mode::Normal);
Keys Mode::mSelectedKey(Keys::Unknown);
bool Mode::needToRelease(false);

Mode::Mode()
{
    mAlert.setRadius(Settings::AlertRadius);
    mAlert.setOrigin(Settings::AlertRadius, Settings::AlertRadius);
    mAlert.setFillColor(Settings::AlertColor);
}

void Mode::updateCurrent(sf::Time dt)
{
    if (!KeyCombination::isCombinationPressed(mSelectedKey))
    {
        needToRelease = false;
    }
}

void Mode::switchState()
{
    switch (getState())
    {
        case Normal: mState = Edit; break;
        case Edit: mState = Normal; break;
        default: break;
    }
}

void Mode::selectKey(Keys key)
{
    if (!needToRelease)
    {
        // mSelectedKey = (mSelectedKey == key ? Keys::Unknown : key);
        mSelectedKey = key;
        needToRelease = true;
    }
}

Keys Mode::getSelectedKey()
{
    return mSelectedKey;
}

void Mode::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (getState() == Edit)
        target.draw(mAlert, states);
}

Mode::State Mode::getState()
{
    return mState;
}

unsigned Mode::getCategory() const
{
    return Category::EditMode;
}