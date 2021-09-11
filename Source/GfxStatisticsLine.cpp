#include "../Headers/GfxStatisticsLine.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/Button.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>


GfxStatisticsLine::GfxStatisticsLine(const FontHolder& fontHolder, const bool &show, const StatisticsID identifier)
: mFonts(fontHolder)
, mIdentifier(identifier)
, mShow(show)
{
    updateAsset();
    mStatLineText.setString(*getStatLineString(mIdentifier));
    mStatValueText.setString(getStatValueString(mIdentifier));
    updateParameters();
    centerOrigin();
}

void GfxStatisticsLine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (!mShow || !Settings::ShowStatisticsText)
        return;

    states.transform.combine(getTransform());

    target.draw(mStatLineText, states);
    const sf::FloatRect rect(mStatLineText.getLocalBounds());
    states.transform.translate(rect.left + rect.width, 0);
    target.draw(mStatValueText, states);
}

void GfxStatisticsLine::update()
{
    if (!mShow || !Settings::ShowStatisticsText)
        return;

    mStatLineText.setString(*getStatLineString(mIdentifier));
    mStatValueText.setString(getStatValueString(mIdentifier));
    centerOrigin();

    // Due to different text sizes of KPS and Max that change continuously
    // value text position has to be updated. I've put it here cuz I'm lazy atm :(
    if (Settings::StatisticsTextPositionsAdvancedMode
    && !Settings::StatisticsTextValuePositionsAdvancedMode)
    {
        const sf::FloatRect bounds = mStatLineText.getLocalBounds();
        float x = -bounds.width / 2.f;
        float y = -bounds.height / 2.f;
        mStatValueText.setPosition(x, y);
    } 
}

void GfxStatisticsLine::updateAsset()
{
    mStatLineText.setFont(mFonts.get(Fonts::Statistics));
    mStatValueText.setFont(mFonts.get(Fonts::Statistics));
}

void GfxStatisticsLine::updateParameters()
{
    const sf::Color color = !Settings::StatisticsTextColorsAdvancedMode ? 
        Settings::StatisticsTextColor : Settings::StatisticsTextColors[mIdentifier];

    const unsigned chSz = !Settings::StatisticsTextChSzssAdvancedMode ?
        Settings::StatisticsTextCharacterSize : Settings::StatisticsTextCharacterSizes[mIdentifier];

    const bool bold = !Settings::StatisticsTextBoldAdvancedMode ?
        Settings::StatisticsTextBold : Settings::StatisticsTextBolds[mIdentifier];
    
    const bool italic = !Settings::StatisticsTextItalicAdvancedMode ?
        Settings::StatisticsTextItalic : Settings::StatisticsTextItalics[mIdentifier];

    const sf::Uint32 style = (bold ? sf::Text::Bold : 0) | (italic ? sf::Text::Italic : 0);

    mStatLineText.setFillColor(color);
    mStatValueText.setFillColor(color);

    mStatLineText.setCharacterSize(chSz);
    mStatValueText.setCharacterSize(chSz);

    mStatLineText.setStyle(style);
    mStatValueText.setStyle(style);

    mStatLineText.setOutlineThickness(Settings::StatisticsTextOutlineThickness / 10.f);
    mStatValueText.setOutlineThickness(Settings::StatisticsTextOutlineThickness / 10.f);

    mStatLineText.setOutlineColor(Settings::StatisticsTextOutlineColor);
    mStatValueText.setOutlineColor(Settings::StatisticsTextOutlineColor);

    sf::Vector2f pos;
    if (Settings::StatisticsTextPositionsAdvancedMode
    && !Settings::StatisticsTextValuePositionsAdvancedMode)
    {
        const sf::FloatRect bounds = mStatLineText.getLocalBounds();
        pos.x = -bounds.width / 2.f;
        pos.y = -bounds.height / 2.f;
    } 
    else if (!Settings::StatisticsTextValuePositionsAdvancedMode)
    {
        pos.x = Settings::StatisticsTextValuePosition.x;
        pos.y = -Settings::StatisticsTextValuePosition.y;
    } 
    else
    {
        pos.x = Settings::StatisticsTextValuePositions[mIdentifier].x;
        pos.y = -Settings::StatisticsTextValuePositions[mIdentifier].y;
    }
    mStatValueText.setPosition(pos);
    centerOrigin();
}

