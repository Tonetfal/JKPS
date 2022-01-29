#include "../Headers/GfxButtonSelector.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"
#include "../Headers/StringHelper.hpp"

#include <SFML/Window/Event.hpp>

#include <cassert>
#include <stdexcept>
#include <iostream>


sf::RectangleShape GfxButtonSelector::mCursor(sf::Vector2f(1, 21));
int GfxButtonSelector::mSelectedBtnTextIndex(-1);
GfxParameter *GfxButtonSelector::mSelectedBtn(nullptr);
const std::string GfxButtonSelector::mDefaultVisualKeyStr("Visual key");
const sf::Color GfxButtonSelector::mDefaultVisualKeyColor(sf::Color(160, 160, 160));

GfxButtonSelector::GfxButtonSelector()
: mWindowSize(300, 200)
, mLogKey(nullptr)
{
    if (!mFont.loadFromMemory(RobotoMono, 1100000))
        throw std::runtime_error("KeySelector::KeySelector - Failed to load default font");

    std::unique_ptr<GfxParameter> realKeyGfx(new GfxParameter(nullptr, "Key", 0, sf::Vector2f(150, 25)));
    realKeyGfx->setPosition(mWindowSize.x / 2, 25);
    mButtons[RealKeyButton] = std::move(realKeyGfx);
    
    std::unique_ptr<GfxParameter> visualKeyGfx(new GfxParameter(nullptr, "Visual key", 0, sf::Vector2f(250, 25)));
    visualKeyGfx->setPosition(mWindowSize.x / 2, 75);
    // Make the text gray in order to show that it is a hint, not an actual text
    visualKeyGfx->mValText.setFillColor(mDefaultVisualKeyColor);
    mButtons[VisualKeyButton] = std::move(visualKeyGfx);

    std::unique_ptr<GfxParameter> acceptButton(new GfxParameter(nullptr, "True"));
    acceptButton->setPosition(mWindowSize.x / 2, 125);
    mButtons[AcceptButton] = std::move(acceptButton);

    mCursor.setOutlineThickness(1);
    mCursor.setFillColor(sf::Color::White);
    mCursor.setOutlineColor(sf::Color::Black);
    mCursor.setOrigin(mCursor.getSize() / 2.f);
    // set visual key button height since it is the only one gfx button where the cursor is needed
    mCursor.setPosition(0, mButtons[VisualKeyButton]->getPosition().y); 
}

void GfxButtonSelector::handleOwnInput()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed
        || (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
        &&  sf::Keyboard::isKeyPressed(sf::Keyboard::W)))
        {
            deselect();
            mWindow.close();
            return;
        }
        handleButtonModificationEvent(event);
        handleButtonInteractionEvent(event);
    }
}

