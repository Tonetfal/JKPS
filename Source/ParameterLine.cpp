#include "../Headers/ParameterLine.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/ResourceIdentifiers.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/GfxButtonSelector.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/GfxButton.hpp"
#include "../Headers/ConfigHelper.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>
#include <cmath>
#include <mutex>


sf::RectangleShape ParameterLine::mCursor(sf::Vector2f(1, 21));
std::shared_ptr<LogicalParameter> ParameterLine::mSelectedParameter(nullptr);
std::shared_ptr<ParameterLine> ParameterLine::mSelectedLine(nullptr);
std::shared_ptr<GfxParameter> ParameterLine::mSelectedValue(nullptr);
int ParameterLine::mSelectedValueIndex(-1);
Palette ParameterLine::mPalette(0);
bool ParameterLine::mRefresh(false);

ParameterLine::ParameterLine(
    std::shared_ptr<LogicalParameter> parameter,
    const FontHolder &fonts, 
    const TextureHolder &textures,
    sf::RenderWindow &window)
: mWindow(window)
, mType(parameter->mType)
, mParameter(parameter)
, mRectLine(sf::Vector2f(785.f, 40.f))
, mColorButtonP(nullptr)
, paramValWasChanged(false)
, mIsThRunning(false)
{
    mRectLine.setFillColor(pickColor(mType));
    mParameterName.setString(parameter->mParName);
    mParameterName.setFont(fonts.get(Fonts::Parameter));
    setCharacterSize(20);

    buildButtons(parameter->getValStr(), fonts, textures);
    buildLimits(fonts);

    // If it wasn't still initialized
    if (mCursor.getPosition() == sf::Vector2f())
    {
        mCursor.setOutlineThickness(1);
        mCursor.setFillColor(sf::Color::White);
        mCursor.setOutlineColor(sf::Color::Black);
        mCursor.setOrigin(mCursor.getSize() / 2.f);
        mCursor.setPosition(0, mRectLine.getSize().y / 2);
    }
}

bool ParameterLine::handleEvent(sf::Event event)
{
    bool isSelected = handleButtonsInteractionEvent(event);
    bool wasModified = handleValueModEvent(event);
    return isSelected || wasModified;
}

// Keyboard events which modify values
bool ParameterLine::handleValueModEvent(sf::Event event)
{
    // // Allow mouse wheel affect the values on scroll
    // if (event.type == sf::Event::MouseWheelScrolled)
    // {
    //     // Create new event and swap it with the original one
    //     sf::Event e;
    //     e.type = sf::Event::KeyPressed;
    //     e.key.code = (event.mouseWheelScroll.delta > 0 
    //         ? sf::Keyboard::Up : sf::Keyboard::Down);
    //     event = e;
    // }

    if (event.type == sf::Event::KeyPressed)
    {
        if (mSelectedValue == nullptr || !isSelectedValHere()) 
            return false;

        auto str = static_cast<std::string>(mSelectedValue->mValText.getString());
        auto key = event.key.code;
        bool isStrType = mType == LogicalParameter::Type::String || mType == LogicalParameter::Type::StringPath;
        int btnIdx = 0;
        if (mType == LogicalParameter::Type::Color
        ||  mType == LogicalParameter::Type::VectorU
        ||  mType == LogicalParameter::Type::VectorI
        ||  mType == LogicalParameter::Type::VectorF)
        {
            btnIdx = -1;
            for (unsigned i = 0; i < mParameterValues.size(); ++i)
            {
                if (mParameterValues[i] == mSelectedValue)
                {
                    btnIdx = i;
                    break;
                }
            }
            assert(btnIdx != -1);
            if (str != "-0")
                str = mParameter->getValPt(btnIdx);
        }

        if (!isStrType
        && ((key >= sf::Keyboard::Num0 &&  key <= sf::Keyboard::Num9)
        || (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9)))
        {
            int n = 0;
            if (key >= sf::Keyboard::Num0 &&  key <= sf::Keyboard::Num9)
                n = (key - sf::Keyboard::Num0);
            if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9)
                n = (key - sf::Keyboard::Numpad0);

            if (mSelectedValueIndex == 0 && (str[0] == '-' || n == 0))
                return true;

            if (str.size() == 1 && str[0] == '0'
            ||  str.size() == 2 && str[1] == '0' && str[0] == '-')
            {
                str.back() = n + '0';
                if (mSelectedValueIndex == str.size() - 1)
                    ++mSelectedValueIndex;
            }
            else
            {
                std::string prevVal = str;
                addChOnIdx(str, mSelectedValueIndex, n + '0');
                float check = stof(str); 
                str = std::to_string(static_cast<int>(check));

                if (checkLimits(check))
                {  
                    runThread(str, prevVal);
                    --mSelectedValueIndex;
                }
                ++mSelectedValueIndex;
            }
        }

        if (!isStrType && key == sf::Keyboard::Up)
        {
            std::string prevVal = str;
            float check = stof(str) + 1; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);
            mSelectedValueIndex += str.length() == prevVal.length() ? 0 : str.length() > prevVal.length() ? 1 : -1;
        }

        if (!isStrType && key == sf::Keyboard::Down)
        {
            std::string prevVal = str;
            float check = stof(str) - 1; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);
            mSelectedValueIndex += str.length() == prevVal.length() ? 0 : str.length() > prevVal.length() ? 1 : -1;
        }

        if (!isStrType && (key == sf::Keyboard::Hyphen || key == sf::Keyboard::Subtract))
        {
            if (mType == LogicalParameter::Type::Unsigned || mType == LogicalParameter::Type::Color)
                return true;

            if (str[0] == '-')
            {
                rmChOnIdx(str, 0);
                --mSelectedValueIndex;
            } else
            {
                addChOnIdx(str, 0, '-');
                ++mSelectedValueIndex;
            }
        }

        if (key == sf::Keyboard::Backspace)
        {
            if (mSelectedValueIndex != 0)
            {
                // "0" or "-0"
                if ((!isStrType && str.size() == 1)
                ||  (str.size() == 2 && str[0] == '-' && mSelectedValueIndex == 2))
                {
                    str = "0";
                    mSelectedValueIndex = 1;
                }
                else
                {
                    rmChOnIdx(str, mSelectedValueIndex - 1);
                    if (!isStrType &&std::stoi(str) == 0)
                    {
                        // ex.: -500, remove 5, result "-0", if 500, then "0"
                        str = (str[0] == '-' ? "-0" : "0");
                    }
                    --mSelectedValueIndex;
                }
            }
        }
        
        if (key == sf::Keyboard::Delete)
        {
            if (str.size() > mSelectedValueIndex)
            {
                // "0" or "-0"
                if ((!isStrType && str.size() == 1) 
                ||  (!isStrType && str.size() == 2 && str[0] == '-' && mSelectedValueIndex == 0))
                {
                    str = "0";
                    mSelectedValueIndex = 0;
                }
                else
                {
                    rmChOnIdx(str, mSelectedValueIndex);
                    if (!isStrType &&std::stoi(str) == 0)
                    {
                        // ex.: -500, remove 5, result "-0", if 500, then "0"
                        str = (str[0] == '-' ? "-0" : "0");
                    }
                    --mSelectedValueIndex;
                }
            }
        }

        if (key == sf::Keyboard::Left)
        {
            if (mSelectedValueIndex > 0)
                --mSelectedValueIndex;
        }

        if (key == sf::Keyboard::Right)
        {
            if (str.size() > mSelectedValueIndex)
                ++mSelectedValueIndex;
        }

        if (key == sf::Keyboard::Home)
        {
            mSelectedValueIndex = 0;
        }

        if (key == sf::Keyboard::End)
        {
            mSelectedValueIndex = str.size();
        }

        if (isStrType && GfxButtonSelector::isCharacter(key))
        {
            unsigned maxLength = 50;
            if (str.size() < maxLength)
            {
                addChOnIdx(str, mSelectedValueIndex, enumKeyToStr(key));
                ++mSelectedValueIndex;
            }
        }

        mParameter->setValStr(str, btnIdx);
        mSelectedValue->mValText.setString(str);
        paramValWasChanged = true;

        mSelectedValue->setupValPos();
        setCursorPos();

        // Doesn't work
        // If values were changed in menu, then they must be also changed on palette
        // if (mPalette.isWindowOpen())
        //     mPalette.setColorOnPalette(lineToColor(mSelectedLine));
        return true;
    }
    return false;
}

