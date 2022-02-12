#include "../Headers/StatisticLinesPositioner.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Application.hpp"
#include "../Headers/Utility.hpp"


float StatisticsPositioner::getTotalHeight()
{
    auto totalLinesHeight = 0.f;
    for (auto &text : *mStatistics)
    {
        if (text->getShowState())
        {
            const float rectLineHeight(text->getLineText().getLocalBounds().height);
            const float rectValHeight(text->getValueText().getLocalBounds().height);
            totalLinesHeight += std::max(rectLineHeight, rectValHeight) + Settings::StatisticsTextDistance;
        }
    }
    if (totalLinesHeight > 0.f)
        totalLinesHeight -= Settings::StatisticsTextDistance;
    
    return totalLinesHeight;
}

void StatisticsPositioner::operator()()
{
    assert(mStatistics);

    const auto advMode = Settings::StatisticsTextAdvancedMode;

    const auto width = Settings::StatisticsTextPosition.x + Application::getWindowWidth() - Settings::WindowBonusSizeRight;
    const auto totalHeight = getTotalHeight();
    const auto startHeight = Settings::WindowBonusSizeTop + Settings::GfxButtonTextureSize.y / 2.f - totalHeight / 2.f - Settings::StatisticsTextPosition.y;
    auto currentHeight = 0.f;

    auto idx = 0u;
    for (auto &text : *mStatistics)
    {
        const auto orig = advMode ? Utility::swapY(Settings::StatisticsTextAdvPosition[idx]) : sf::Vector2f();

        if (text->getShowState())
        {
            const auto rectLineHeight = text->getLineText().getLocalBounds().height;
            const auto rectValHeight = text->getValueText().getLocalBounds().height;
            const auto pos = orig + sf::Vector2f(width, startHeight + currentHeight);

            text->setPosition(pos);
            currentHeight += std::max(rectLineHeight, rectValHeight) + Settings::StatisticsTextDistance;
        }

        ++idx;
    }
}