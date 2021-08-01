#include "../Headers/Statistics.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Menu.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


Statistics::Statistics(const FontHolder& fonts)
: mFonts(fonts)
{
    setupLong(KPS);
    setupLong(MaxKPS);
    setupLong(TotalKeys);
    setupLong(BPM);
    
    setupString(KPS, Settings::StatisticsKPSText);
    setupString(MaxKPS, Settings::StatisticsMaxKPSText);
    setupString(TotalKeys, Settings::StatisticsTotalText);
    setupString(BPM, Settings::StatisticsBPMText);

    mTexts.build(KPS);
    mTexts.build(MaxKPS);
    mTexts.build(TotalKeys);
    mTexts.build(BPM);

    setTextString(KPS);
    setTextString(MaxKPS);
    setTextString(TotalKeys);
    setTextString(BPM);

    setupText();
    setTextPositions();
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
            {
                mLongs.get(KPS) = mLongs.get(MaxKPS);
                mStrings.get(KPS) = Settings::StatisticsKPS2Text;
            }
            else
            {
                mStrings.get(KPS) = Settings::StatisticsKPSText;
            }
        }

        for (auto& element : clickedKeys)
            mLongs.get(TotalKeys) += element * Settings::ValueToMultiplyOnClick;

        mLongs.get(BPM) = BeatsPerMinute;
            
        setTextString(KPS);
        setTextString(MaxKPS);
        setTextString(TotalKeys);
        setTextString(BPM);
    }
}

void Statistics::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (Settings::ShowStatisticsText)
    {
        states.transform.translate(getStatisticsWidth(), 
            (Settings::ButtonTextureSize.y / 2.f + 
            Settings::WindowBonusSizeTop - getTotalStatisticsHeight() / 2.f));

        if (Settings::ShowKPS)
            target.draw(mTexts.get(KPS), states);
        if (Settings::ShowMaxKPS)
            target.draw(mTexts.get(MaxKPS), states);
        if (Settings::ShowTotal)
            target.draw(mTexts.get(TotalKeys), states);
        if (Settings::ShowBPMText)
            target.draw(mTexts.get(BPM), states);
    }
}

void Statistics::setupText()
{
    setFonts();

    mStrings.get(KPS) = Settings::ShowMaxKPS ? Settings::StatisticsKPSText : Settings::StatisticsKPS2Text;
    mStrings.get(MaxKPS) = Settings::StatisticsMaxKPSText;
    mStrings.get(TotalKeys) = Settings::StatisticsTotalText;
    mStrings.get(BPM) = Settings::StatisticsBPMText;

    setupText(KPS);
    setupText(MaxKPS);
    setupText(TotalKeys);
    setupText(BPM);
    setTextPositions();
}

void Statistics::setFonts()
{
    const sf::Font &font = mFonts.get(Fonts::Statistics);

    mTexts.get(KPS).setFont(font);
    mTexts.get(MaxKPS).setFont(font);
    mTexts.get(TotalKeys).setFont(font);
    mTexts.get(BPM).setFont(font);
}

void Statistics::setupText(ID id)
{
    mTexts.get(id).setCharacterSize(Settings::StatisticsTextCharacterSize);
    mTexts.get(id).setFillColor(Settings::StatisticsTextColor);

    sf::Text::Style style(static_cast<sf::Text::Style>(
        (Settings::StatisticsBold ? sf::Text::Bold : 0) | 
        (Settings::StatisticsItalic ? sf::Text::Italic : 0)));
    mTexts.get(id).setStyle(style);
}

