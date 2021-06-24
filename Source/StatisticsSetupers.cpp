#include "../Headers/Statistics.hpp"

void Statistics::setupText(ID id)
{
    mTexts.build(id);
    mTexts.get(id).setCharacterSize(Settings::StatisticsTextCharacterSize);
    
    mTexts.get(id).setFillColor(Settings::StatisticsTextColor);
    if (!Settings::ShowStatisticsText)
        mTexts.get(id).setFillColor((Settings::KeyCountersTextColor - sf::Color(0,0,0,255)));

    mTexts.get(id).setPosition(sf::Vector2f(getStatisticsWidth(),
                                            getStatisticsHeight(id)));
}

void Statistics::setupTextVector()
{
    mKeyCountersText.resize(Settings::ButtonAmount);

    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
        mKeyCountersText[i].setFillColor(Settings::KeyCountersTextColor);
        if (!Settings::ShowKeyCountersText && !Settings::IsChangeable)
            mKeyCountersText[i].setFillColor(Settings::KeyCountersTextColor - sf::Color(0,0,0,255));

        setupTextPosition(i);
    }
}

void Statistics::setupTextPosition(int index)
{
    mKeyCountersText[index].setPosition(sf::Vector2f(
        getKeyCountersWidth(index), getKeyCountersHeight(index)));
}

void Statistics::setupLong(ID id)
{
    mLongs.build(id);
    mLongs.get(id) = 0;
}

void Statistics::setupLongVector()
{
    mKeyCounters.resize(Settings::ButtonAmount);
    for (auto& element : mKeyCounters)
        element = 0;
}

void Statistics::setupLongVector(std::size_t index)
{
    mKeyCounters.resize(Settings::ButtonAmount);
    mKeyCounters[index] = 0;
}

void Statistics::setupString(ID id, const std::string& name)
{
    mStrings.build(id);
    mStrings.get(id) = name;
}

void Statistics::setupStringVector()
{
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        mKeyCountersText[i].setString(std::to_string(mKeyCounters[i]).c_str());
    }
}