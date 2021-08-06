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
    updateParameters();
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
    mStatLineText.setCharacterSize(chSz);
    mStatLineText.setStyle(style);
    mStatValueText.setFillColor(color);
    mStatValueText.setCharacterSize(chSz);
    mStatValueText.setStyle(style);
    sf::Vector2f pos;
    if (!Settings::StatisticsTextValuePositionsAdvancedMode)
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

const sf::Text &GfxStatisticsLine::getText() const
{
    return mStatLineText;
}

void GfxStatisticsLine::centerOrigin()
{
    const sf::FloatRect lineRect(mStatLineText.getLocalBounds());
    const sf::Vector2f lineOrigin(lineRect.left + lineRect.width / 2, lineRect.top + lineRect.height / 2);
    const sf::FloatRect valueRect(mStatValueText.getLocalBounds());
    const sf::Vector2f valueOrigin(valueRect.left + valueRect.width / 2, valueRect.top + valueRect.height / 2);
    mStatLineText.setOrigin(lineOrigin);
    mStatValueText.setOrigin(valueOrigin);
}

std::string *GfxStatisticsLine::getStatLineString(StatisticsID id)
{
    std::string *str;
    
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
        id == LogicalParameter::ID::StatTextBold || 
        id == LogicalParameter::ID::StatTextItal || 
        id == LogicalParameter::ID::StatTextChSz || 
        id == LogicalParameter::ID::StatTextClr ||
        id == LogicalParameter::ID::StatValPos ||
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
        id == LogicalParameter::ID::StatTextItal3;
}
