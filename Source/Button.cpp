#include "../Headers/Button.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/StringHelper.hpp"


unsigned Button::mSize(0u);

Button::Button(LogKey &key, const TextureHolder &textureHolder, const FontHolder &fontHolder)
: LogButton(mSize, key)
, GfxButton(mSize, textureHolder, fontHolder)
, mTextures(textureHolder)
, mFonts(fontHolder)
, mBtnIdx(mSize)
{
    LogButton::mTotal = Settings::KeysTotal[mBtnIdx];
    ++mSize;
}

void Button::update()
{
    GfxButton::update(LogButton::isButtonPressed());
    setTextStrings();
    controlBounds();
}

void Button::processInput()
{
    processRealtimeInput();
}

void Button::reset()
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::ButtonTextAdvancedMode;
    const auto chSz = !advMode ? Settings::ButtonTextCharacterSize : Settings::ButtonTextAdvCharacterSize[mBtnIdx];

    LogButton::reset();
    setTextStrings();
    // text could be too large, but if everything was reset then original ch size must be set and controlled
    for (auto &text : mTexts)
    {
        text->setCharacterSize(chSz);
        GfxButton::keepInBounds(*text);
    }
    GfxButton::centerOrigins();
}

void Button::setTextStrings()
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::ButtonTextAdvancedMode;
    const auto chSz = !advMode ? Settings::ButtonTextCharacterSize : Settings::ButtonTextAdvCharacterSize[mBtnIdx];

    const auto lAlt = Settings::ShowOppOnAlt && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
    const auto sepValAdvMode = Settings::ButtonTextSepPosAdvancedMode;

    if (Settings::ButtonTextShowVisualKeys) 
    {
        if ((!lAlt) || (lAlt && sepValAdvMode && Settings::ButtonTextShowTotal))
            mTexts[VisualKey]->setString(LogButton::mKey.visualStr);
        else
            mTexts[VisualKey]->setString(std::to_string(LogButton::mTotal));
    }
    if (Settings::ButtonTextShowTotal)
    {
        if ((!lAlt) || (lAlt && sepValAdvMode && Settings::ButtonTextShowVisualKeys))
            mTexts[KeyCounter]->setString(std::to_string(LogButton::mTotal));
        else
            mTexts[KeyCounter]->setString(LogButton::mKey.visualStr);
    }
    if (Settings::ButtonTextShowKPS)
    {
        // static_cast is required since KPS w/floating point is not still done
        mTexts[KeyPerSecond]->setString(std::to_string(static_cast<unsigned>(mKeysPerSecond)));
    }
    if (Settings::ButtonTextShowBPM)
    {
        mTexts[BeatsPerMinute]->setString(std::to_string(static_cast<unsigned>(LogButton::getLocalBeatsPerMinute())));
    }

    for (auto &text : mTexts)
    {
        text->setCharacterSize(chSz);
        GfxButton::keepInBounds(*text);
    }
    GfxButton::centerOrigins();
}

void Button::controlBounds()
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::ButtonTextAdvancedMode;
    const auto chSz = !advMode ? Settings::ButtonTextCharacterSize : Settings::ButtonTextAdvCharacterSize[mBtnIdx];

    for (auto &text : mTexts)
    {
        if (LogButton::mKey.resetChangedState())
            text->setCharacterSize(chSz);

        GfxButton::keepInBounds(*text);
    }
    GfxButton::centerOrigins();
}

LogKey *Button::getLogKey()
{
    return &(LogButton::mKey);
}

Button::~Button()
{
    Settings::KeysTotal[mBtnIdx] = 0;
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
    const auto idU = static_cast<unsigned>(id);
    const auto textAdvParms = 8ul;
    const auto firstTextAdvPar = static_cast<unsigned>(LogicalParameter::ID::BtnTextAdvClr1);
    const auto lastTextAdvPar = firstTextAdvPar + textAdvParms * Settings::SupportedAdvancedKeysNumber - 1u;

    const auto gfxBtnAdvParms = 3ul;
    const auto firstGfxBtnAdvPar = static_cast<unsigned>(LogicalParameter::ID::BtnGfxBtnPos1);
    const auto lastGfxBtnAdvPar = firstGfxBtnAdvPar + gfxBtnAdvParms * Settings::SupportedAdvancedKeysNumber - 1u;

    return 
        id == LogicalParameter::ID::BtnTextClr ||
        id == LogicalParameter::ID::BtnTextChSz ||
        id == LogicalParameter::ID::BtnTextOutThck ||
        id == LogicalParameter::ID::BtnTextOutClr ||
        id == LogicalParameter::ID::BtnTextPosition ||
        id == LogicalParameter::ID::BtnTextBounds ||
        id == LogicalParameter::ID::BtnTextBold ||
        id == LogicalParameter::ID::BtnTextItal ||
        id == LogicalParameter::ID::BtnTextShowVisKeys ||
        id == LogicalParameter::ID::BtnTextShowTot ||
        id == LogicalParameter::ID::BtnTextShowKps ||
        id == LogicalParameter::ID::BtnTextSepPosAdvMode ||
        id == LogicalParameter::ID::BtnTextVisPosition ||
        id == LogicalParameter::ID::BtnTextTotPosition ||
        id == LogicalParameter::ID::BtnTextKPSPosition ||
        id == LogicalParameter::ID::BtnTextBPMPosition ||

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
        (idU >= firstTextAdvPar && idU <= lastTextAdvPar) ||
        (idU >= firstGfxBtnAdvPar && idU <= lastGfxBtnAdvPar);
}