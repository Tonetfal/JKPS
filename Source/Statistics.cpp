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
        setupString(TotalKeys, "Total");

        SET_TEXT_STRING(KPS);
        SET_TEXT_STRING(MaxKPS);
        SET_TEXT_STRING(TotalKeys);

        setupLong(BPM);
        setupText(BPM);
        setupString(BPM, "BPM");
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
        if (!Settings::ShowMaxKPS)
        {
            if (mLongs.get(KPS) == 0)
                mLongs.get(KPS) = mLongs.get(MaxKPS);
        }

        for (auto& element : clickedKeys)
            mLongs.get(TotalKeys) += element * Settings::ValueToMultiplyOnClick;

        if (Settings::ShowBPMText)
            mLongs.get(BPM) = BeatsPerMinute;
    }


    if (Settings::ShowStatisticsText)
    {
        SET_TEXT_STRING(KPS);
        SET_TEXT_STRING(MaxKPS);
        SET_TEXT_STRING(TotalKeys);
        if (Settings::ShowBPMText)
            SET_TEXT_STRING(BPM);
    }
}

void Statistics::handleEvent(sf::Event event)
{
    if (Settings::ShowStatisticsText)
    {
        setupText(KPS);
        setupText(MaxKPS);
        setupText(TotalKeys);
        if (Settings::ShowBPMText)
        {
            setupText(BPM);
        }
        setFonts();
    }
}

void Statistics::draw()
{
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(getStatisticsWidth(), 
        (int(mWindow.getSize().y) - int(Settings::WindowBonusSizeBottom) + 
        int(Settings::WindowBonusSizeTop) - int(getTotalStatisticsHeight())) / 2);

    if (Settings::ShowStatisticsText)
    {
        mWindow.draw(mTexts.get(KPS), transform);
        if (Settings::ShowMaxKPS)
            mWindow.draw(mTexts.get(MaxKPS), transform);
        mWindow.draw(mTexts.get(TotalKeys), transform);
        if (Settings::ShowBPMText)
            mWindow.draw(mTexts.get(BPM), transform);
    }
}

void Statistics::loadFonts(FontHolder& font)
{
    mStatisticsFont = &font.get(Fonts::Statistics);
    setFonts();
}

void Statistics::setFonts()
{
    if (Settings::ShowStatisticsText)
    {
        mTexts.get(KPS).setFont(*mStatisticsFont);
        mTexts.get(MaxKPS).setFont(*mStatisticsFont);
        mTexts.get(TotalKeys).setFont(*mStatisticsFont);

        mTexts.get(KPS).setOrigin(0, mTexts.get(KPS).getLocalBounds().height / 2.f);
        mTexts.get(TotalKeys).setOrigin(0, mTexts.get(KPS).getLocalBounds().height / 2.f);
        mTexts.get(MaxKPS).setOrigin(0, mTexts.get(KPS).getLocalBounds().height / 2.f);

        if (Settings::ShowBPMText)
        {
            mTexts.get(BPM).setFont(*mStatisticsFont);
            mTexts.get(BPM).setOrigin(0, mTexts.get(KPS).getLocalBounds().height / 2.f);
        }
    }

    bool skip = false;
    for (size_t i = 0; i < StatisticsCounter; ++i)
    {
        if ((ID(i) == MaxKPS && !Settings::ShowMaxKPS)
        ||  (ID(i) == BPM && !Settings::ShowBPMText))
        {
            skip = true;
            continue;
        }

        sf::Text &elem = mTexts.get(ID(i));
        elem.setOrigin(elem.getLocalBounds().left, elem.getLocalBounds().top);
        if (i > 0)
        {
            sf::Text &prev = mTexts.get(ID(i - 1 - skip));
            elem.setPosition(0, prev.getPosition().y + prev.getLocalBounds().height + Settings::StatisticsDistance);
        }
        skip = false;
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
    static float height = 0;
    
    // Multiply by 2 because if there is no BPM line there will be only 2 spaces between 3 lines,
    // but if BPM line is active the space will be added there
    if (height == 0)
    {
        if (Settings::ShowStatisticsText)
        {
            height = getStatisticsHeight(MaxKPS) + mTexts.get(MaxKPS).getLocalBounds().height;
            if (Settings::ShowMaxKPS)
            {
                height += mTexts.get(TotalKeys).getLocalBounds().height + Settings::StatisticsDistance;
            }
            if (Settings::ShowBPMText)
            {
                height += mTexts.get(TotalKeys).getLocalBounds().height + Settings::StatisticsDistance;
            }
        }
    }

    return height;
}

void Statistics::clear()
{
    if (Settings::ShowStatisticsText)
    {
        mLongs.get(KPS) = 0;
        mLongs.get(MaxKPS) = 0;
        mLongs.get(TotalKeys) = 0;
        if (Settings::ShowBPMText)
            mLongs.get(BPM) = 0;
    }
}

void Statistics::setupText(ID id)
{
    mTexts.build(id);
    mTexts.get(id).setCharacterSize(Settings::StatisticsTextCharacterSize);
    mTexts.get(id).setFillColor(Settings::StatisticsTextColor);

    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::StatisticsBold ? sf::Text::Bold : 0) | 
        (Settings::StatisticsItalic ? sf::Text::Italic : 0)));
    mTexts.get(id).setStyle(style);
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
    return (Settings::ButtonAmount) * Settings::ButtonTextureSize.x +
        Settings::SpaceBetweenButtonsAndStatistics + Settings::WindowBonusSizeLeft;
}

unsigned int Statistics::getStatisticsHeight(ID id)
{
    unsigned int height = 0;
    for (size_t i = 0; i < id; ++i)
    {
        height += mTexts.get(ID(i)).getLocalBounds().height + Settings::StatisticsDistance;
    }

    return height;
}
