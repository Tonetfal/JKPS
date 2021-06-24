#include "../Headers/Statistics.hpp"

#define SET_TEXT_STRING(x) mTexts.get(x).setString((mStrings.get(x) + ": " \
                                                 + std::to_string(mLongs.get(x))).c_str());


Statistics::Statistics(sf::RenderWindow& window)
: mWindow(window)
, mStatisticsFont()
, mKeyCountersFont()
, mTexts()
, mLongs()
, mStrings()
, mKeyCounters()
, mKeyCountersText()
{
    setupLong(KPS);
    setupLong(MaxKPS);
    setupLong(TotalKeys);
    setupLong(BPM);
    setupLongVector();
    
    setupText(KPS);
    setupText(MaxKPS);
    setupText(TotalKeys);
    setupText(BPM);
    setupTextVector();

    setupString(KPS, "KPS");
    setupString(MaxKPS, "Max KPS");
    setupString(TotalKeys, "Total keys");
    setupString(BPM, "Current BPM");
    setupStringVector();
}

void Statistics::update(  std::size_t KeyPerSecond
                        , size_t BeatsPerMinute
                        , std::vector<int>& clickedKeys )
{
    mLongs.get(KPS) = KeyPerSecond;
    mLongs.get(BPM) = BeatsPerMinute;

    if (mLongs.get(KPS) > mLongs.get(MaxKPS))
        mLongs.get(MaxKPS) = mLongs.get(KPS);

    for (auto& element : clickedKeys)
        mLongs.get(TotalKeys) += element;


    SET_TEXT_STRING(KPS);
    SET_TEXT_STRING(MaxKPS);
    SET_TEXT_STRING(TotalKeys);
    SET_TEXT_STRING(BPM);


    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        if (mKeyCounters[i] == 0 || Settings::IsChangeable)
        {
            if (i < Settings::KeyAmount)
                mKeyCountersText[i].setString(convertKeyToString(
                    Settings::Keys[i]).c_str());
            else
                mKeyCountersText[i].setString(convertButtonToString(
                    Settings::MouseButtons[i - Settings::KeyAmount]).c_str());
        }
        else
            mKeyCountersText[i].setString(std::to_string(mKeyCounters[i]).c_str());
         
        setupTextPosition(i);
        mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
        while (mKeyCountersText[i].getLocalBounds().width > Settings::ButtonTextureSize.x)
            decreaseTextCharacterSize(i);
    }

}

void Statistics::handleEvent(sf::Event event)
{
    
    mKeyCounters.resize(Settings::ButtonAmount);
    mKeyCountersText.resize(Settings::ButtonAmount);

    if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease))
        setupLongVector(Settings::ButtonAmount - 1);
        
    setupText(KPS);
    setupText(MaxKPS);
    setupText(TotalKeys);
    setupText(BPM);
    setupTextVector();
    setFonts();
}

void Statistics::handleInput(KeyPressingManager& container)
{
    for (size_t i = 0; i < container.mClickedKeys.size(); ++i)
        mKeyCounters[i] += container.mClickedKeys[i] ;
}

void Statistics::draw()
{
    mWindow.draw(mTexts.get(KPS));
    mWindow.draw(mTexts.get(MaxKPS));
    mWindow.draw(mTexts.get(TotalKeys));
    if (Settings::ShowBPMText)
        mWindow.draw(mTexts.get(BPM));

    for (auto& element : mKeyCountersText)
    {
        mWindow.draw(element);
    }
}



void Statistics::loadFonts(FontHolder& font)
{
    mStatisticsFont = &font.get(Fonts::Statistics);
    mKeyCountersFont = &font.get(Fonts::KeyCounters);
}

void Statistics::setFonts()
{
    mTexts.get(KPS).setFont(*mStatisticsFont);
    mTexts.get(MaxKPS).setFont(*mStatisticsFont);
    mTexts.get(TotalKeys).setFont(*mStatisticsFont);
    mTexts.get(BPM).setFont(*mStatisticsFont);

    for (auto& element : mKeyCountersText)
        element.setFont(*mKeyCountersFont);
}

void Statistics::reset()
{
    mLongs.get(KPS) = mLongs.get(MaxKPS) = 0;

    for (auto& element : mKeyCounters)
        element = 0;
}

void Statistics::decreaseTextCharacterSize(int index)
{
    mKeyCountersText[index].setCharacterSize(
        mKeyCountersText[index].getCharacterSize() - 1);
}

void Statistics::handleHighlight(int buttonIndex)
{
    // :(
    for (auto& element : mKeyCountersText)
        element.setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mKeyCountersText[buttonIndex].setFillColor(Settings::HighlightedKeyColor);
}