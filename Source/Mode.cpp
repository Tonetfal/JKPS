#include "../Headers/Mode.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


Mode::State Mode::state(Mode::Normal);
Keys Mode::selectedKey(Keys::Unknown);
bool Mode::needToRelease(false);
bool Mode::wasChanged(false);

Mode::Mode()
{
    mAlert.setRadius(Settings::AlertRadius);
    mAlert.setOrigin(Settings::AlertRadius, Settings::AlertRadius);
    mAlert.setFillColor(Settings::AlertColor);
}

void Mode::updateCurrent(sf::Time dt)
{
    if (!KeyCombination::isCombinationPressed(selectedKey))
    {
        needToRelease = false;
    }
}

void Mode::switchState()
{
    switch (getState())
    {
        case Normal: state = Edit; break;
        case Edit: state = Normal; selectedKey = Keys::Unknown; break;
        default: break;
    }
}

void Mode::selectKey(Keys key)
{
    if (!needToRelease)
    {
        // mSelectedKey = (mSelectedKey == key ? Keys::Unknown : key);
        selectedKey = key;
        needToRelease = true;
    }
}

Keys Mode::getSelectedKey()
{
    return selectedKey;
}

bool Mode::wasKeyChanged()
{
    return wasChanged;
}

void Mode::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (getState() == Edit)
        target.draw(mAlert, states);
}

Mode::State Mode::getState()
{
    return state;
}

unsigned Mode::getCategory() const
{
    return Category::EditMode;
}