bool GfxStatisticsLine::getShowState() const
{
    return mShow;
}

const sf::Text &GfxStatisticsLine::getLineText() const
{
    return mStatLineText;
}

const sf::Text &GfxStatisticsLine::getValueText() const
{
    return mStatValueText;
}

void GfxStatisticsLine::centerOrigin()
{
    const sf::FloatRect lineRect(mStatLineText.getLocalBounds());
    const sf::Vector2f lineOrigin(lineRect.left + lineRect.width / 2, lineRect.top + lineRect.height / 2);
    const sf::FloatRect valueRect(mStatValueText.getLocalBounds());
    const sf::Vector2f valueOrigin(valueRect.left + valueRect.width / 2, valueRect.top + valueRect.height / 2);

    if (!Settings::StatisticsTextPositionsAdvancedMode)
        mStatLineText.setOrigin(lineRect.left, lineRect.top);
    else
        mStatLineText.setOrigin(lineOrigin);

    if (!Settings::StatisticsTextValuePositionsAdvancedMode)
        mStatValueText.setOrigin(valueRect.left, valueRect.top);
    else
        mStatValueText.setOrigin(valueOrigin);
}

std::string *GfxStatisticsLine::getStatLineString(StatisticsID id)
{
    std::string *str(nullptr);
    
    switch(id)
    {
        case KPS:
            str = Button::getKeysPerSecond() != 0 ? &Settings::StatisticsKPSText : &Settings::StatisticsKPS2Text;
            break;

        case Total:
            str = &Settings::StatisticsTotalText;
            break;

        case BPM:
            str = &Settings::StatisticsBPMText;
            break;
    }
    
    assert(str);
    return str;
}

std::string GfxStatisticsLine::getStatValueString(StatisticsID id)
{
    std::string str;
    
    switch(id)
    {
        case KPS:
            if (Button::getKeysPerSecond() != 0)
            {
                // if (show decimal)
                //     str = eraseDigitsOverHundredths(std::to_string(Button::getKeysPerSecond()));
                // else
                str = std::to_string(static_cast<unsigned>(Button::getKeysPerSecond()));
            }
            else
            {
                // if (show decimal)
                //     str = eraseDigitsOverHundredths(std::to_string(Button::getMaxKeysPerSecond()));
                // else
                str = std::to_string(static_cast<unsigned>(Button::getMaxKeysPerSecond()));
            }
            break;

        case Total:
            str = std::to_string(Button::getTotal());
            break;

        case BPM:
            str = std::to_string(static_cast<unsigned>(Button::getBeatsPerMinute()));
            break;
    }
    
    return str;
}

