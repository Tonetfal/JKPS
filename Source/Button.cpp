#include "../Headers/Button.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/StringHelper.hpp"


unsigned Button::mSize(0);

Button::Button(LogKey &key, const TextureHolder &textureHolder, const FontHolder &fontHolder)
: LogButton(key)
, GfxButton(textureHolder, fontHolder)
, mTextures(textureHolder)
, mFonts(fontHolder)
{
    ++mSize;
}

void Button::update()
{
    GfxButton::update(LogButton::isButtonPressed());

    std::string str;
    const GfxButton::TextID id = GfxButton::getTextIdToDisplay();
    switch (id)
    {
        case TextID::VisualKey: 
            str = LogButton::mKey.visualStr;
            break;

        case TextID::KeyCounters:
            str = std::to_string(LogButton::mTotal);
            break;
        case TextID::KeyPerSecond:
            if (false)
                str = eraseDigitsOverHundredths(std::to_string(mKeysPerSecond));
            else
                str = std::to_string(static_cast<unsigned>(mKeysPerSecond));
            break;
        case TextID::BeatsPerMinute:
            str = std::to_string(static_cast<unsigned>(LogButton::getLocalBeatsPerMinute()));
            break;
    }
    sf::Text &text = *mTexts[id];
    text.setString(str);

    if (LogButton::mKey.resetChangedState())
        text.setCharacterSize(Settings::ButtonTextCharacterSize);

    GfxButton::keepInBounds(text);
    GfxButton::centerOrigins();
}

void Button::processInput()
{
    processRealtimeInput();
}

LogKey *Button::getLogKey()
{
    return &(LogButton::mKey);
}

Button::~Button()
{
    --mSize;
}

unsigned Button::size()
{
    return mSize;
}

bool Button::parameterIdMatches(LogicalParameter::ID id)
{
    return 
        id == LogicalParameter::ID::BtnTextClr ||
        id == LogicalParameter::ID::BtnTextChSz ||
        id == LogicalParameter::ID::BtnTextPosition ||
        id == LogicalParameter::ID::BtnTextBounds ||
        id == LogicalParameter::ID::BtnTextBold ||
        id == LogicalParameter::ID::BtnTextItal ||
        id == LogicalParameter::ID::BtnTextShowVisKeys ||
        id == LogicalParameter::ID::BtnTextShowTot ||
        id == LogicalParameter::ID::BtnTextShowKps ||
        id == LogicalParameter::ID::BtnGfxDist ||
        id == LogicalParameter::ID::BtnGfxTxtrSz ||
        id == LogicalParameter::ID::BtnGfxTxtrClr ||
        id == LogicalParameter::ID::AnimGfxVel ||
        id == LogicalParameter::ID::AnimGfxScl ||
        id == LogicalParameter::ID::AnimGfxClr ||
        id == LogicalParameter::ID::AnimGfxOffset ||
        id == LogicalParameter::ID::MainWndwTop ||
        id == LogicalParameter::ID::MainWndwBot ||
        id == LogicalParameter::ID::MainWndwLft ||
        id == LogicalParameter::ID::MainWndwRght;
}