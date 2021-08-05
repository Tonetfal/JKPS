#include "../Headers/Button.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/StringHelper.hpp"


unsigned Button::mSize(0);

Button::Button(LogKey &key, const TextureHolder &textureHolder, const FontHolder &fontHolder)
: LogButton(key)
, GfxButton(mSize, textureHolder, fontHolder)
, mTextures(textureHolder)
, mFonts(fontHolder)
, mBtnIdx(mSize)
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

unsigned Button::getIdx() const
{
    return mBtnIdx;
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
        id == LogicalParameter::ID::MainWndwRght ||
        id == LogicalParameter::ID::BtnGfxTxtrClr ||
        id == LogicalParameter::ID::BtnTextPosAdvMode ||
        id == LogicalParameter::ID::BtnTextPos1 ||
        id == LogicalParameter::ID::BtnTextPos2 ||
        id == LogicalParameter::ID::BtnTextPos3 ||
        id == LogicalParameter::ID::BtnTextPos4 ||
        id == LogicalParameter::ID::BtnTextPos5 ||
        id == LogicalParameter::ID::BtnTextPos6 ||
        id == LogicalParameter::ID::BtnTextPos7 ||
        id == LogicalParameter::ID::BtnTextPos8 ||
        id == LogicalParameter::ID::BtnTextPos9 ||
        id == LogicalParameter::ID::BtnTextPos10 ||
        id == LogicalParameter::ID::BtnTextPos11 ||
        id == LogicalParameter::ID::BtnTextPos12 ||
        id == LogicalParameter::ID::BtnTextPos13 ||
        id == LogicalParameter::ID::BtnTextPos14 ||
        id == LogicalParameter::ID::BtnTextPos15 ||
        id == LogicalParameter::ID::BtnGfxDist ||
        id == LogicalParameter::ID::BtnGfxTxtr ||
        id == LogicalParameter::ID::BtnGfxTxtrSz ||
        id == LogicalParameter::ID::BtnGfxTxtrClr ||
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
        id == LogicalParameter::ID::BtnGfxSz15;
}