#include "../Headers/Statistics.hpp"

std::size_t Statistics::getKeyPerSecond()
{
    return mLongs.get(KPS);
}

std::size_t Statistics::getMaxKeyPerSecond()
{
    return mLongs.get(MaxKPS);
}

std::size_t Statistics::getTotalKeys()
{
    return mLongs.get(TotalKeys);
}


unsigned int Statistics::getStatisticsWidth()
{
    return    mWindow.getSize().x 
            - Settings::StatisticsTextCharacterSize * 9.f
            - Settings::SpaceOnStatisticsRight;
            // 9.f is value by eye
}

unsigned int Statistics::getStatisticsHeight(ID id)
{
    return    Settings::ButtonDistance 
            + Settings::StatisticsTextCharacterSize * id;
}

