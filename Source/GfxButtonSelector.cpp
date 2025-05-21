#include "../Headers/GfxButtonSelector.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Window/Clipboard.hpp>
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

    auto realKeyGfx = std::make_unique<GfxParameter>(nullptr, "Key", 0, sf::Vector2f(150, 25));
    realKeyGfx->setPosition(mWindowSize.x / 2, 25);
    mButtons[RealKeyButton] = std::move(realKeyGfx);
    
    auto visualKeyGfx = std::make_unique<GfxParameter>(nullptr, "Visual key", 0, sf::Vector2f(250, 25));
    visualKeyGfx->setPosition(mWindowSize.x / 2, 75);
    // Make the text gray in order to show that it is a hint, not an actual text
    visualKeyGfx->mValText.setFillColor(mDefaultVisualKeyColor);
    mButtons[VisualKeyButton] = std::move(visualKeyGfx);

    auto acceptButton = std::make_unique<GfxParameter>(nullptr, "True");
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
    auto event = sf::Event();
    while (mWindow.pollEvent(event))
    {
        auto handleExit = [this] ()
            {
                deselect();
                mWindow.close();
            };
        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.control && key.code == Settings::KeyExit)
            {
                handleExit();
            }
        }

        if (event.type == sf::Event::Closed)
            handleExit();

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

        auto str = static_cast<std::string>(mSelectedBtn->mValText.getString());

        const auto key = event.key;

        if (mSelectedBtn == mButtons[RealKeyButton].get())
        {
            const auto visualKeyChanged = mButtons[VisualKeyButton]->mValText.getString() != 
                keyToStr(strToKey(std::string(mButtons[RealKeyButton]->mValText.getString())));

            mButtons[RealKeyButton]->mValText.setString(key.code != sf::Keyboard::Unknown ? keyToStr(key.code, true) : "Unknown");
            mButtons[RealKeyButton]->setupValPos();

            if (!visualKeyChanged)
            {
                mButtons[VisualKeyButton]->mValText.setString(key.code != sf::Keyboard::Unknown ? keyToStr(key.code) : "Unknown");
                mButtons[VisualKeyButton]->setupValPos();
            }
            deselect();
            return;
        }

        auto strChanged = false;
        if (key.code == sf::Keyboard::Backspace)
        {
            if (mSelectedBtnTextIndex != 0)
            {
                rmChOnIdx(str, mSelectedBtnTextIndex - 1);
                --mSelectedBtnTextIndex;
            }
            strChanged = true;
        }
        else if (key.code == sf::Keyboard::Delete)
        {
            if (str.size() > mSelectedBtnTextIndex)
                rmChOnIdx(str, mSelectedBtnTextIndex);
            strChanged = true;
        }
        else if (key.code == sf::Keyboard::Left)
        {
            if (mSelectedBtnTextIndex > 0)
                --mSelectedBtnTextIndex;
        }
        else if (key.code == sf::Keyboard::Right)
        {
            if (str.size() > mSelectedBtnTextIndex)
                ++mSelectedBtnTextIndex;
        }
		else if (key.code == sf::Keyboard::Home)
		{
			mSelectedBtnTextIndex = 0;
		}
        else if (key.code == sf::Keyboard::End)
        {
            mSelectedBtnTextIndex = str.size();
        }
     
        if (isCharacter(key.code))
        {
            const auto maxLength = 20ul;
            if (mButtons[VisualKeyButton]->mValText.getString().getSize() >= maxLength)
                return;

            if (key.control && key.code == sf::Keyboard::V)
            {
                const auto clipboardStr = std::string(sf::Clipboard::getString());
                const auto lhs = std::string(str.begin(), str.begin() + mSelectedBtnTextIndex);
                const auto rhs = std::string(str.begin() + mSelectedBtnTextIndex, str.end());
                const auto newStr = lhs + clipboardStr + rhs;

                if (maxLength >= newStr.length())
                {
                    str = newStr;
                    mSelectedBtnTextIndex += static_cast<int>(clipboardStr.length());
                    strChanged = true;
                }
            }
            else
            {
                addChOnIdx(str, mSelectedBtnTextIndex, enumKeyToStr(key.code));
                ++mSelectedBtnTextIndex;
                strChanged = true;
            }
        }

        // If user starts to write anything, then change text color and delete the hint
        if (mButtons[VisualKeyButton]->mValText.getFillColor() == mDefaultVisualKeyColor && strChanged)
        {
            mButtons[VisualKeyButton]->mValText.setFillColor(sf::Color::White);
            mSelectedBtn->mValText.setString("");
            str = "";
            if (isCharacter(key.code))
                str += enumKeyToStr(key.code);
            mSelectedBtnTextIndex = str.size();
        }
        mSelectedBtn->mValText.setString(str);
        mSelectedBtn->setupValPos();
        setCursorPos();
    }

    if (event.type == sf::Event::MouseButtonPressed)
    {
        const auto button = event.mouseButton.button;
        if (mSelectedBtn == mButtons[RealKeyButton].get() && mKeyType == Mouse)
        {
            const auto visualKeyChanged = mButtons[VisualKeyButton]->mValText.getString() != mButtons[RealKeyButton]->mValText.getString();

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
        const auto button = event.mouseButton.button;
        for (auto &elem : mButtons)
        {
            const auto mousePos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow));

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
        const auto key = event.key.code;
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

void GfxButtonSelector::openWindow()
{
    if (!mWindow.isOpen())
    {
        const auto title = std::string(mKeyType == Keyboard ? "Keyboard key selector" : "Mouse button selector");

        sf::Uint32 style;
#ifdef _WIN32
        style = sf::Style::Close;
#elif linux
        style = sf::Style::Default;
#else
#error Unsupported compiler
#endif

        mWindow.create(sf::VideoMode(300u, 150u), title, style);
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

    static auto text = sf::Text();
    static auto chSz = sf::Vector2f();
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
    auto x = static_cast<float>(mSelectedBtn->getPosition().x - mSelectedBtn->mRect.getSize().x / 2 + 
        (mSelectedBtn->mRect.getSize().x - mSelectedBtn->mValText.getLocalBounds().width) / 2 +
        mSelectedBtnTextIndex * (chSz.x - text.getLetterSpacing() * 2));
    auto y = static_cast<float>(mCursor.getPosition().y);

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
