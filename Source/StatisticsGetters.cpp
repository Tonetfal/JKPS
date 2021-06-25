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
            - Settings::StatisticsTextCharacterSize * 9.f
            - Settings::SpaceOnStatisticsRight;
            // 9.f is value by eye
}

unsigned int Statistics::getStatisticsHeight(ID id)
{
    return    Settings::ButtonDistance 
            + Settings::StatisticsTextCharacterSize * id;
}

unsigned int Statistics::getKeyCountersWidth(size_t index)
{
    unsigned int buttonCenterX = 
        Settings::ButtonTextureSize.x * (index + 1) - 
        Settings::ButtonTextureSize.x / 2U +
        Settings::ButtonDistance * (index + 1);

    return buttonCenterX - mKeyCountersText[index].getLocalBounds().width / 2.f;
}

unsigned int Statistics::getKeyCountersHeight(size_t index)
{
    unsigned int buttonCenterY = Settings::ButtonTextureSize.y / 2U + Settings::ButtonDistance;

    return buttonCenterY - mKeyCountersText[index].getLocalBounds().height / 1.4f;
}