bool ParameterLine::handleButtonsInteractionEvent(sf::Event event)
{
    // Mouse/keyboard events to control buttons selection/deselection
    if (event.type == sf::Event::MouseButtonPressed 
    ||  event.type == sf::Event::KeyPressed)
    {
        const auto halfWindowSize = static_cast<sf::Vector2f>(mWindow.getSize()) / 2.f;
        const auto viewCenter = mWindow.getView().getCenter();
        const auto viewOffset = viewCenter - halfWindowSize;
        const auto relMousePos = sf::Mouse::getPosition(mWindow);
        const auto absMousePos = static_cast<sf::Vector2f>(relMousePos) + viewOffset;
        const auto button = event.mouseButton.button;
        const auto key = event.key.code;
        bool ret = false;

        // Check click on buttons
        for (const auto &elem : mParameterValues)
        {
            auto contains = elem->contains(absMousePos);
            if (contains)
                ret = true;

            if (event.type == sf::Event::MouseButtonPressed 
            && (button == sf::Mouse::Left && contains))
            {
                if (mType == LogicalParameter::Type::Bool)
                {
                    mParameter->setValStr(LogicalParameter::getInverseBool(mParameter->getBool()));
                    elem->mValText.setString(mParameter->getValStr());
                    elem->setInverseMark();
                    deselect();
                    ret = false;
                }
                // Refresh button has 0x0 rectangle shape 
                else if (mType == LogicalParameter::Type::StringPath && elem->mRect.getSize().x == 0)
                {
                    mRefresh = true;
                    deselect();
                    // return in order to don't select refresh button
                    ret = false; 
                }
                else if (mType != LogicalParameter::Type::Color && mPalette.isWindowOpen())
                    mPalette.closeWindow();
                else
                {
                    deselect();
                    select(elem);
                    ret = true;
                }
            }
            if (event.type == sf::Event::KeyPressed && key == sf::Keyboard::Enter)
            {
                // Refresh button has 0x0 rectangle shape 
                if (mType == LogicalParameter::Type::StringPath && elem->mRect.getSize().x == 0)
                {
                    mRefresh = true;
                    deselect();
                    // return in order to don't select refresh button
                    ret = false; 
                }
            }

            if ((event.type == sf::Event::KeyPressed && (key == sf::Keyboard::Escape || key == sf::Keyboard::Enter)) 
            ||  (event.type == sf::Event::MouseButtonPressed && button == sf::Mouse::Right))
                ret = false;

            if (event.type == sf::Event::KeyPressed && key == sf::Keyboard::Tab)
            {
                if (tabulation())
                {
                    // return in order to avoid further tabulation and segmentation fault
                    ret = true;
                    break;
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && button == sf::Mouse::Left 
        &&  mType == LogicalParameter::Type::Color)
            ret |= selectRgbCircle(button, absMousePos);

        return ret;
    }
    return false;
}

bool ParameterLine::tabulation()
{
    if (isItSelectedLine(shared_from_this()))
    {
        for (auto it = mParameterValues.begin(); it < mParameterValues.end(); ++it)
        {
            if (*it == mSelectedValue)
            {
                deselect();
                std::shared_ptr<GfxParameter> tabOn = nullptr;

                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    // tab forward
                    tabOn = (it + 1 != mParameterValues.end() ? *(it + 1) : mParameterValues[0]);
                else
                    // tab backward
                    tabOn = (it == mParameterValues.begin() ? mParameterValues.back() : *(it - 1));

                select(tabOn);
                return true;
            }
        }
    }
    return false;
}

bool ParameterLine::selectRgbCircle(sf::Mouse::Button button, sf::Vector2f mousePos)
{
    const sf::Vector2f circleOrigin(mColorButtonP->getOrigin());
    const sf::Vector2f circlePosition(mColorButtonP->getPosition());
    sf::FloatRect circleRect(mColorButtonP->rgbCircleSprite.getGlobalBounds());
    circleRect.left = getPosition().x + circlePosition.x - circleOrigin.x;
    circleRect.top = getPosition().y + circlePosition.y - circleOrigin.y;

    if (circleRect.contains(mousePos))
    {
        // If there is already selected line, but it isn't this one
        if (mSelectedLine && !isItSelectedLine(shared_from_this()))
            deselect();
        // If nothing is selected
        if (!mSelectedLine)
            select(mParameterValues[0]);

        // mPalette.setColorOnPalette(lineToColor(mSelectedLine));
        mPalette.openWindow(mWindow.getPosition() + static_cast<sf::Vector2i>(mWindow.getSize() / 2U));
        return true;
    }
    return false;
}

void ParameterLine::processInput()
{
    if (mType == LogicalParameter::Type::Color)
        mPalette.processInput();
}

void ParameterLine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    target.draw(mRectLine, states);
    target.draw(mParameterName, states);
    target.draw(mLimits, states);
    for (const auto &elem : mParameterValues)
        target.draw(*elem, states);
    
    if (mSelectedValue && isSelectedValHere())
        target.draw(mCursor, states);

    if (mType == LogicalParameter::Type::Color)
    {
        target.draw(*mColorButtonP, states);
        if (mPalette.isWindowOpen() && this == mSelectedLine.get())
            mPalette.render();
    }
}

void ParameterLine::buildButtons(const std::string &valueStr, const FontHolder &fonts, const TextureHolder &textures)
{
    GfxParameter::mTextures = &textures;
    GfxParameter::mFonts = &fonts;

    std::shared_ptr<GfxParameter> val = nullptr; 
    if (mType == LogicalParameter::Type::Bool)
    {
        val = std::make_shared<GfxParameter>(this, readValue(valueStr, 0) == "True");
        val->setPosition(sf::Vector2f(mRectLine.getSize().x - 
            GfxParameter::getPosX(), mRectLine.getSize().y / 2));
        mParameterValues.emplace_back(std::move(val));
        return;
    }

    if (mType == LogicalParameter::Type::String || mType == LogicalParameter::Type::StringPath)
    {
        val = std::make_shared<GfxParameter>(this, readValue(valueStr, 0), 0, sf::Vector2f(265.f, 25.f));
        val->setPosition(598.f, mRectLine.getSize().y / 2.f);

        mParameterValues.emplace_back(std::move(val));

        if (mType == LogicalParameter::Type::StringPath)
        {
            val = std::make_shared<GfxParameter>(this);
            val->setPosition(765.f, mRectLine.getSize().y / 2.f);

            mParameterValues.emplace_back(std::move(val));
        }
        return;
    }

    for (unsigned i = 0; i < readAmountOfParms(valueStr); ++i)
    {
        val = std::make_shared<GfxParameter>(this, readValue(valueStr, i), i);

        val->setPosition(val->getPosition() + sf::Vector2f(mRectLine.getSize().x - 
            GfxParameter::getPosX(), mRectLine.getSize().y / 2));
        
        if (mType == LogicalParameter::Type::Color && i == 0)
        {
            float dist = 65.f;
            mColorButtonP = std::make_unique<ColorButton>(textures.get(Textures::rgbCircle));
            mColorButtonP->setPosition(val->getPosition().x - dist, mRectLine.getSize().y / 2);
            mColorButtonP->setOrigin(static_cast<sf::Vector2f>(mColorButtonP->rgbCircleSprite.getTexture()->getSize()) / 2.f);
        }

        mParameterValues.emplace_back(std::move(val));
    }
}

void ParameterLine::buildLimits(const FontHolder &fonts)
{
    if (mParameter->mLowLimits != mParameter->mHighLimits)
    {
        mLimits.setFont(fonts.get(Fonts::Parameter));
        mLimits.setCharacterSize(10);
        mLimits.setOrigin(mLimits.getLocalBounds().left, mLimits.getLocalBounds().top);
        mLimits.setPosition(mParameterName.getLocalBounds().left, mParameterName.getLocalBounds().top + 
            mParameterName.getLocalBounds().height + 4.f);
        mLimits.setString("Available values: " + std::to_string(static_cast<int>(mParameter->mLowLimits)) + 
            " - " + std::to_string(static_cast<int>(mParameter->mHighLimits)));
    }
}


void ParameterLine::select(std::shared_ptr<GfxParameter> ptr)
{
    mSelectedValue = ptr;
    mSelectedParameter = mParameter;
    mSelectedLine = shared_from_this();
    mSelectedValue->mRect.setFillColor(GfxParameter::defaultSelectedRectColor);
    mSelectedValueIndex = mSelectedValue->mValText.getString().getSize();
    setCursorPos();

    if (mSelectedParameter->mParName == "Buttons text bounds")
        GfxButton::setShowBounds(true);
}

void ParameterLine::deselect()
{
    if (mSelectedValue == nullptr && mSelectedLine == nullptr)
        return;

    mSelectedValue->mRect.setFillColor(GfxParameter::defaultRectColor);
    if (mSelectedParameter->mParName == "Buttons text bounds")
        GfxButton::setShowBounds(false);
    mSelectedParameter = nullptr;
    mSelectedLine = nullptr;
    mSelectedValue = nullptr;
    mSelectedValueIndex = -1;
}

bool ParameterLine::isSelectedValHere() const
{
    for (const auto &elem : mParameterValues)
        if (elem == mSelectedValue)
            return true;

    return false;
}

bool ParameterLine::isItSelectedLine(const std::shared_ptr<ParameterLine> val) const
{
    return mSelectedLine == val;
}

void ParameterLine::setCursorPos()
{
    static sf::Text text;
    static sf::Vector2f chSz;
    if (text.getFont() == nullptr)
    {
        text.setFont(*mSelectedValue->mValText.getFont());
        text.setString("0");
        chSz.x = text.getLocalBounds().width;
        chSz.y = text.getLocalBounds().height;
    }

    // Take absolute position of the center of the button, substract by half width - the cursor is on the left bound,
    // then find the width of the part on the text left, and add it - the cursor is on the text left,
    // then take space in X axes for each character, substract it by 2 times spacing between them, 
    // and multiply by current cursor index - the cursor is on the index left
    float x = mSelectedValue->getPosition().x - mSelectedValue->mRect.getSize().x / 2 + 
        (mSelectedValue->mRect.getSize().x - mSelectedValue->mValText.getLocalBounds().width) / 2 +
        mSelectedValueIndex * (chSz.x - text.getLetterSpacing() * 2);
    float y = mCursor.getPosition().y;

    mCursor.setPosition(x, y);
}

sf::Color ParameterLine::pickColor(LogicalParameter::Type type) const
{
    switch(type)
    {
        case LogicalParameter::Type::Empty: return sf::Color::Transparent;
        case LogicalParameter::Type::Collection: return sf::Color(25,25,25);
        case LogicalParameter::Type::Hint: return sf::Color(150, 0, 0);
        default: return sf::Color(30,30,30);
    }
}

sf::Color ParameterLine::lineToColor(const std::shared_ptr<ParameterLine> linePtr) const
{
    assert(linePtr->mType == LogicalParameter::Type::Color);

    sf::Uint8 r(std::stoi(static_cast<std::string>(linePtr->mParameterValues[0]->mValText.getString())));
    sf::Uint8 g(std::stoi(static_cast<std::string>(linePtr->mParameterValues[1]->mValText.getString())));
    sf::Uint8 b(std::stoi(static_cast<std::string>(linePtr->mParameterValues[2]->mValText.getString())));
    sf::Uint8 a(std::stoi(static_cast<std::string>(linePtr->mParameterValues[3]->mValText.getString())));

    return { r, g, b, a };
}

bool ParameterLine::checkLimits(float check) const
{
    return check < mParameter->mLowLimits || check > mParameter->mHighLimits;
}

void ParameterLine::runThread(std::string &curVal, const std::string &prevVal)
{
    if (!mIsThRunning)
    {
        mWarningTh = std::thread(ParameterLine::warningVisualization, &mIsThRunning, this);
        mWarningTh.detach();
    }
    curVal = prevVal;
}

void ParameterLine::warningVisualization(bool *isRunning, ParameterLine *parLine)
{
    *isRunning = true;
    
    std::mutex mtx;
    std::shared_ptr<GfxParameter> gfxPar = mSelectedValue;
    sf::Color red(sf::Color(170,0,0));
    sf::Clock clock;
    sf::Time elapsedTime = sf::Time::Zero, totalTime = sf::Time::Zero;
    sf::Time timeToChange = sf::milliseconds(500);

    mtx.lock();
    // gfxPar->mValText.setFillColor(sf::Color::Red);
    gfxPar->mRect.setFillColor(red);
    mtx.unlock();

    while (totalTime.asMilliseconds() < timeToChange.asMilliseconds() * 4)
    {
        elapsedTime += clock.restart();

        if (elapsedTime >= timeToChange)
        {
            elapsedTime -= timeToChange;
            totalTime += timeToChange;

            mtx.lock();

            // gfxPar->mValText.setFillColor(gfxPar->mValText.getFillColor() == sf::Color::White ? 
            //     sf::Color::Red : sf::Color::White);
            gfxPar->mRect.setFillColor(gfxPar->mRect.getFillColor() != red ? red :
                gfxPar == mSelectedValue ? GfxParameter::defaultSelectedRectColor : GfxParameter::defaultRectColor);

            mtx.unlock();
        }
    }
    mtx.lock();
    gfxPar->mRect.setFillColor(gfxPar == mSelectedValue ? 
        GfxParameter::defaultSelectedRectColor : GfxParameter::defaultRectColor);
    mtx.unlock();

    *isRunning = false;
}

const std::shared_ptr<LogicalParameter> ParameterLine::getParameter() const
{
    return mParameter;
}

bool ParameterLine::resetState()
{
    return paramValWasChanged && !(paramValWasChanged = false);
}

void ParameterLine::setCharacterSize(unsigned size)
{
    mParameterName.setCharacterSize(size);
    // Don't let text go beyound 1.8 of the button width
    while (
       mType != LogicalParameter::Type::Collection 
    && mType != LogicalParameter::Type::Empty 
    && mType != LogicalParameter::Type::Hint
    && mParameterName.getLocalBounds().width >= mRectLine.getSize().x / 1.8)
    {
        mParameterName.setCharacterSize(mParameterName.getCharacterSize() - 1);
    }
}

void ParameterLine::setColor(sf::Color color)
{
    assert(mSelectedLine->mType == LogicalParameter::Type::Color);

    if (mSelectedLine->mType == LogicalParameter::Type::Color)
    {
        // Change Parameter
        mSelectedLine->mParameter->setColor(color);
        mSelectedLine->paramValWasChanged = true;

        // Change GraphicalParameter text
        mSelectedLine->mParameterValues[0]->mValText.setString(std::to_string(static_cast<unsigned>(color.r)));
        mSelectedLine->mParameterValues[1]->mValText.setString(std::to_string(static_cast<unsigned>(color.g)));
        mSelectedLine->mParameterValues[2]->mValText.setString(std::to_string(static_cast<unsigned>(color.b)));
        mSelectedLine->mParameterValues[0]->setupValPos();
        mSelectedLine->mParameterValues[1]->setupValPos();
        mSelectedLine->mParameterValues[2]->setupValPos();

        // Change cursor position
        for (auto &elem : mSelectedLine->mParameterValues)
        {
            if (elem == mSelectedValue)
            {
                mSelectedValueIndex = elem->mValText.getString().getSize();
                setCursorPos();
            }
        }
    }
}

LogicalParameter::Type ParameterLine::getType() const
{
    return mType;
}


ParameterLine::ID ParameterLine::parIdToParLineId(LogicalParameter::ID id)
{
    switch(id)
    {
        case LogicalParameter::ID::StatTextDist: return ParameterLine::ID::StatTextDist;
        case LogicalParameter::ID::StatPos: return ParameterLine::ID::StatPos;
        case LogicalParameter::ID::StatValPos: return ParameterLine::ID::StatValPos;
        case LogicalParameter::ID::StatTextFont: return ParameterLine::ID::StatTextFont;
        case LogicalParameter::ID::StatTextClr: return ParameterLine::ID::StatTextClr;
        case LogicalParameter::ID::StatTextChSz: return ParameterLine::ID::StatTextChSz;
        case LogicalParameter::ID::StatTextOutThck: return ParameterLine::ID::StatTextOutThck;
        case LogicalParameter::ID::StatTextOutClr: return ParameterLine::ID::StatTextOutClr;
        case LogicalParameter::ID::StatTextBold: return ParameterLine::ID::StatTextBold;
        case LogicalParameter::ID::StatTextItal: return ParameterLine::ID::StatTextItal;
        case LogicalParameter::ID::StatTextShow: return ParameterLine::ID::StatTextShow;
        case LogicalParameter::ID::StatTextShowKPS: return ParameterLine::ID::StatTextShowKPS;
        case LogicalParameter::ID::StatTextShowTotal: return ParameterLine::ID::StatTextShowTotal;
        case LogicalParameter::ID::StatTextShowBPM: return ParameterLine::ID::StatTextShowBPM;
        case LogicalParameter::ID::StatTextPosAdvMode: return ParameterLine::ID::StatTextPosAdvMode;
        case LogicalParameter::ID::StatTextPos1: return ParameterLine::ID::StatTextPos1;
        case LogicalParameter::ID::StatTextPos2: return ParameterLine::ID::StatTextPos2;
        case LogicalParameter::ID::StatTextPos3: return ParameterLine::ID::StatTextPos3;
        case LogicalParameter::ID::StatTextValPosAdvMode: return ParameterLine::ID::StatTextValPosAdvMode;
        case LogicalParameter::ID::StatTextValPos1: return ParameterLine::ID::StatTextValPos1;
        case LogicalParameter::ID::StatTextValPos2: return ParameterLine::ID::StatTextValPos2;
        case LogicalParameter::ID::StatTextValPos3: return ParameterLine::ID::StatTextValPos3;
        case LogicalParameter::ID::StatTextClrAdvMode: return ParameterLine::ID::StatTextClrAdvMode;
        case LogicalParameter::ID::StatTextClr1: return ParameterLine::ID::StatTextClr1;
        case LogicalParameter::ID::StatTextClr2: return ParameterLine::ID::StatTextClr2;
        case LogicalParameter::ID::StatTextClr3: return ParameterLine::ID::StatTextClr3;
        case LogicalParameter::ID::StatTextChSzAdvMode: return ParameterLine::ID::StatTextChSzAdvMode;
        case LogicalParameter::ID::StatTextChSz1: return ParameterLine::ID::StatTextChSz1;
        case LogicalParameter::ID::StatTextChSz2: return ParameterLine::ID::StatTextChSz2;
        case LogicalParameter::ID::StatTextChSz3: return ParameterLine::ID::StatTextChSz3;
        case LogicalParameter::ID::StatTextBoldAdvMode: return ParameterLine::ID::StatTextBoldAdvMode;
        case LogicalParameter::ID::StatTextBold1: return ParameterLine::ID::StatTextBold1;
        case LogicalParameter::ID::StatTextBold2: return ParameterLine::ID::StatTextBold2;
        case LogicalParameter::ID::StatTextBold3: return ParameterLine::ID::StatTextBold3;
        case LogicalParameter::ID::StatTextItalAdvMode: return ParameterLine::ID::StatTextItalAdvMode;
        case LogicalParameter::ID::StatTextItal1: return ParameterLine::ID::StatTextItal1;
        case LogicalParameter::ID::StatTextItal2: return ParameterLine::ID::StatTextItal2;
        case LogicalParameter::ID::StatTextItal3: return ParameterLine::ID::StatTextItal3;
        case LogicalParameter::ID::StatTextKPSText: return ParameterLine::ID::StatTextKPSText;
        case LogicalParameter::ID::StatTextKPS2Text: return ParameterLine::ID::StatTextKPS2Text;
        case LogicalParameter::ID::StatTextTotalText: return ParameterLine::ID::StatTextTotalText;
        case LogicalParameter::ID::StatTextBPMText: return ParameterLine::ID::StatTextBPMText;

        case LogicalParameter::ID::BtnTextFont: return ParameterLine::ID::BtnTextFont;
        case LogicalParameter::ID::BtnTextClr: return ParameterLine::ID::BtnTextClr;
        case LogicalParameter::ID::BtnTextChSz: return ParameterLine::ID::BtnTextChSz;
        case LogicalParameter::ID::BtnTextOutThck: return ParameterLine::ID::BtnTextOutThck;
        case LogicalParameter::ID::BtnTextOutClr: return ParameterLine::ID::BtnTextOutClr;
        case LogicalParameter::ID::BtnTextPosition: return ParameterLine::ID::BtnTextPosition;
        case LogicalParameter::ID::BtnTextBounds: return ParameterLine::ID::BtnTextBounds;
        case LogicalParameter::ID::BtnTextIgnoreBtnMovement: return ParameterLine::ID::BtnTextIgnoreBtnMovement;
        case LogicalParameter::ID::BtnTextBold: return ParameterLine::ID::BtnTextBold;
        case LogicalParameter::ID::BtnTextItal: return ParameterLine::ID::BtnTextItal;
        case LogicalParameter::ID::BtnTextShowVisKeys: return ParameterLine::ID::BtnTextShowVisKeys;
        case LogicalParameter::ID::BtnTextShowTot: return ParameterLine::ID::BtnTextShowTot;
        case LogicalParameter::ID::BtnTextShowKps: return ParameterLine::ID::BtnTextShowKps;
        case LogicalParameter::ID::BtnTextShowBpm: return ParameterLine::ID::BtnTextShowBpm;

        case LogicalParameter::ID::BtnTextSepPosAdvMode: return ParameterLine::ID::BtnTextSepPosAdvMode;
        case LogicalParameter::ID::BtnTextVisPosition: return ParameterLine::ID::BtnTextVisPosition;
        case LogicalParameter::ID::BtnTextTotPosition: return ParameterLine::ID::BtnTextTotPosition;
        case LogicalParameter::ID::BtnTextKPSPosition: return ParameterLine::ID::BtnTextKPSPosition;
        case LogicalParameter::ID::BtnTextBPMPosition: return ParameterLine::ID::BtnTextBPMPosition;
        case LogicalParameter::ID::BtnTextPosAdvMode: return ParameterLine::ID::BtnTextPosAdvMode;
        case LogicalParameter::ID::BtnTextPos1:  return ParameterLine::ID::BtnTextPos1;
        case LogicalParameter::ID::BtnTextPos2:  return ParameterLine::ID::BtnTextPos2;
        case LogicalParameter::ID::BtnTextPos3:  return ParameterLine::ID::BtnTextPos3;
        case LogicalParameter::ID::BtnTextPos4:  return ParameterLine::ID::BtnTextPos4;
        case LogicalParameter::ID::BtnTextPos5:  return ParameterLine::ID::BtnTextPos5;
        case LogicalParameter::ID::BtnTextPos6:  return ParameterLine::ID::BtnTextPos6;
        case LogicalParameter::ID::BtnTextPos7:  return ParameterLine::ID::BtnTextPos7;
        case LogicalParameter::ID::BtnTextPos8:  return ParameterLine::ID::BtnTextPos8;
        case LogicalParameter::ID::BtnTextPos9:  return ParameterLine::ID::BtnTextPos9;
        case LogicalParameter::ID::BtnTextPos10: return ParameterLine::ID::BtnTextPos10;
        case LogicalParameter::ID::BtnTextPos11: return ParameterLine::ID::BtnTextPos11;
        case LogicalParameter::ID::BtnTextPos12: return ParameterLine::ID::BtnTextPos12;
        case LogicalParameter::ID::BtnTextPos13: return ParameterLine::ID::BtnTextPos13;
        case LogicalParameter::ID::BtnTextPos14: return ParameterLine::ID::BtnTextPos14;
        case LogicalParameter::ID::BtnTextPos15: return ParameterLine::ID::BtnTextPos15;

        case LogicalParameter::ID::BtnGfxDist: return ParameterLine::ID::BtnGfxDist;
        case LogicalParameter::ID::BtnGfxTxtr: return ParameterLine::ID::BtnGfxTxtr;
        case LogicalParameter::ID::BtnGfxTxtrSz: return ParameterLine::ID::BtnGfxTxtrSz;
        case LogicalParameter::ID::BtnGfxTxtrClr: return ParameterLine::ID::BtnGfxTxtrClr;
        case LogicalParameter::ID::BtnGfxBtnPosAdvMode: return ParameterLine::ID::BtnGfxBtnPosAdvMode;
        case LogicalParameter::ID::BtnGfxSzAdvMode: return ParameterLine::ID::BtnGfxSzAdvMode;
        case LogicalParameter::ID::BtnGfxBtnPos1: return ParameterLine::ID::BtnGfxBtnPos1;
        case LogicalParameter::ID::BtnGfxSz1: return ParameterLine::ID::BtnGfxSz1;
        case LogicalParameter::ID::BtnGfxBtnPos2: return ParameterLine::ID::BtnGfxBtnPos2;
        case LogicalParameter::ID::BtnGfxSz2: return ParameterLine::ID::BtnGfxSz2;
        case LogicalParameter::ID::BtnGfxBtnPos3: return ParameterLine::ID::BtnGfxBtnPos3;
        case LogicalParameter::ID::BtnGfxSz3: return ParameterLine::ID::BtnGfxSz3;
        case LogicalParameter::ID::BtnGfxBtnPos4: return ParameterLine::ID::BtnGfxBtnPos4;
        case LogicalParameter::ID::BtnGfxSz4: return ParameterLine::ID::BtnGfxSz4;
        case LogicalParameter::ID::BtnGfxBtnPos5: return ParameterLine::ID::BtnGfxBtnPos5;
        case LogicalParameter::ID::BtnGfxSz5: return ParameterLine::ID::BtnGfxSz5;
        case LogicalParameter::ID::BtnGfxBtnPos6: return ParameterLine::ID::BtnGfxBtnPos6;
        case LogicalParameter::ID::BtnGfxSz6: return ParameterLine::ID::BtnGfxSz6;
        case LogicalParameter::ID::BtnGfxBtnPos7: return ParameterLine::ID::BtnGfxBtnPos7;
        case LogicalParameter::ID::BtnGfxSz7: return ParameterLine::ID::BtnGfxSz7;
        case LogicalParameter::ID::BtnGfxBtnPos8: return ParameterLine::ID::BtnGfxBtnPos8;
        case LogicalParameter::ID::BtnGfxSz8: return ParameterLine::ID::BtnGfxSz8;
        case LogicalParameter::ID::BtnGfxBtnPos9: return ParameterLine::ID::BtnGfxBtnPos9;
        case LogicalParameter::ID::BtnGfxSz9: return ParameterLine::ID::BtnGfxSz9;
        case LogicalParameter::ID::BtnGfxBtnos10: return ParameterLine::ID::BtnGfxBtnos10;
        case LogicalParameter::ID::BtnGfxSz10: return ParameterLine::ID::BtnGfxSz10;
        case LogicalParameter::ID::BtnGfxBtnos11: return ParameterLine::ID::BtnGfxBtnos11;
        case LogicalParameter::ID::BtnGfxSz11: return ParameterLine::ID::BtnGfxSz11;
        case LogicalParameter::ID::BtnGfxBtnos12: return ParameterLine::ID::BtnGfxBtnos12;
        case LogicalParameter::ID::BtnGfxSz12: return ParameterLine::ID::BtnGfxSz12;
        case LogicalParameter::ID::BtnGfxBtnos13: return ParameterLine::ID::BtnGfxBtnos13;
        case LogicalParameter::ID::BtnGfxSz13: return ParameterLine::ID::BtnGfxSz13;
        case LogicalParameter::ID::BtnGfxBtnos14: return ParameterLine::ID::BtnGfxBtnos14;
        case LogicalParameter::ID::BtnGfxSz14: return ParameterLine::ID::BtnGfxSz14;
        case LogicalParameter::ID::BtnGfxBtnos15: return ParameterLine::ID::BtnGfxBtnos15;
        case LogicalParameter::ID::BtnGfxSz15: return ParameterLine::ID::BtnGfxSz15;
        
        case LogicalParameter::ID::AnimGfxLight: return ParameterLine::ID::AnimGfxLight;
        case LogicalParameter::ID::AnimGfxTxtr: return ParameterLine::ID::AnimGfxTxtr;
        case LogicalParameter::ID::AnimGfxVel: return ParameterLine::ID::AnimGfxVel;
        case LogicalParameter::ID::AnimGfxScl: return ParameterLine::ID::AnimGfxScl;
        case LogicalParameter::ID::AnimGfxClr: return ParameterLine::ID::AnimGfxClr;
        
        case LogicalParameter::ID::AnimGfxPress: return ParameterLine::ID::AnimGfxPress;
        case LogicalParameter::ID::AnimGfxOffset: return ParameterLine::ID::AnimGfxOffset;

        case LogicalParameter::ID::BgTxtr: return ParameterLine::ID::BgTxtr;
        case LogicalParameter::ID::BgClr: return ParameterLine::ID::BgClr;
        case LogicalParameter::ID::BgScale: return ParameterLine::ID::BgScale;

        case LogicalParameter::ID::MainWndwTitleBar: return ParameterLine::ID::MainWndwTitleBar;
        case LogicalParameter::ID::MainWndwTop: return ParameterLine::ID::MainWndwTop;
        case LogicalParameter::ID::MainWndwBot: return ParameterLine::ID::MainWndwBot;
        case LogicalParameter::ID::MainWndwLft: return ParameterLine::ID::MainWndwLft;
        case LogicalParameter::ID::MainWndwRght: return ParameterLine::ID::MainWndwRght;

        case LogicalParameter::ID::KPSWndwEn: return ParameterLine::ID::KPSWndwEn;
        case LogicalParameter::ID::KPSWndwSz: return ParameterLine::ID::KPSWndwSz;
        case LogicalParameter::ID::KPSWndwTxtChSz: return ParameterLine::ID::KPSWndwTxtChSz;
        case LogicalParameter::ID::KPSWndwNumChSz: return ParameterLine::ID::KPSWndwNumChSz;
        case LogicalParameter::ID::KPSWndwBgClr: return ParameterLine::ID::KPSWndwBgClr;
        case LogicalParameter::ID::KPSWndwTxtClr: return ParameterLine::ID::KPSWndwTxtClr;
        case LogicalParameter::ID::KPSWndwNumClr: return ParameterLine::ID::KPSWndwNumClr;
        case LogicalParameter::ID::KPSWndwTxtFont: return ParameterLine::ID::KPSWndwTxtFont;
        case LogicalParameter::ID::KPSWndwNumFont: return ParameterLine::ID::KPSWndwNumFont;
        case LogicalParameter::ID::KPSWndwTopPadding: return ParameterLine::ID::KPSWndwTopPadding;
        case LogicalParameter::ID::KPSWndwDistBtw: return ParameterLine::ID::KPSWndwDistBtw;

        case LogicalParameter::ID::KeyPressVisHint: return ParameterLine::ID::KeyPressVisHint;
        case LogicalParameter::ID::KeyPressVisToggle: return ParameterLine::ID::KeyPressVisToggle;
        case LogicalParameter::ID::KeyPressVisSpeed: return ParameterLine::ID::KeyPressVisSpeed; 
        case LogicalParameter::ID::KeyPressVisRotation: return ParameterLine::ID::KeyPressVisRotation;
        case LogicalParameter::ID::KeyPressVisOrig: return ParameterLine::ID::KeyPressVisOrig;
        case LogicalParameter::ID::KeyPressVisFadeLineLen: return ParameterLine::ID::KeyPressVisFadeLineLen;
        case LogicalParameter::ID::KeyPressVisColor: return ParameterLine::ID::KeyPressVisColor;

        case LogicalParameter::ID::OtherSaveStats: return ParameterLine::ID::OtherSaveStats;
        case LogicalParameter::ID::OtherMultpl: return ParameterLine::ID::OtherMultpl;

        case LogicalParameter::ID::SaveStatMaxKPS: return ParameterLine::ID::SaveStatMaxKPS;
        case LogicalParameter::ID::SaveStatTotal: return ParameterLine::ID::SaveStatTotal;
        case LogicalParameter::ID::SaveStatTotal1: return ParameterLine::ID::SaveStatTotal1;
        case LogicalParameter::ID::SaveStatTotal2: return ParameterLine::ID::SaveStatTotal2;
        case LogicalParameter::ID::SaveStatTotal3: return ParameterLine::ID::SaveStatTotal3;
        case LogicalParameter::ID::SaveStatTotal4: return ParameterLine::ID::SaveStatTotal4;
        case LogicalParameter::ID::SaveStatTotal5: return ParameterLine::ID::SaveStatTotal5;
        case LogicalParameter::ID::SaveStatTotal6: return ParameterLine::ID::SaveStatTotal6;
        case LogicalParameter::ID::SaveStatTotal7: return ParameterLine::ID::SaveStatTotal7;
        case LogicalParameter::ID::SaveStatTotal8: return ParameterLine::ID::SaveStatTotal8;
        case LogicalParameter::ID::SaveStatTotal9: return ParameterLine::ID::SaveStatTotal9;
        case LogicalParameter::ID::SaveStatTotal10: return ParameterLine::ID::SaveStatTotal10;
        case LogicalParameter::ID::SaveStatTotal11: return ParameterLine::ID::SaveStatTotal11;
        case LogicalParameter::ID::SaveStatTotal12: return ParameterLine::ID::SaveStatTotal12;
        case LogicalParameter::ID::SaveStatTotal13: return ParameterLine::ID::SaveStatTotal13;
        case LogicalParameter::ID::SaveStatTotal14: return ParameterLine::ID::SaveStatTotal14;
        case LogicalParameter::ID::SaveStatTotal15: return ParameterLine::ID::SaveStatTotal15;

        bool ForgotToAddParameter;
        default: 
            assert(!(ForgotToAddParameter = true)); 
            return ParameterLine::ID::StatTextColl;
    }
}

bool ParameterLine::isEmpty(ParameterLine::ID id)
{
    return
        // id == ParameterLine::ID::StatTextMty ||
        id == ParameterLine::ID::StatTextAdvMty ||
        // id == ParameterLine::ID::BtnTextMty ||
        id == ParameterLine::ID::BtnTextAdvMty ||
        // id == ParameterLine::ID::BtnGfxMty ||
        id == ParameterLine::ID::BtnGfxAdvMty ||
        // id == ParameterLine::ID::AnimGfxLightMty ||
        id == ParameterLine::ID::AnimGfxPressMty ||
        id == ParameterLine::ID::MainWndwMty ||
        id == ParameterLine::ID::KPSWndwMty ||
        id == ParameterLine::ID::KeyPressVisMty ||
        id == ParameterLine::ID::OtherMty ||
        // id == ParameterLine::ID::InfoMty ||
        id == ParameterLine::ID::LastLine;
}

void ParameterLine::deselectValue()
{
    if (mSelectedValue == nullptr && mSelectedLine == nullptr)
        return;

    mSelectedValue->mRect.setFillColor(GfxParameter::defaultRectColor);
    if (mSelectedParameter->mParName == "Buttons text bounds")
        GfxButton::setShowBounds(false);
    mSelectedParameter = nullptr;
    mSelectedLine = nullptr;
    mSelectedValue = nullptr;
    mSelectedValueIndex = -1;
}

bool ParameterLine::isValueSelected()
{
    return mSelectedValue.get();
}

bool ParameterLine::resetRefreshState()
{
    return mRefresh && !(mRefresh = false);
}
