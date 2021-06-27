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

        SET_TEXT_STRING(KPS);
        SET_TEXT_STRING(MaxKPS);
        SET_TEXT_STRING(TotalKeys);
    }

    if (Settings::ShowBPMText)
    {
        setupLong(BPM);
        setupText(BPM);
        setupString(BPM, "Current BPM");

        SET_TEXT_STRING(BPM);
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

std::size_t Statistics::getTotalStatisticsHeight()
{
    const static float mult(1.43f); // This is the value that helps to find the height of a text by using only its size
                                    // Should work only for digits and letters
    const static int textHeight(ceil(Settings::StatisticsTextCharacterSize * mult));
    static std::size_t height(
        Settings::ShowStatisticsText ? textHeight * 3 : 1 +
        Settings::ShowBPMText ? textHeight : 0);
    
    return height;
}


void Statistics::setupText(ID id)
{
    mTexts.build(id);
    mTexts.get(id).setCharacterSize(Settings::StatisticsTextCharacterSize);
    mTexts.get(id).setFillColor(Settings::StatisticsTextColor);
    mTexts.get(id).setPosition(sf::Vector2f(
        getStatisticsWidth(), getStatisticsHeight(id)));
}

void Statistics::setupLong(ID id)
{
    mLongs.build(id);
    mLongs.get(id) = 0;
}

void Statistics::setupString(ID id, const std::string& name)
{
    mStrings.build(id);
    mStrings.get(id) = name;
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
