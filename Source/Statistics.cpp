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
    if (Settings::ShowStatisticsText)
    {
        setupLong(KPS);
        setupLong(MaxKPS);
        setupLong(TotalKeys);
        
        setupText(KPS);
        setupText(MaxKPS);
        setupText(TotalKeys);

        setupString(KPS, "KPS");
        setupString(MaxKPS, "Max KPS");
        setupString(TotalKeys, "Total keys");
    }

    if (Settings::ShowBPMText)
    {
        setupLong(BPM);
        setupText(BPM);
        setupLongVector();
        setupString(BPM, "Current BPM");
        setupTextVector();
        setupStringVector();
    }
}

void Statistics::update(  std::size_t KeyPerSecond
                        , size_t BeatsPerMinute
                        , std::vector<int>& clickedKeys )
{
    if (Settings::ShowStatisticsText)
    {
        mLongs.get(KPS) = KeyPerSecond;
        if (mLongs.get(KPS) > mLongs.get(MaxKPS))
            mLongs.get(MaxKPS) = mLongs.get(KPS);

        for (auto& element : clickedKeys)
            mLongs.get(TotalKeys) += element;
    }
    if (Settings::ShowBPMText)
        mLongs.get(BPM) = BeatsPerMinute;


    if (Settings::ShowStatisticsText)
    {
        SET_TEXT_STRING(KPS);
        SET_TEXT_STRING(MaxKPS);
        SET_TEXT_STRING(TotalKeys);
    }
    if (Settings::ShowBPMText)
        SET_TEXT_STRING(BPM);


    // Update counters on the buttons
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        // Display keys
        if (mKeyCounters[i] == 0 || Settings::IsChangeable || Settings::ShowSetKeysText)
        {
            if (i < Settings::KeyAmount)
                mKeyCountersText[i].setString(convertKeyToString(
                    Settings::Keys[i]).c_str());
            else
                mKeyCountersText[i].setString(convertButtonToString(
                    Settings::MouseButtons[i - Settings::KeyAmount]).c_str());
        }
        // Display clicks amount
        else if (Settings::ShowKeyCountersText)
            mKeyCountersText[i].setString(std::to_string(mKeyCounters[i]).c_str());
        else   
            mKeyCountersText[i].setString("");

        setupTextPosition(i);
        if (Settings::ShowKeyCountersText)
        {
            mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
            while (mKeyCountersText[i].getLocalBounds().width > Settings::ButtonTextureSize.x)
                decreaseTextCharacterSize(i);
        }
    }

}

void Statistics::handleEvent(sf::Event event)
{
    mKeyCounters.resize(Settings::ButtonAmount);
    mKeyCountersText.resize(Settings::ButtonAmount);

    if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease))
        setupLongVector(Settings::ButtonAmount - 1);
        
    if (Settings::ShowStatisticsText)
    {
        setupText(KPS);
        setupText(MaxKPS);
        setupText(TotalKeys);
    }
    if (Settings::ShowBPMText)
    {
        setupText(BPM);
        setupTextVector();
        setFonts();
    }
}

void Statistics::handleInput(KeyPressingManager& container)
{
    for (size_t i = 0; i < container.mClickedKeys.size(); ++i)
        mKeyCounters[i] += container.mClickedKeys[i];
}

void Statistics::draw()
{
    if (Settings::ShowStatisticsText)
    {
        mWindow.draw(mTexts.get(KPS));
        mWindow.draw(mTexts.get(MaxKPS));
        mWindow.draw(mTexts.get(TotalKeys));
        if (Settings::ShowBPMText)
            mWindow.draw(mTexts.get(BPM));
    }

    if (Settings::ShowBPMText)
    {
        for (auto& element : mKeyCountersText)
        {
            mWindow.draw(element);
        }
    }
}


void Statistics::loadFonts(FontHolder& font)
{
    mStatisticsFont = &font.get(Fonts::Statistics);
    mKeyCountersFont = &font.get(Fonts::KeyCounters);
}

void Statistics::setFonts()
{
    if (Settings::ShowStatisticsText)
    {
        mTexts.get(KPS).setFont(*mStatisticsFont);
        mTexts.get(MaxKPS).setFont(*mStatisticsFont);
        mTexts.get(TotalKeys).setFont(*mStatisticsFont);
        mTexts.get(BPM).setFont(*mStatisticsFont);
    }
    if (Settings::ShowBPMText)
    {
        for (auto& element : mKeyCountersText)
            element.setFont(*mKeyCountersFont);
    }
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
    for (auto& element : mKeyCountersText)
        element.setFillColor(Settings::KeyCountersTextColor);

    if (buttonIndex != -1)
        mKeyCountersText[buttonIndex].setFillColor(Settings::HighlightedKeyColor);
}