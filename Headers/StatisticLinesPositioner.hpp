#pragma once

#include "GfxStatisticsLine.hpp"
#include "Settings.hpp"

#include <iostream>
#include <array>
#include <memory>


struct StatisticsPositioner
{
    template <std::size_t SIZE>
    StatisticsPositioner(std::array<std::unique_ptr<GfxStatisticsLine>, SIZE> *statistics)
    : mStatistics(statistics)
    {
        assert(mStatistics);
    }

    float getTotalHeight()
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

    void operator()()
    {
        assert(mStatistics);
        const float width = Settings::StatisticsPosition.x;
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

    std::array<std::unique_ptr<GfxStatisticsLine>, GfxStatisticsLine::StatisticsIdCounter> *mStatistics;
};
