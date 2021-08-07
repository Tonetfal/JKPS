#include "../Headers/ButtonPositioner.hpp"
#include "../Headers/Settings.hpp"

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
        const unsigned idx = button->getIdx();
        const bool isInRange = idx < Settings::GfxButtonsBtnPositions.size();
        const sf::Vector2f defaultPos(Button::getWidth(idx), Button::getHeight(idx));
        const sf::Vector2f additionPos(Settings::GfxButtonsBtnPositions[idx].x, -Settings::GfxButtonsBtnPositions[idx].y);
        const sf::Vector2f pos(!Settings::GfxButtonBtnPositionsAdvancedMode || !isInRange ? defaultPos : defaultPos + additionPos);

        button->setPosition(pos);
    }
}