bool GfxStatisticsLine::parameterIdMatches(LogicalParameter::ID id)
{
    return 
        id == LogicalParameter::ID::StatTextDist ||
        id == LogicalParameter::ID::StatPos ||
        id == LogicalParameter::ID::StatValPos ||
        id == LogicalParameter::ID::StatTextFont ||
        id == LogicalParameter::ID::StatTextClr ||
        id == LogicalParameter::ID::StatTextChSz ||
        id == LogicalParameter::ID::StatTextOutThck ||
        id == LogicalParameter::ID::StatTextOutClr ||
        id == LogicalParameter::ID::StatTextBold ||
        id == LogicalParameter::ID::StatTextItal ||
        id == LogicalParameter::ID::StatTextShow ||
        id == LogicalParameter::ID::StatTextShowKPS ||
        id == LogicalParameter::ID::StatTextShowTotal ||
        id == LogicalParameter::ID::StatTextShowBPM ||
        id == LogicalParameter::ID::StatTextPosAdvMode ||
        id == LogicalParameter::ID::StatTextPos1 ||
        id == LogicalParameter::ID::StatTextPos2 ||
        id == LogicalParameter::ID::StatTextPos3 ||
        id == LogicalParameter::ID::StatTextValPosAdvMode ||
        id == LogicalParameter::ID::StatTextValPos1 ||
        id == LogicalParameter::ID::StatTextValPos2 ||
        id == LogicalParameter::ID::StatTextValPos3 ||
        id == LogicalParameter::ID::StatTextClrAdvMode ||
        id == LogicalParameter::ID::StatTextClr1 ||
        id == LogicalParameter::ID::StatTextClr2 ||
        id == LogicalParameter::ID::StatTextClr3 ||
        id == LogicalParameter::ID::StatTextChSzAdvMode ||
        id == LogicalParameter::ID::StatTextChSz1 ||
        id == LogicalParameter::ID::StatTextChSz2 ||
        id == LogicalParameter::ID::StatTextChSz3 ||
        id == LogicalParameter::ID::StatTextBoldAdvMode ||
        id == LogicalParameter::ID::StatTextBold1 ||
        id == LogicalParameter::ID::StatTextBold2 ||
        id == LogicalParameter::ID::StatTextBold3 ||
        id == LogicalParameter::ID::StatTextItalAdvMode ||
        id == LogicalParameter::ID::StatTextItal1 ||
        id == LogicalParameter::ID::StatTextItal2 ||
        id == LogicalParameter::ID::StatTextItal3 ||
        id == LogicalParameter::ID::StatTextKPSText ||
        id == LogicalParameter::ID::StatTextKPS2Text ||
        id == LogicalParameter::ID::StatTextTotalText ||
        id == LogicalParameter::ID::StatTextBPMText ||
        id == LogicalParameter::ID::BtnGfxBtnPosAdvMode ||
        id == LogicalParameter::ID::BtnGfxSzAdvMode ||
        id == LogicalParameter::ID::BtnGfxBtnPos1 ||
        id == LogicalParameter::ID::BtnGfxSz1 ||
        id == LogicalParameter::ID::BtnGfxBtnPos2 ||
        id == LogicalParameter::ID::BtnGfxSz2 ||
        id == LogicalParameter::ID::BtnGfxBtnPos3 ||
        id == LogicalParameter::ID::BtnGfxSz3 ||
        id == LogicalParameter::ID::BtnGfxBtnPos4 ||
        id == LogicalParameter::ID::BtnGfxSz4 ||
        id == LogicalParameter::ID::BtnGfxBtnPos5 ||
        id == LogicalParameter::ID::BtnGfxSz5 ||
        id == LogicalParameter::ID::BtnGfxBtnPos6 ||
        id == LogicalParameter::ID::BtnGfxSz6 ||
        id == LogicalParameter::ID::BtnGfxBtnPos7 ||
        id == LogicalParameter::ID::BtnGfxSz7 ||
        id == LogicalParameter::ID::BtnGfxBtnPos8 ||
        id == LogicalParameter::ID::BtnGfxSz8 ||
        id == LogicalParameter::ID::BtnGfxBtnPos9 ||
        id == LogicalParameter::ID::BtnGfxSz9 ||
        id == LogicalParameter::ID::BtnGfxBtnos10 ||
        id == LogicalParameter::ID::BtnGfxSz10 ||
        id == LogicalParameter::ID::BtnGfxBtnos11 ||
        id == LogicalParameter::ID::BtnGfxSz11 ||
        id == LogicalParameter::ID::BtnGfxBtnos12 ||
        id == LogicalParameter::ID::BtnGfxSz12 ||
        id == LogicalParameter::ID::BtnGfxBtnos13 ||
        id == LogicalParameter::ID::BtnGfxSz13 ||
        id == LogicalParameter::ID::BtnGfxBtnos14 ||
        id == LogicalParameter::ID::BtnGfxSz14 ||
        id == LogicalParameter::ID::BtnGfxBtnos15 ||
        id == LogicalParameter::ID::BtnGfxSz15 ||
        id == LogicalParameter::ID::BtnGfxTxtrSz ||
        id == LogicalParameter::ID::BtnGfxDist ||
        id == LogicalParameter::ID::MainWndwTop ||
        id == LogicalParameter::ID::MainWndwBot ||
        id == LogicalParameter::ID::MainWndwLft ||
        id == LogicalParameter::ID::MainWndwRght;
}
