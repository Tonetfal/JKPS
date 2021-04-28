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

long Statistics::getKeyCounter(std::size_t index)
{
    return mKeyCounters[index];
}

unsigned int Statistics::getStatisticsWidth()
{
    return    mWindow.getSize().x 
            - Settings::StatisticsTextCharacterSize * 9.f;
            // 9.f is value by eye
}

unsigned int Statistics::getStatisticsHeight(ID id)
{
    return    Settings::Distance 
            + Settings::StatisticsTextCharacterSize * id;
}

unsigned int Statistics::getKeyCountersWidth(size_t index)
{
    return    Settings::Distance * (index + 1) 
            + Settings::ButtonTextureSize.x / 2.f * (index * 2 + 1)
            - mKeyCountersText[index].getLocalBounds().width / 2;
}

unsigned int Statistics::getKeyCountersHeight(size_t index)
{
    return    Settings::ButtonTextureSize.y / 2 
            - mKeyCountersText[index].getCharacterSize() / 2 
            + Settings::Distance;
}
