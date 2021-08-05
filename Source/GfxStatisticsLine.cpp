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

    states.transform.translate(Transformable::getPosition());
    states.transform.scale(getScale());

    target.draw(mText, states);
}

void GfxStatisticsLine::update()
{
    if (!mShow || !Settings::ShowStatisticsText)
        return;

    mText.setString(getString(mIdentifier));
    centerOrigin();
}

void GfxStatisticsLine::updateAsset()
{
    mText.setFont(mFonts.get(Fonts::Statistics));
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

    mText.setFillColor(color);
    mText.setCharacterSize(chSz);
    mText.setStyle(style);
}

bool GfxStatisticsLine::getShowState() const
{
    return mShow;
}

const sf::Text &GfxStatisticsLine::getText() const
{
    return mText;
}

void GfxStatisticsLine::centerOrigin()
{
    const sf::FloatRect rect(mText.getLocalBounds());
    mText.setOrigin(rect.left + rect.width / 2, rect.top + rect.height / 2);
}

std::string GfxStatisticsLine::getString(StatisticsID id)
{
    std::string str;
    
    switch(id)
    {
        case KPS:
            if (Button::getKeysPerSecond() != 0)
            {
                if (false)
                    str = Settings::StatisticsKPSText + eraseDigitsOverHundredths(std::to_string(Button::getKeysPerSecond()));
                else
                    str = Settings::StatisticsKPSText + (std::to_string(static_cast<unsigned>(Button::getKeysPerSecond())));
            }
            else
            {
                if (false)
                    str = Settings::StatisticsKPS2Text + eraseDigitsOverHundredths(std::to_string(Button::getMaxKeysPerSecond()));
                else
                    str = Settings::StatisticsKPS2Text + std::to_string(static_cast<unsigned>(Button::getMaxKeysPerSecond()));

            }
            break;

        case Total:
            str = Settings::StatisticsTotalText + std::to_string(Button::getTotal());
            break;

        case BPM:
            str = Settings::StatisticsBPMText + std::to_string(static_cast<unsigned>(Button::getBeatsPerMinute()));
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
        id == LogicalParameter::ID::StatTextPosAdvMode ||
        id == LogicalParameter::ID::StatTextPos1 ||
        id == LogicalParameter::ID::StatTextPos2 ||
        id == LogicalParameter::ID::StatTextPos3 ||
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
