#include "../Headers/KeySelector.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"
#include "../Headers/StringHelper.hpp"

#include <SFML/Window/Event.hpp>

#include <cassert>
#include <stdexcept>
#include <iostream>


sf::RectangleShape KeySelector::mCursor(sf::Vector2f(1, 21));
int KeySelector::mSelectedBtnTextIndex(-1);
GraphicalParameter *KeySelector::mSelectedBtn(nullptr);
const std::string KeySelector::mDefaultVisualKeyStr("Visual key");
const sf::Color KeySelector::mDefaultVisualKeyColor(sf::Color(160, 160, 160));

KeySelector::KeySelector()
: mWindowSize(300, 200)
, mMainWindow(nullptr)
, mKey(nullptr)
, mButton(nullptr)
{
    if (!mFont.loadFromMemory(RobotoMono, 1100000))
        throw std::runtime_error("KeySelector::KeySelector - Failed to load default font");

    std::unique_ptr<GraphicalParameter> realKeyGfx(new GraphicalParameter("Key", 0, sf::Vector2f(150, 25)));
    realKeyGfx->setPosition(mWindowSize.x / 2, 25);
    mButtons[RealKeyButton] = std::move(realKeyGfx);
    
    std::unique_ptr<GraphicalParameter> visualKeyGfx(new GraphicalParameter("Visual key", 0, sf::Vector2f(250, 25)));
    visualKeyGfx->setPosition(mWindowSize.x / 2, 75);
    // Make the text gray in order to show that it is a hint, not an actual text
    visualKeyGfx->mValText.setFillColor(mDefaultVisualKeyColor);
    mButtons[VisualKeyButton] = std::move(visualKeyGfx);

    std::unique_ptr<GraphicalParameter> acceptButton(new GraphicalParameter("True"));
    acceptButton->setPosition(mWindowSize.x / 2, 125);
    mButtons[AcceptButton] = std::move(acceptButton);

    mCursor.setOutlineThickness(1);
    mCursor.setFillColor(sf::Color::White);
    mCursor.setOutlineColor(sf::Color::Black);
    mCursor.setOrigin(mCursor.getSize() / 2.f);
    // set visual key button height since it is the only one gfx button where the cursor is needed
    mCursor.setPosition(0, mButtons[VisualKeyButton]->getPosition().y); 
}

void KeySelector::handleOwnInput()
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
        }
        handleButtonModificationEvent(event);
        handleButtonInteractionEvent(event);
    }
}

