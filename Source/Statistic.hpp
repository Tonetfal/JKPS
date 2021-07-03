#include "../Headers/Statistic.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <SFML/Graphics/Font.hpp>


Statistic::Statistic(const FontHolder &fonts)
{
    mText.setFont(fonts.get(Fonts::Statistic));
    mText.setCharacterSize(Settings::StatisticsTextCharacterSize);
    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::StatisticsBold ? sf::Text::Bold : 0) | 
        (Settings::StatisticsItalic ? sf::Text::Italic : 0)));
}

void Statistic::updateCurrent(sf::Time dt)
{
    
}

void Statistic::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mText, states);
}

unsigned Statistic::getCategory() const
{
    return Category::Statistics;
}