#include "../Headers/StatisticLinesPositioner.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Application.hpp"


float StatisticsPositioner::getTotalHeight()
{
    float totalLinesHeight = 0;
    for (auto &text : *mStatistics)
    {
        if (text->getShowState())
            totalLinesHeight += text->getText().getLocalBounds().height + Settings::StatisticsDistance;
    }
    if (totalLinesHeight > 0)
        totalLinesHeight -= Settings::StatisticsDistance;
    
    return totalLinesHeight;
}

void StatisticsPositioner::operator()()
{
    assert(mStatistics);

    const float width = Settings::StatisticsPosition.x + Application::getWindowWidth() - Settings::WindowBonusSizeRight;
    const float totalHeight = getTotalHeight();
    const float startHeight = Settings::WindowBonusSizeTop + Settings::ButtonTextureSize.y / 2 - totalHeight / 2 - Settings::StatisticsPosition.y;
    float currentHeight = 0;

    for (auto &text : *mStatistics)
    {
        if (text->getShowState())
        {
            text->setPosition(width, startHeight + currentHeight);
            currentHeight += text->getText().getLocalBounds().height + Settings::StatisticsDistance;
        }
    }
}