void KeySelector::handleButtonModificationEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {    
        if (!mSelectedBtn || (mSelectedBtn == mButtons[RealKeyButton].get() 
        &&  mKeyType == Mouse))
            return;
        std::string str = static_cast<std::string>(mSelectedBtn->mValText.getString());

        sf::Keyboard::Key key = event.key.code;

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

void KeySelector::handleButtonInteractionEvent(sf::Event event)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        sf::Mouse::Button button = event.mouseButton.button;
        for (auto &elem : mButtons)
        {
            sf::FloatRect buttonBounds = elem->getTransform().transformRect(elem->getGlobalBounds());
            sf::Vector2f mousePos(sf::Mouse::getPosition(mWindow));

            if (button == sf::Mouse::Left && buttonBounds.contains(mousePos))
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
        sf::Keyboard::Key key = event.key.code;
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

void KeySelector::render()
{
    mWindow.clear(sf::Color(45,45,45));

    for (const auto &elem : mButtons)
        mWindow.draw(*elem);
    if (mSelectedBtnTextIndex != -1 && mSelectedBtn != mButtons[RealKeyButton].get()
    &&  mButtons[VisualKeyButton]->mValText.getFillColor() != mDefaultVisualKeyColor)
        mWindow.draw(mCursor);

    mWindow.display();
}

void KeySelector::setKey(LogicalKey *key, LogicalButton *button)
{
    assert(key || button);
    // only one pointer can be nullptr
    assert((key && !button) || (!key && button));
    mKey = nullptr;
    mButton = nullptr;

    if (key)
    {
        assert(strToKey(key->realStr) != sf::Keyboard::Unknown);
        mKey = key;
        mKeyType = Keyboard;
    }
    else
    {
        mButton = button;
        mKeyType = Mouse;
    }

    if (mKey)
    {
        mButtons[RealKeyButton]->mValText.setString(mKey->realStr);
        mButtons[VisualKeyButton]->mValText.setString(mKey->visualStr);
        resetVisualKeyGfxButton(mKey->realStr, mKey->visualStr);
    }

    if (mButton)
    {
        mButtons[RealKeyButton]->mValText.setString(mButton->realStr);
        mButtons[VisualKeyButton]->mValText.setString(mButton->visualStr);
        resetVisualKeyGfxButton(mButton->realStr, mButton->visualStr);
    }

    mButtons[RealKeyButton]->setupValPos();
    mButtons[VisualKeyButton]->setupValPos();
    select(mButtons[RealKeyButton].get());
}

void KeySelector::openWindow()
{
    if (!mWindow.isOpen())
    {
        std::string title = mKey ? "Keyboard key selector" : "Mouse button selector";
        mWindow.create(sf::VideoMode(300, 150), title, sf::Style::Close);
        mWindow.setPosition(mMainWindow->getPosition());
    }
}

void KeySelector::closeWindow()
{
    mWindow.close();
    mKey = nullptr;
    mButton = nullptr;
}

bool KeySelector::isOpen()
{
    return mWindow.isOpen();
}

void KeySelector::setMainWindowPointer(sf::RenderWindow *mainWindow)
{
    assert(mainWindow);
    mMainWindow = mainWindow;
}

void KeySelector::select(GraphicalParameter *ptr)
{
    mSelectedBtn = ptr;
    // Make the text gray in order to show that it is a hint, not an actual text
    mSelectedBtn->mRect.setFillColor(GraphicalParameter::defaultSelectedRectColor);
    mSelectedBtnTextIndex = mSelectedBtn->mValText.getString().getSize();
    setCursorPos();
}

void KeySelector::deselect()
{
    if (mSelectedBtn == nullptr)
        return;

    if (mSelectedBtn == mButtons[VisualKeyButton].get() 
    && mButtons[VisualKeyButton]->mValText.getString() == "")
    {
        resetVisualKeyGfxButton("", "");
    }

    mSelectedBtn->mRect.setFillColor(GraphicalParameter::defaultRectColor);
    mSelectedBtn = nullptr;
    mSelectedBtnTextIndex = -1;
}

void KeySelector::saveKey()
{
    std::string realKeyStr(mButtons[RealKeyButton]->mValText.getString());
    std::string visualKeyStr(mButtons[VisualKeyButton]->mValText.getString());
    if (mKey)
    {
        mKey->realStr = realKeyStr;
        mKey->visualStr = visualKeyStr;
        mKey->key = strToKey(realKeyStr);
        mKey = nullptr;
    }
    if (mButton)
    {
        mButton->realStr = realKeyStr;
        mButton->visualStr = visualKeyStr;
        mButton->button = strToBtn(realKeyStr);
        mButton = nullptr;
    }
    deselect();
}

void KeySelector::setCursorPos()
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

void KeySelector::resetVisualKeyGfxButton(const std::string &str1, const std::string &str2)
{
    if (str1 == str2)
    {
        std::string strToStr;
        if (mKey)
            strToStr = keyToStr(strToKey(std::string(mButtons[RealKeyButton]->mValText.getString())));
        else
            strToStr = mButtons[RealKeyButton]->mValText.getString();

        mButtons[VisualKeyButton]->mValText.setString(strToStr);
    }
    mButtons[VisualKeyButton]->mValText.setFillColor(mDefaultVisualKeyColor);
    mButtons[VisualKeyButton]->setupValPos();
}

bool KeySelector::isCharacter(sf::Keyboard::Key key)
{
    return 
        (key >= sf::Keyboard::A         && key <= sf::Keyboard::Num9)
    ||  (key >= sf::Keyboard::LBracket  && key <= sf::Keyboard::Space)
    ||  (key >= sf::Keyboard::Add       && key <= sf::Keyboard::Divide)
    ||  (key >= sf::Keyboard::Numpad0   && key <= sf::Keyboard::Numpad9);
}
