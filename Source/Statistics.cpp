#include "../Headers/Statistics.hpp"

#define SET_TEXT_STRING(x) mTexts.get(x).setString((mStrings.get(x) + ": " \
                                                 + std::to_string(mLongs.get(x))).c_str());


Statistics::Statistics(sf::RenderWindow& window)
: mWindow(window)
, mStatisticsFont()
, mTexts()
, mLongs()
, mStrings()
{
    if (Settings::ShowStatisticsText)
    {
        setupLong(KPS);
        setupLong(MaxKPS);
        setupLong(TotalKeys);
        
        setupText(KPS);
        setupText(MaxKPS);
        setupText(TotalKeys);

        setupString(KPS, "KPS");
        setupString(MaxKPS, "Max KPS");
        setupString(TotalKeys, "Total keys");
    }

    if (Settings::ShowBPMText)
    {
        setupLong(BPM);
        setupText(BPM);
        setupString(BPM, "Current BPM");
    }
}

void Statistics::update(std::size_t KeyPerSecond, 
    size_t BeatsPerMinute, std::vector<int>& clickedKeys )
{
    if (Settings::ShowStatisticsText)
    {
        mLongs.get(KPS) = KeyPerSecond;
        if (mLongs.get(KPS) > mLongs.get(MaxKPS))
            mLongs.get(MaxKPS) = mLongs.get(KPS);

        for (auto& element : clickedKeys)
            mLongs.get(TotalKeys) += element;
    }
    if (Settings::ShowBPMText)
        mLongs.get(BPM) = BeatsPerMinute;


    if (Settings::ShowStatisticsText)
    {
        SET_TEXT_STRING(KPS);
        SET_TEXT_STRING(MaxKPS);
        SET_TEXT_STRING(TotalKeys);
    }
    if (Settings::ShowBPMText)
        SET_TEXT_STRING(BPM);
}

void Statistics::handleEvent(sf::Event event)
{
    if (Settings::ShowStatisticsText)
    {
        setupText(KPS);
        setupText(MaxKPS);
        setupText(TotalKeys);
    }
    if (Settings::ShowBPMText)
    {
        setupText(BPM);
        setFonts();
    }
}

void Statistics::draw()
{
    if (Settings::ShowStatisticsText)
    {
        mWindow.draw(mTexts.get(KPS));
        mWindow.draw(mTexts.get(MaxKPS));
        mWindow.draw(mTexts.get(TotalKeys));
        if (Settings::ShowBPMText)
            mWindow.draw(mTexts.get(BPM));
    }
}


void Statistics::loadFonts(FontHolder& font)
{
    mStatisticsFont = &font.get(Fonts::Statistics);
}

void Statistics::setFonts()
{
    if (Settings::ShowStatisticsText)
    {
        mTexts.get(KPS).setFont(*mStatisticsFont);
        mTexts.get(MaxKPS).setFont(*mStatisticsFont);
        mTexts.get(TotalKeys).setFont(*mStatisticsFont);
    }
    if (Settings::ShowBPMText)
    {
        mTexts.get(BPM).setFont(*mStatisticsFont);
    }
}