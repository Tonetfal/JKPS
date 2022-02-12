#include "../Headers/GfxStatisticsLine.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Utility.hpp"

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

    // Draw values in front of text string
    const auto rect = mStatLineText.getLocalBounds();
    states.transform.translate(rect.left + rect.width, 0.f);

    target.draw(mStatValueText, states);
}

void GfxStatisticsLine::update()
{
    if (!mShow || !Settings::ShowStatisticsText)
        return;

    mStatLineText.setString(*getStatLineString(mIdentifier));
    mStatValueText.setString(getStatValueString(mIdentifier));
    centerOrigin();

    const auto advMode = Settings::StatisticsTextAdvancedMode;
    const auto origPos = Settings::StatisticsTextValuePosition;
    const auto advPos = Settings::StatisticsTextAdvValuePosition.at(mIdentifier);
    const auto pos = origPos + (advMode ? advPos : sf::Vector2f());

    mStatValueText.setPosition(pos);
}

void GfxStatisticsLine::updateAsset()
{
    mStatLineText.setFont(mFonts.get(Fonts::Statistics));
    mStatValueText.setFont(mFonts.get(Fonts::Statistics));
}

void GfxStatisticsLine::updateParameters()
{
    const auto advMode = Settings::StatisticsTextAdvancedMode;

    const auto color = !advMode ? Settings::StatisticsTextColor 
        : Settings::StatisticsTextAdvColor[mIdentifier];

    const auto chSz = !advMode ? Settings::StatisticsTextCharacterSize 
        : Settings::StatisticsTextAdvCharacter[mIdentifier];

    const auto outThck = static_cast<float>(Settings::StatisticsTextOutlineThickness) / 10.f;
    const auto outColor = Settings::StatisticsTextOutlineColor;

    const auto bold = !advMode ? Settings::StatisticsTextBold 
        : Settings::StatisticsTextAdvBold[mIdentifier];
    
    const auto italic = !advMode ? Settings::StatisticsTextItalic 
        : Settings::StatisticsTextAdvItalic[mIdentifier];

    const auto style = sf::Uint32(bold ? sf::Text::Bold : 0) | (italic ? sf::Text::Italic : 0);

    mStatLineText.setFillColor(color);
    mStatValueText.setFillColor(color);

    mStatLineText.setCharacterSize(chSz);
    mStatValueText.setCharacterSize(chSz);

    mStatLineText.setStyle(style);
    mStatValueText.setStyle(style);

    mStatLineText.setOutlineThickness(outThck);
    mStatValueText.setOutlineThickness(outThck);

    mStatLineText.setOutlineColor(outColor);
    mStatValueText.setOutlineColor(outColor);

    const auto origPos = Settings::StatisticsTextValuePosition;
    const auto advPos = Settings::StatisticsTextAdvValuePosition.at(mIdentifier);
    const auto pos = origPos + (advMode ? advPos : sf::Vector2f());

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
    const auto advMode = Settings::StatisticsTextAdvancedMode;
    const auto centerOrigin = !advMode ? Settings::StatisticsTextCenterOrigin
        : Settings::StatisticsTextAdvCenterOrigin.at(mIdentifier);

    const auto lineRect = mStatLineText.getLocalBounds();
    const auto valueRect = mStatValueText.getLocalBounds();

    mStatLineText.setOrigin(lineRect.left, lineRect.top);

    // Origin center
    if (centerOrigin)
    {
        const auto valueOrigin = sf::Vector2f(
            valueRect.left + valueRect.width / 2.f, 
            lineRect.top
        );

        mStatValueText.setOrigin(valueOrigin);
    }
    // Origin left
    else
    {
        mStatValueText.setOrigin(valueRect.left, valueRect.top);
    }
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
    // const auto idU = static_cast<unsigned>(id);
    // const auto advParms = 8ul;
    // const auto firstAdvPar = static_cast<unsigned>(LogicalParameter::ID::BtnTextAdvClr1);
    // const auto lastAdvPar = firstAdvPar + advParms * Settings::SupportedAdvancedKeysNumber - 1u;

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
        (id >= LogicalParameter::ID::StatTextAdvMode && id <= LogicalParameter::ID::StatTextBPMText) ||
        (id >= LogicalParameter::ID::BtnGfxAdvMode   && id <= LogicalParameter::ID::BtnGfxClr20) ||
        id == LogicalParameter::ID::BtnGfxTxtrSz ||
        id == LogicalParameter::ID::BtnGfxDist ||
        id == LogicalParameter::ID::MainWndwTop ||
        id == LogicalParameter::ID::MainWndwBot ||
        id == LogicalParameter::ID::MainWndwLft ||
        id == LogicalParameter::ID::MainWndwRght;
}
