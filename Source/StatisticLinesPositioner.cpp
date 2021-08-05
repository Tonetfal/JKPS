#include "../Headers/StatisticLinesPositioner.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Application.hpp"


float StatisticsPositioner::getTotalHeight()
{
    float totalLinesHeight = 0;
    for (auto &text : *mStatistics)
    {
        if (text->getShowState())
            totalLinesHeight += text->getText().getLocalBounds().height + Settings::StatisticsTextDistance;
    }
    if (totalLinesHeight > 0)
        totalLinesHeight -= Settings::StatisticsTextDistance;
    
    return totalLinesHeight;
}

void StatisticsPositioner::operator()()
{
    assert(mStatistics);

    if (!Settings::StatisticsTextPositionsAdvancedMode)
    {
        const sf::Vector2f halfOrigin(mStatistics->front()->getText().getOrigin() / 2.f);
        const float width = Settings::StatisticsTextPosition.x + Application::getWindowWidth() - Settings::WindowBonusSizeRight + halfOrigin.x;
        const float totalHeight = getTotalHeight();
        const float startHeight = Settings::WindowBonusSizeTop + Settings::GfxButtonTextureSize.y / 2 - totalHeight / 2 - Settings::StatisticsTextPosition.y + halfOrigin.y;
        float currentHeight = 0;

        for (auto &text : *mStatistics)
        {
            if (text->getShowState())
            {
                text->setPosition(width, startHeight + currentHeight);
                currentHeight += text->getText().getLocalBounds().height + Settings::StatisticsTextDistance;
            }
        }
    }
    else
    {
        unsigned idx = 0;
        for (auto &text : *mStatistics)
        {
            text->setPosition(Settings::StatisticsTextPositions[idx]);
            ++idx;
        }
    }
}