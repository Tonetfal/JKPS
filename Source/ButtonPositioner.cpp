#include "../Headers/ButtonPositioner.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Utility.hpp"

#include <cassert>


ButtonPositioner::ButtonPositioner(std::vector<std::unique_ptr<Button>> *buttons)
: mButtons(buttons)
{
    assert(mButtons);
}

void ButtonPositioner::operator()()
{
    assert(mButtons);

    for (auto &button : *mButtons)
    {
        const auto idx = button->getIdx();
        const auto isInSupportedRange = idx < Settings::SupportedAdvancedKeysNumber;
        const auto advMode = isInSupportedRange && Settings::GfxButtonAdvancedMode;
        const auto defaultPos = sf::Vector2f(Button::getWidth(idx), Button::getHeight(idx));
        const auto offsetPos = !advMode ? sf::Vector2f() : Utility::swapY(Settings::GfxButtonsBtnPositions[idx]);
        const auto pos = defaultPos + offsetPos;

        button->setPosition(pos);
    }
}