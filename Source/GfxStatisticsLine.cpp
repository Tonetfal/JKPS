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
    mText.setFillColor(Settings::StatisticsTextColor);
    mText.setCharacterSize(Settings::StatisticsTextCharacterSize);
    mText.setStyle(
        Settings::StatisticsTextBold   ? sf::Text::Bold   : 0 | 
        Settings::StatisticsTextItalic ? sf::Text::Italic : 0);
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
    mText.setOrigin(mText.getLocalBounds().left, mText.getLocalBounds().top);
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
        id == LogicalParameter::ID::StatTextClr;
}
