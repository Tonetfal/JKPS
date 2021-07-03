#include "../Headers/Statistic.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Calculator.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <SFML/Graphics/Font.hpp>

std::string typeToStr(Statistic::Type type)
{
    switch(type)
    {
        case Statistic::KPS: return "KPS";
        case Statistic::MaxKPS: return "MaxKPS";
        case Statistic::Total: return "Total";
        case Statistic::BPM: return "BPM";
        default: return "Unknown";
    };
}

std::string makeStr(const std::string &stat, int number)
{
    return stat + ": " + std::to_string(number);
}

Statistic::Statistic(const FontHolder &fonts, Type type)
: mType(type)
{
    mText.setFont(fonts.get(Fonts::Statistic));
    mText.setCharacterSize(Settings::StatisticsTextCharacterSize);
    mStat = typeToStr(mType);
    mText.setString(makeStr(mStat, 0));
    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::StatisticsBold ? sf::Text::Bold : 0) | 
        (Settings::StatisticsItalic ? sf::Text::Italic : 0)));
    
    switch (mType)
    {
        case KPS: mFunction = Calculator::getKeyPerSecond; break;
        case MaxKPS: mFunction = Calculator::getMaxKeyPerSecond; break;
        case Total: mFunction = Calculator::getTotalKeys; break;
        case BPM: mFunction = Calculator::getBeatsPerMinute; break;
    }
}

void Statistic::updateCurrent(sf::Time dt)
{
    if (mType == KPS)
    {
        if (!Settings::ShowMaxKPS && Calculator::getKeyPerSecond() == 0)
            mFunction = Calculator::getMaxKeyPerSecond;
        else
            mFunction = Calculator::getKeyPerSecond;
    }

    mText.setString(makeStr(mStat, mFunction()));
    mDebug(mText, mText.getPosition());
}

void Statistic::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mText, states);

#ifdef STATISTIC_DEBUG
    target.draw(mDebug, states);
#endif
}

unsigned Statistic::getCategory() const
{
    return Category::Statistics;
}

sf::Vector2f Statistic::getStartPosition(float textHeight)
{
    float spacePerButton = Settings::ButtonTextureSize.x + Settings::ButtonDistance;
    float statTotalHeight = (textHeight + Settings::StatisticsDistance) * 
        (StatisticCounter - !Settings::ShowMaxKPS - !Settings::ShowBPMText);
    float x = Settings::ButtonAmount * spacePerButton + Settings::SpaceBetweenButtonsAndStatistics;
    float y = Settings::ButtonDistance;

    if (statTotalHeight < Settings::ButtonTextureSize.y)
        y = (Settings::ButtonTextureSize.y - statTotalHeight) / 2.f - Settings::ButtonDistance;

    std::cout << statTotalHeight << "\n";
    std::cout << x << " " << y << "\n";
    return { x, y };
}

float Statistic::getWidth() const
{
    return mText.getLocalBounds().width;
}

float Statistic::getHeight() const
{
    return mText.getLocalBounds().height;
}