void GfxButtonSelector::handleButtonModificationEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {    
        if (!mSelectedBtn || (mSelectedBtn == mButtons[RealKeyButton].get() 
        &&  mKeyType == Mouse))
            return;
        std::string str = static_cast<std::string>(mSelectedBtn->mValText.getString());

        const sf::Keyboard::Key key = event.key.code;

        if (mSelectedBtn == mButtons[RealKeyButton].get())
        {
            bool visualKeyChanged = mButtons[VisualKeyButton]->mValText.getString() != 
                keyToStr(strToKey(std::string(mButtons[RealKeyButton]->mValText.getString())));

            mButtons[RealKeyButton]->mValText.setString(key != sf::Keyboard::Unknown ? keyToStr(key, true) : "Unknown");
            mButtons[RealKeyButton]->setupValPos();

            if (!visualKeyChanged)
            {
                mButtons[VisualKeyButton]->mValText.setString(key != sf::Keyboard::Unknown ? keyToStr(key) : "Unknown");
                mButtons[VisualKeyButton]->setupValPos();
            }
            deselect();
            return;
        }

        const unsigned maxLength = 20;
        if (mButtons[VisualKeyButton]->mValText.getString().getSize() >= maxLength)
            return;

        bool strChanged = false;
        if (key == sf::Keyboard::Backspace)
        {
            if (mSelectedBtnTextIndex != 0)
            {
                rmChOnIdx(str, mSelectedBtnTextIndex - 1);
                --mSelectedBtnTextIndex;
            }
            strChanged = true;
        }

        if (key == sf::Keyboard::Delete)
        {
            if (str.size() > mSelectedBtnTextIndex)
                rmChOnIdx(str, mSelectedBtnTextIndex);
            strChanged = true;
        }
        
        if (key == sf::Keyboard::Left)
        {
            if (mSelectedBtnTextIndex > 0)
                --mSelectedBtnTextIndex;
        }

        if (key == sf::Keyboard::Right)
        {
            if (str.size() > mSelectedBtnTextIndex)
                ++mSelectedBtnTextIndex;
        }

        if (key == sf::Keyboard::Home)
        {
            mSelectedBtnTextIndex = 0;
        }

        if (key == sf::Keyboard::End)
        {
            mSelectedBtnTextIndex = str.size();
        }

        if (isCharacter(key))
        {
            addChOnIdx(str, mSelectedBtnTextIndex, enumKeyToStr(key));
            ++mSelectedBtnTextIndex;
            strChanged = true;
        }

        // If user starts to write anything, then change text color and delete the hint
        if (mButtons[VisualKeyButton]->mValText.getFillColor() == mDefaultVisualKeyColor && strChanged)
        {
            mButtons[VisualKeyButton]->mValText.setFillColor(sf::Color::White);
            mSelectedBtn->mValText.setString("");
            str = "";
            if (isCharacter(key))
                str += enumKeyToStr(key);
            mSelectedBtnTextIndex = str.size();
        }
        mSelectedBtn->mValText.setString(str);
        mSelectedBtn->setupValPos();
        setCursorPos();
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::Mouse::Button button = event.mouseButton.button;
        if (mSelectedBtn == mButtons[RealKeyButton].get() && mKeyType == Mouse)
        {
            bool visualKeyChanged = mButtons[VisualKeyButton]->mValText.getString() != mButtons[RealKeyButton]->mValText.getString();

            mButtons[RealKeyButton]->mValText.setString(btnToStr(button));
            mButtons[RealKeyButton]->setupValPos();

            if (!visualKeyChanged)
            {
                mButtons[VisualKeyButton]->mValText.setString(mButtons[RealKeyButton]->mValText.getString());
                mButtons[VisualKeyButton]->setupValPos();
            }
            deselect();
            return;
        }
    }
}

void GfxButtonSelector::handleButtonInteractionEvent(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        const sf::Mouse::Button button = event.mouseButton.button;
        for (auto &elem : mButtons)
        {
            const sf::Vector2f mousePos(sf::Mouse::getPosition(mWindow));

            if (button == sf::Mouse::Left && elem->contains(mousePos))
            {
                if (elem == mButtons[VisualKeyButton] 
                &&  mButtons[VisualKeyButton]->mValText.getFillColor() == mDefaultVisualKeyColor)
                {
                    mButtons[VisualKeyButton]->mValText.setString("");
                }
                if (elem == mButtons[AcceptButton])
                {
                    saveKey();
                    mWindow.close();
                    return;
                }
                deselect();
                select(elem.get());
            } 
            else if ((button == sf::Mouse::Left || button == sf::Mouse::Right) 
            && elem.get() == mSelectedBtn)
            {
                deselect();
            }
        }
    }
    if (event.type == sf::Event::KeyPressed)
    {
        const sf::Keyboard::Key key = event.key.code;
        if (key == sf::Keyboard::Escape)
        {
            deselect();
        }
        if (mSelectedBtn != mButtons[RealKeyButton].get() && key == sf::Keyboard::Enter)
        {
            saveKey();
            mWindow.close();
            return;
        }
    }
}

void GfxButtonSelector::render()
{
    mWindow.clear(sf::Color(45,45,45));

    for (const auto &elem : mButtons)
        mWindow.draw(*elem);
    if (mSelectedBtnTextIndex != -1 && mSelectedBtn != mButtons[RealKeyButton].get()
    &&  mButtons[VisualKeyButton]->mValText.getFillColor() != mDefaultVisualKeyColor)
        mWindow.draw(mCursor);

    mWindow.display();
}

void GfxButtonSelector::setKey(LogKey *logKey)
{
    assert(logKey);

    mLogKey = logKey;
    mKeyType = mLogKey->keyboardKey ? Keyboard : Mouse;

    mButtons[RealKeyButton]->mValText.setString(mLogKey->realStr);
    mButtons[VisualKeyButton]->mValText.setString(mLogKey->visualStr);
    resetVisualKeyGfxButton(mLogKey->realStr, mLogKey->visualStr);

    mButtons[RealKeyButton]->setupValPos();
    mButtons[VisualKeyButton]->setupValPos();
    select(mButtons[RealKeyButton].get());
}

