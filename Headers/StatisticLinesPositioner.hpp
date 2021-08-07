#pragma once

#include "GfxStatisticsLine.hpp"

#include <array>
#include <memory>


struct StatisticsPositioner
{
    template <std::size_t SIZE>
    StatisticsPositioner(std::array<std::unique_ptr<GfxStatisticsLine>, SIZE> *statistics)
        : mStatistics(statistics) { assert(mStatistics); }

    float getTotalHeight();
    void operator()();

    std::array<std::unique_ptr<GfxStatisticsLine>, GfxStatisticsLine::StatisticsIdCounter> *mStatistics;
};
