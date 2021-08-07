#include "../Headers/StatisticLinesPositioner.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Application.hpp"


float StatisticsPositioner::getTotalHeight()
{
    float totalLinesHeight = 0;
    for (auto &text : *mStatistics)
    {
        if (text->getShowState())
        {
            const float rectLineHeight(text->getLineText().getLocalBounds().height);
            const float rectValHeight(text->getValueText().getLocalBounds().height);
            totalLinesHeight += std::max(rectLineHeight, rectValHeight) + Settings::StatisticsTextDistance;
        }
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
        const float width = Settings::StatisticsTextPosition.x + Application::getWindowWidth() - Settings::WindowBonusSizeRight;
        const float totalHeight = getTotalHeight();
        const float startHeight = Settings::WindowBonusSizeTop + Settings::GfxButtonTextureSize.y / 2 - totalHeight / 2 - Settings::StatisticsTextPosition.y;
        float currentHeight = 0;

        for (auto &text : *mStatistics)
        {
            if (text->getShowState())
            {
                const float rectLineHeight(text->getLineText().getLocalBounds().height);
                const float rectValHeight(text->getValueText().getLocalBounds().height);

                text->setPosition(width, startHeight + currentHeight);
                currentHeight += std::max(rectLineHeight, rectValHeight) + Settings::StatisticsTextDistance;
            }
        }
    }
    else
    {
        unsigned idx = 0;
        for (auto &text : *mStatistics)
        {
            const sf::Vector2f pos(Settings::StatisticsTextPositions[idx].x, -Settings::StatisticsTextPositions[idx].y);
            text->setPosition(pos);
            ++idx;
        }
    }
}