void Statistics::setTextPositions()
{
    unsigned skip = 0;
    for (unsigned i = 0; i < StatisticsCounter; ++i)
    {
        sf::Text &elem = mTexts.get(ID(i));
        elem.setOrigin(elem.getLocalBounds().left, elem.getLocalBounds().top);

        if ((ID(i) == KPS       && !Settings::ShowKPS)
        ||  (ID(i) == MaxKPS    && !Settings::ShowMaxKPS)
        ||  (ID(i) == TotalKeys && !Settings::ShowTotal)
        ||  (ID(i) == BPM       && !Settings::ShowBPMText))
        {
            ++skip;
            continue;
        }

        // Calculate statistics position by taking position of previous drawn element
        if (i - skip > 0)
        {
            sf::Text &prev = mTexts.get(ID(i - 1 - skip));
            elem.setPosition(0, prev.getPosition().y + prev.getLocalBounds().height + Settings::StatisticsDistance);
        }
        else
            elem.setPosition(0, 0);
        skip = 0;
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

void Statistics::resize()
{
    if (Settings::ShowStatisticsText)
    {
        if (Settings::ShowKPS)
            setupText(KPS);
        if (Settings::ShowMaxKPS)
            setupText(MaxKPS);
        if (Settings::ShowTotal)
            setupText(TotalKeys);
        if (Settings::ShowBPMText)
            setupText(BPM);
        setFonts();
    }
}

void Statistics::clear()
{
    if (Settings::ShowStatisticsText)
    {
        mLongs.get(KPS) = 0;
        mLongs.get(MaxKPS) = 0;
        mLongs.get(TotalKeys) = 0;
        mLongs.get(BPM) = 0;
    }
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

void Statistics::setTextString(ID id)
{
    mTexts.get(id).setString((mStrings.get(id) + std::to_string(mLongs.get(id))).c_str());
}

unsigned int Statistics::getStatisticsWidth() const
{
    return Settings::ButtonAmount * Settings::ButtonTextureSize.x +
        (Settings::ButtonAmount - 1) * Settings::ButtonDistance +
        Settings::SpaceBetweenButtonsAndStatistics + Settings::WindowBonusSizeLeft;
}

unsigned int Statistics::getStatisticsHeight(ID id) const
{
    unsigned int height = 0;
    for (size_t i = 0; i < id; ++i)
    {
        height += mTexts.get(ID(i)).getLocalBounds().height + Settings::StatisticsDistance;
    }

    return height;
}

std::size_t Statistics::getTotalStatisticsHeight() const
{
    float height = 0;
    
    unsigned show = 0;
    if (Settings::ShowStatisticsText)
    {        
        if (Settings::ShowKPS)
        {
            height += mTexts.get(KPS).getLocalBounds().height;
            ++show;
        }
        if (Settings::ShowMaxKPS)
        {
            height += mTexts.get(MaxKPS).getLocalBounds().height;
            ++show;
        }
        if (Settings::ShowTotal)
        {
            height += mTexts.get(TotalKeys).getLocalBounds().height;
            ++show;
        }
        if (Settings::ShowBPMText)
        {
            height += mTexts.get(BPM).getLocalBounds().height;
            ++show;
        }
        if (height != 0)
        {
            height += Settings::StatisticsDistance * (show - 1);
        }
    }

    return height;
}

bool Statistics::parameterIdMatches(LogicalParameter::ID id)
{
    return 
        id == LogicalParameter::ID::StatTextDist ||
        id == LogicalParameter::ID::SpaceBtwBtnAndStat ||
        id == LogicalParameter::ID::StatTextFont ||
        id == LogicalParameter::ID::StatTextClr ||
        id == LogicalParameter::ID::StatTextChSz ||
        id == LogicalParameter::ID::StatTextBold ||
        id == LogicalParameter::ID::StatTextItal ||
        id == LogicalParameter::ID::StatTextShow ||
        id == LogicalParameter::ID::StatTextShowKPS ||
        id == LogicalParameter::ID::StatTextShowMaxKPS ||
        id == LogicalParameter::ID::StatTextShowTotal ||
        id == LogicalParameter::ID::StatTextShowBPM ||
        id == LogicalParameter::ID::StatTextKPSText ||
        id == LogicalParameter::ID::StatTextKPS2Text ||
        id == LogicalParameter::ID::StatTextMaxKPSText ||
        id == LogicalParameter::ID::StatTextTotalText ||
        id == LogicalParameter::ID::StatTextBPMText;
}
