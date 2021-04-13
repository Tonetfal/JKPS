#include "../Headers/Statistics.hpp"

#include <SFML/Graphics.hpp>

#define SET_TEXT_STRING(x) mTexts.get(x).setString(mStrings.get(x) + ": " + \
                                            std::to_string(mLongs.get(x)));

Statistics::Statistics(sf::RenderWindow& window)
: mWindow(window)
, mTextFont()
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
                        , std::vector<int>& clickedKeys
                        , bool isChangeable  )
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


    for (size_t i = 0; i < Settings::KeyAmount; ++i)
    {
        if (mKeyCounters[i] == 0 || isChangeable)
            mKeyCountersText[i].setString(convertKeyToString(Settings::Keys[i]));
        else
            mKeyCountersText[i].setString(std::to_string(mKeyCounters[i]));
            
         
        setupTextPosition(i);
        mKeyCountersText[i].setCharacterSize(Settings::KeyCountersTextCharacterSize);
        while (mKeyCountersText[i].getLocalBounds().width > Settings::ButtonTextureSize.x)
            decreaseTextCharacterSize(i);
    }

}

void Statistics::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease)
        || sf::Keyboard::isKeyPressed(Settings::KeyToDecrease))
        {
            mKeyCounters.resize(Settings::KeyAmount);
            mKeyCountersText.resize(Settings::KeyAmount);

            if (sf::Keyboard::isKeyPressed(Settings::KeyToIncrease))
                setupLongVector(Settings::KeyAmount - 1);
            setupText(KPS);
            setupText(MaxKPS);
            setupText(TotalKeys);
            setupText(BPM);
            setupTextVector();
            setFonts();
        }
    }
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
    mWindow.draw(mTexts.get(BPM));

    for (auto& element : mKeyCountersText)
        mWindow.draw(element);
}



void Statistics::loadFonts(FontHolder& font)
{
    mTextFont = &font.get(Fonts::Main);
}

void Statistics::setFonts()
{
    mTexts.get(KPS).setFont(*mTextFont);
    mTexts.get(MaxKPS).setFont(*mTextFont);
    mTexts.get(TotalKeys).setFont(*mTextFont);
    mTexts.get(BPM).setFont(*mTextFont);

    for (auto& element : mKeyCountersText)
        element.setFont(*mTextFont);
}

void Statistics::reset()
{
    mLongs.get(KPS) = mLongs.get(MaxKPS) = 0;

    for (auto& element : mKeyCounters)
        element = 0;
}

void Statistics::decreaseTextCharacterSize(int index)
{
    mKeyCountersText[index].setCharacterSize
                            (mKeyCountersText[index].getCharacterSize() - 1);
}