#include "../Headers/Statistics.hpp"

void Statistics::setupText(ID id)
{
    mTexts.build(id);
    mTexts.get(id).setCharacterSize(Settings::StatisticsTextCharacterSize);
    mTexts.get(id).setFillColor(Settings::StatisticsTextColor);
    mTexts.get(id).setPosition(sf::Vector2f(
        getStatisticsWidth(), getStatisticsHeight(id)));
}

void Statistics::setupLong(ID id)
{
    mLongs.build(id);
    mLongs.get(id) = 0;
}

void Statistics::setupString(ID id, const std::string& name)
{
    mStrings.build(id);
    mStrings.get(id) = name;
}