void GfxButtonSelector::openWindow(sf::Vector2i spawnPosition)
{
    if (!mWindow.isOpen())
    {
        std::string title = mKeyType == Keyboard ? "Keyboard key selector" : "Mouse button selector";
        // spawnPosition -= sf::Vector2i(150, 75);
        mWindow.create(sf::VideoMode(300, 150), title, sf::Style::Default);
        // mWindow.setPosition(spawnPosition);
        mWindow.requestFocus();
    }
}

bool GfxButtonSelector::isOpen()
{
    return mWindow.isOpen();
}

void GfxButtonSelector::select(GfxParameter *ptr)
{
    mSelectedBtn = ptr;
    // Make the text gray in order to show that it is a hint, not an actual text
    mSelectedBtn->mRect.setFillColor(GfxParameter::defaultSelectedRectColor);
    mSelectedBtnTextIndex = mSelectedBtn->mValText.getString().getSize();
    setCursorPos();
}

void GfxButtonSelector::deselect()
{
    if (mSelectedBtn == nullptr)
        return;

    if (mSelectedBtn == mButtons[VisualKeyButton].get() 
    && mButtons[VisualKeyButton]->mValText.getString() == "")
    {
        resetVisualKeyGfxButton("", "");
    }

    mSelectedBtn->mRect.setFillColor(GfxParameter::defaultRectColor);
    mSelectedBtn = nullptr;
    mSelectedBtnTextIndex = -1;
}

void GfxButtonSelector::saveKey()
{
    mLogKey->realStr = mButtons[RealKeyButton]->mValText.getString();
    mLogKey->visualStr = mButtons[VisualKeyButton]->mValText.getString();

    switch(mKeyType)
    {
        case Keyboard:
            *mLogKey->keyboardKey = strToKey(mLogKey->realStr);
            break;
        case Mouse:
            *mLogKey->mouseButton = strToBtn(mLogKey->realStr);
            break;
    }

    mLogKey->changed = true;
    mLogKey = nullptr;
    deselect();
}

void GfxButtonSelector::setCursorPos()
{
    if (!mSelectedBtn)
        return;

    static sf::Text text;
    static sf::Vector2f chSz;
    if (text.getFont() == nullptr)
    {
        text.setFont(mFont);
        text.setString("0");
        chSz.x = text.getLocalBounds().width;
        chSz.y = text.getLocalBounds().height;
    }

    // Take absolute position of the center of the button, substract by half width - the cursor is on the left bound,
    // then find the width of the part on the text left, and add it - the cursor is on the text left,
    // then take space in X axes for each character, substract it by 2 times spacing between them, 
    // and multiply by current cursor index - the cursor is on the index left
    float x = mSelectedBtn->getPosition().x - mSelectedBtn->mRect.getSize().x / 2 + 
        (mSelectedBtn->mRect.getSize().x - mSelectedBtn->mValText.getLocalBounds().width) / 2 +
        mSelectedBtnTextIndex * (chSz.x - text.getLetterSpacing() * 2);
    float y = mCursor.getPosition().y;

    mCursor.setPosition(x, y);
}

void GfxButtonSelector::resetVisualKeyGfxButton(const std::string &str1, const std::string &str2)
{
    if (str1 == str2)
    {
        std::string strToStr;
        switch(mKeyType)
        {
            case Keyboard: 
                strToStr = keyToStr(strToKey(std::string(mButtons[RealKeyButton]->mValText.getString()))); 
                break;

            case Mouse: 
                strToStr = mButtons[RealKeyButton]->mValText.getString(); 
                break;
        }
            
        mButtons[VisualKeyButton]->mValText.setString(strToStr);
    }
    mButtons[VisualKeyButton]->mValText.setFillColor(mDefaultVisualKeyColor);
    mButtons[VisualKeyButton]->setupValPos();
}

bool GfxButtonSelector::isCharacter(sf::Keyboard::Key key)
{
    return 
        (key >= sf::Keyboard::A         && key <= sf::Keyboard::Num9)
    ||  (key >= sf::Keyboard::LBracket  && key <= sf::Keyboard::Space)
    ||  (key >= sf::Keyboard::Add       && key <= sf::Keyboard::Divide)
    ||  (key >= sf::Keyboard::Numpad0   && key <= sf::Keyboard::Numpad9);
}
