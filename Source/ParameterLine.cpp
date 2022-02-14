#include "../Headers/ParameterLine.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/ResourceIdentifiers.hpp"
#include "../Headers/StringHelper.hpp"
#include "../Headers/GfxButtonSelector.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/GfxButton.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/Utility.hpp"

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
, mRectLine(sf::Vector2f(785.f, 40.f))
, mParameter(parameter)
, mColorButtonP(nullptr)
, paramValWasChanged(false)
, mIsThRunning(false)
{
    mRectLine.setFillColor(pickColor(mType));
    mParameterName.setString(parameter->mParName);
    mParameterName.setFont(fonts.get(Fonts::Parameter));
    setCharacterSize(20u);

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
    // Don't handle events for hidden objects
    if (isHidden())
        return false;

    const auto isSelected = handleButtonsInteractionEvent(event);
    const auto wasModified = handleValueModEvent(event);
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
        auto isStrType = mType == LogicalParameter::Type::String || mType == LogicalParameter::Type::StringPath;
        auto btnIdx = 0;
        if (mType == LogicalParameter::Type::Color
        ||  mType == LogicalParameter::Type::VectorU
        ||  mType == LogicalParameter::Type::VectorI
        ||  mType == LogicalParameter::Type::VectorF)
        {
            btnIdx = -1;
            auto size = mParameterValues.size();
            for (auto i = 0ul; i < size; ++i)
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
        && ((key >= sf::Keyboard::Num0    && key <= sf::Keyboard::Num9)
         || (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9)))
        {
            int n = 0;
            if (key >= sf::Keyboard::Num0 &&  key <= sf::Keyboard::Num9)
                n = (key - sf::Keyboard::Num0);
            if (key >= sf::Keyboard::Numpad0 && key <= sf::Keyboard::Numpad9)
                n = (key - sf::Keyboard::Numpad0);

            if (mSelectedValueIndex == 0 && (str[0] == '-' || n == 0))
                return true;

            const auto strSize = str.size();
            if ((strSize == 1ul && str[0] == '0') || (strSize == 2ul && !std::isdigit(str[0]) && str[1] == '0'))
            {
                str.back() = n + '0';
                if (mSelectedValueIndex == strSize - 1ul)
                    ++mSelectedValueIndex;
            }
            else
            {
                const auto prevVal = str;
                addChOnIdx(str, mSelectedValueIndex, n + '0');
                const auto check = stof(str); 
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
            const auto prevVal = str;
            const auto check = stof(str) + 1.f; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);

            const auto strSize = str.size();
            const auto prevValSize = prevVal.size();
            mSelectedValueIndex += strSize == prevValSize ? 0 : strSize > prevValSize ? 1 : -1;
        }

        if (!isStrType && key == sf::Keyboard::Down)
        {
            const auto prevVal = str;
            const auto check = stof(str) - 1.f; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);

            const auto strSize = str.size();
            const auto prevValSize = prevVal.size();
            mSelectedValueIndex += strSize == prevValSize ? 0 : strSize > prevValSize ? 1 : -1;
        }

        if (!isStrType && (key == sf::Keyboard::Hyphen || key == sf::Keyboard::Subtract))
        {
            if (mType == LogicalParameter::Type::Unsigned 
            ||  mType == LogicalParameter::Type::Color 
            ||  mType == LogicalParameter::Type::VectorU)
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
                    if (!isStrType && std::stoi(str) == 0)
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
            const auto strSize = str.size();

            if (strSize > mSelectedValueIndex)
            {
                // "0" or "-0"
                if ((!isStrType && strSize == 1) 
                ||  (!isStrType && strSize == 2 && str[0] == '-' && mSelectedValueIndex == 0))
                {
                    str = "0";
                    mSelectedValueIndex = 0;
                }
                else
                {
                    rmChOnIdx(str, mSelectedValueIndex);
                    if (!isStrType && std::stoi(str) == 0)
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
            const auto maxLength = 50u;
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
        auto ret = false;

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
                auto tabOn = std::shared_ptr<GfxParameter>();

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
    const auto circleOrigin = mColorButtonP->getOrigin();
    const auto circlePosition = mColorButtonP->getPosition();
    auto circleRect = sf::FloatRect(mColorButtonP->rgbCircleSprite.getGlobalBounds());
    const auto position = getPosition();
    circleRect.left = position.x + circlePosition.x - circleOrigin.x;
    circleRect.top = position.y + circlePosition.y - circleOrigin.y;

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
    // Don't act upon hidden objects
    if (isHidden())
        return;

    if (mType == LogicalParameter::Type::Color)
        mPalette.processInput();
}

void ParameterLine::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // Don't draw hidden objects
    if (isHidden())
        return;

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

    auto val = std::shared_ptr<GfxParameter>(); 
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

    const auto count = readAmountOfParms(valueStr);
    for (auto i = 0ul; i < count; ++i)
    {
        val = std::make_shared<GfxParameter>(this, readValue(valueStr, i), i);

        val->setPosition(val->getPosition() + sf::Vector2f(mRectLine.getSize().x - 
            GfxParameter::getPosX(), mRectLine.getSize().y / 2));
        
        if (mType == LogicalParameter::Type::Color && i == 0ul)
        {
            const auto dist = 65.f;
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
        mLimits.setCharacterSize(10u);
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

    const auto &parName = mSelectedParameter->mParName;
    if (parName == "Text bounds")
        GfxButton::setShowBounds(true);
    if (const auto i = Utility::retrieveNumber(parName, "@. Text bounds"); i != -1)
    {
        GfxButton::setShowBounds(true, i - 1);
    }
} 

void ParameterLine::deselect()
{
    if (mSelectedValue == nullptr && mSelectedLine == nullptr)
        return;

    mSelectedValue->mRect.setFillColor(GfxParameter::defaultRectColor);

    // Hide always just in case
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
    static auto text = sf::Text();
    static auto chSz = sf::Vector2f();
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
    const auto x = mSelectedValue->getPosition().x - mSelectedValue->mRect.getSize().x / 2 + 
        (mSelectedValue->mRect.getSize().x - mSelectedValue->mValText.getLocalBounds().width) / 2 +
        mSelectedValueIndex * (chSz.x - text.getLetterSpacing() * 2);
    const auto y = mCursor.getPosition().y;

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

    const auto values = linePtr->mParameterValues;
    sf::Uint8 rgba[4];
    auto i = 0ul;
    for (const auto &value : values)
    {
        const auto str = std::string(value->mValText.getString());
        rgba[i] = static_cast<sf::Uint8>(std::stoi(str));
        ++i;
    }

    return { rgba[0], rgba[1], rgba[2], rgba[3] };
}

bool ParameterLine::checkLimits(float check) const
{
    return check < mParameter->mLowLimits || check > mParameter->mHighLimits;
}

void ParameterLine::runThread(std::string &curVal, const std::string &prevVal)
{
    if (!mIsThRunning)
    {
        mWarningTh = std::thread(ParameterLine::warningVisualization, &mIsThRunning);
        mWarningTh.detach();
    }
    curVal = prevVal;
}

void ParameterLine::warningVisualization(bool *isRunning)
{
    *isRunning = true;
    
    auto mtx = std::mutex();
    auto gfxPar = mSelectedValue;
    const auto red = sf::Color(170,0,0);
    auto clock = sf::Clock();
    auto elapsedTime = sf::Time::Zero;
    auto totalTime = sf::Time::Zero;
    const auto timeToChange = sf::milliseconds(500);

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

bool ParameterLine::isHidden() const
{
    return getPosition() == sf::Vector2f(-1000.f, -1000.f);
}

bool ParameterLine::isEmpty(ParameterLine::ID id)
{
    return
        // id == ParameterLine::ID::StatTextMty ||
        // id == ParameterLine::ID::StatTextAdvMty ||
        id == ParameterLine::ID::StatTextAdvStrMty ||
        // id == ParameterLine::ID::BtnTextMty ||
        id == ParameterLine::ID::BtnTextAdvMty ||
        // id == ParameterLine::ID::BtnGfxMty ||
        id == ParameterLine::ID::BtnGfxAdvMty ||
        // id == ParameterLine::ID::AnimGfxLightMty ||
        id == ParameterLine::ID::AnimGfxPressMty ||
        id == ParameterLine::ID::MainWndwMty ||
        id == ParameterLine::ID::KPSWndwMty ||
        // id == ParameterLine::ID::KeyPressVisMty ||
        id == ParameterLine::ID::KeyPressVisAdvModeMty ||
        id == ParameterLine::ID::OtherMty ||
        // id == ParameterLine::ID::InfoMty ||
        id == ParameterLine::ID::LastLine;
}

bool ParameterLine::isToSkip(ParameterLine::ID id)
{
    auto difference = [] (ParameterLine::ID lhs, ParameterLine::ID rhs)
        {
            return static_cast<int>(lhs) - static_cast<int>(rhs);
        };
    auto isInBounds = [id] (ParameterLine::ID start, int parametersNumber)
        {
            const auto numUl = static_cast<size_t>(std::abs(parametersNumber));
            const auto endUl = static_cast<size_t>(start) + numUl * Settings::SupportedAdvancedKeysNumber - 1u;
            const auto startUl = static_cast<size_t>(start) + numUl;
            const auto idUl = static_cast<size_t>(id);

            return idUl >= startUl && idUl <= endUl;
        };

    return 
        (id >= ParameterLine::ID::SaveStatColl && id <= ParameterLine::ID::SaveStatMty) ||
        isInBounds(ParameterLine::ID::BtnTextAdvVisPosition1, difference(ParameterLine::ID::BtnTextAdvVisPosition1, ParameterLine::ID::BtnTextAdvVisPosition2)) ||
        isInBounds(ParameterLine::ID::BtnTextAdvClr1, difference(ParameterLine::ID::BtnTextAdvClr1, ParameterLine::ID::BtnTextAdvClr2)) ||
        isInBounds(ParameterLine::ID::BtnGfxBtnPos1, difference(ParameterLine::ID::BtnGfxBtnPos1, ParameterLine::ID::BtnGfxBtnPos2)) ||
        isInBounds(ParameterLine::ID::KeyPressVisAdvModeSpeed1, difference(ParameterLine::ID::KeyPressVisAdvModeSpeed1, ParameterLine::ID::KeyPressVisAdvModeSpeed2)) ||
        (id >= ParameterLine::ID::StatTextAdvPos2 && id <= ParameterLine::ID::StatTextAdvItal3);
}

void ParameterLine::deselectValue()
{
    if (mSelectedValue == nullptr && mSelectedLine == nullptr)
        return;

    mSelectedValue->mRect.setFillColor(GfxParameter::defaultRectColor);

    // Hide always just in case
    GfxButton::setShowBounds(false);
    mSelectedParameter = nullptr;
    mSelectedLine = nullptr;
    mSelectedValue = nullptr;
    mSelectedValueIndex = -1;
    mPalette.closeWindow();
}

bool ParameterLine::isValueSelected()
{
    return mSelectedValue.get();
}

bool ParameterLine::resetRefreshState()
{
    return mRefresh && !(mRefresh = false);
}

ParameterLine::ID ParameterLine::parIdToParLineId(LogicalParameter::ID id)
{
    switch(id)
    {
        case LogicalParameter::ID::StatTextDist: return ParameterLine::ID::StatTextDist;
        case LogicalParameter::ID::StatPos: return ParameterLine::ID::StatPos;
        case LogicalParameter::ID::StatValPos: return ParameterLine::ID::StatValPos;
        case LogicalParameter::ID::StatTextCenterOrigin: return ParameterLine::ID::StatTextCenterOrigin;
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

        case LogicalParameter::ID::StatTextAdvMode: return ParameterLine::ID::StatTextAdvMode;
        case LogicalParameter::ID::StatTextAdvPos1: return ParameterLine::ID::StatTextAdvPos1;
        case LogicalParameter::ID::StatTextAdvValPos1: return ParameterLine::ID::StatTextAdvValPos1;
        case LogicalParameter::ID::StatTextAdvCenterOrigin1: return ParameterLine::ID::StatTextAdvCenterOrigin1;
        case LogicalParameter::ID::StatTextAdvClr1: return ParameterLine::ID::StatTextAdvClr1;
        case LogicalParameter::ID::StatTextAdvChSz1: return ParameterLine::ID::StatTextAdvChSz1;
        case LogicalParameter::ID::StatTextAdvBold1: return ParameterLine::ID::StatTextAdvBold1;
        case LogicalParameter::ID::StatTextAdvItal1: return ParameterLine::ID::StatTextAdvItal1;
        case LogicalParameter::ID::StatTextAdvPos2: return ParameterLine::ID::StatTextAdvPos2;
        case LogicalParameter::ID::StatTextAdvCenterOrigin2: return ParameterLine::ID::StatTextAdvCenterOrigin2;
        case LogicalParameter::ID::StatTextAdvClr2: return ParameterLine::ID::StatTextAdvClr2;
        case LogicalParameter::ID::StatTextAdvValPos2: return ParameterLine::ID::StatTextAdvValPos2;
        case LogicalParameter::ID::StatTextAdvChSz2: return ParameterLine::ID::StatTextAdvChSz2;
        case LogicalParameter::ID::StatTextAdvBold2: return ParameterLine::ID::StatTextAdvBold2;
        case LogicalParameter::ID::StatTextAdvItal2: return ParameterLine::ID::StatTextAdvItal2;
        case LogicalParameter::ID::StatTextAdvPos3: return ParameterLine::ID::StatTextAdvPos3;
        case LogicalParameter::ID::StatTextAdvCenterOrigin3: return ParameterLine::ID::StatTextAdvCenterOrigin3;
        case LogicalParameter::ID::StatTextAdvValPos3: return ParameterLine::ID::StatTextAdvValPos3;
        case LogicalParameter::ID::StatTextAdvClr3: return ParameterLine::ID::StatTextAdvClr3;
        case LogicalParameter::ID::StatTextAdvChSz3: return ParameterLine::ID::StatTextAdvChSz3;
        case LogicalParameter::ID::StatTextAdvBold3: return ParameterLine::ID::StatTextAdvBold3;
        case LogicalParameter::ID::StatTextAdvItal3: return ParameterLine::ID::StatTextAdvItal3;
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
        case LogicalParameter::ID::BtnTextBoundsToggle: return ParameterLine::ID::BtnTextBoundsToggle;
        case LogicalParameter::ID::BtnTextBounds: return ParameterLine::ID::BtnTextBounds;
        case LogicalParameter::ID::BtnTextIgnoreBtnMovement: return ParameterLine::ID::BtnTextIgnoreBtnMovement;
        case LogicalParameter::ID::BtnTextBold: return ParameterLine::ID::BtnTextBold;
        case LogicalParameter::ID::BtnTextItal: return ParameterLine::ID::BtnTextItal;
        case LogicalParameter::ID::BtnTextShowVisKeys: return ParameterLine::ID::BtnTextShowVisKeys;
        case LogicalParameter::ID::BtnTextVisPosition: return ParameterLine::ID::BtnTextVisPosition;
        case LogicalParameter::ID::BtnTextShowTot: return ParameterLine::ID::BtnTextShowTot;
        case LogicalParameter::ID::BtnTextTotPosition: return ParameterLine::ID::BtnTextTotPosition;
        case LogicalParameter::ID::BtnTextShowKps: return ParameterLine::ID::BtnTextShowKps;
        case LogicalParameter::ID::BtnTextKPSPosition: return ParameterLine::ID::BtnTextKPSPosition;
        case LogicalParameter::ID::BtnTextShowBpm: return ParameterLine::ID::BtnTextShowBpm;
        case LogicalParameter::ID::BtnTextBPMPosition: return ParameterLine::ID::BtnTextBPMPosition;

        case LogicalParameter::ID::BtnTextSepPosAdvMode: return ParameterLine::ID::BtnTextSepPosAdvMode;
        case LogicalParameter::ID::BtnTextAdvVisPosition1: return ParameterLine::ID::BtnTextAdvVisPosition1;
        case LogicalParameter::ID::BtnTextAdvTotPosition1: return ParameterLine::ID::BtnTextAdvTotPosition1;
        case LogicalParameter::ID::BtnTextAdvKPSPosition1: return ParameterLine::ID::BtnTextAdvKPSPosition1;
        case LogicalParameter::ID::BtnTextAdvBPMPosition1: return ParameterLine::ID::BtnTextAdvBPMPosition1;
        case LogicalParameter::ID::BtnTextAdvVisPosition2: return ParameterLine::ID::BtnTextAdvVisPosition2;
        case LogicalParameter::ID::BtnTextAdvTotPosition2: return ParameterLine::ID::BtnTextAdvTotPosition2;
        case LogicalParameter::ID::BtnTextAdvKPSPosition2: return ParameterLine::ID::BtnTextAdvKPSPosition2;
        case LogicalParameter::ID::BtnTextAdvBPMPosition2: return ParameterLine::ID::BtnTextAdvBPMPosition2;
        case LogicalParameter::ID::BtnTextAdvVisPosition3: return ParameterLine::ID::BtnTextAdvVisPosition3;
        case LogicalParameter::ID::BtnTextAdvTotPosition3: return ParameterLine::ID::BtnTextAdvTotPosition3;
        case LogicalParameter::ID::BtnTextAdvKPSPosition3: return ParameterLine::ID::BtnTextAdvKPSPosition3;
        case LogicalParameter::ID::BtnTextAdvBPMPosition3: return ParameterLine::ID::BtnTextAdvBPMPosition3;
        case LogicalParameter::ID::BtnTextAdvVisPosition4: return ParameterLine::ID::BtnTextAdvVisPosition4;
        case LogicalParameter::ID::BtnTextAdvTotPosition4: return ParameterLine::ID::BtnTextAdvTotPosition4;
        case LogicalParameter::ID::BtnTextAdvKPSPosition4: return ParameterLine::ID::BtnTextAdvKPSPosition4;
        case LogicalParameter::ID::BtnTextAdvBPMPosition4: return ParameterLine::ID::BtnTextAdvBPMPosition4;
        case LogicalParameter::ID::BtnTextAdvVisPosition5: return ParameterLine::ID::BtnTextAdvVisPosition5;
        case LogicalParameter::ID::BtnTextAdvTotPosition5: return ParameterLine::ID::BtnTextAdvTotPosition5;
        case LogicalParameter::ID::BtnTextAdvKPSPosition5: return ParameterLine::ID::BtnTextAdvKPSPosition5;
        case LogicalParameter::ID::BtnTextAdvBPMPosition5: return ParameterLine::ID::BtnTextAdvBPMPosition5;
        case LogicalParameter::ID::BtnTextAdvVisPosition6: return ParameterLine::ID::BtnTextAdvVisPosition6;
        case LogicalParameter::ID::BtnTextAdvTotPosition6: return ParameterLine::ID::BtnTextAdvTotPosition6;
        case LogicalParameter::ID::BtnTextAdvKPSPosition6: return ParameterLine::ID::BtnTextAdvKPSPosition6;
        case LogicalParameter::ID::BtnTextAdvBPMPosition6: return ParameterLine::ID::BtnTextAdvBPMPosition6;
        case LogicalParameter::ID::BtnTextAdvVisPosition7: return ParameterLine::ID::BtnTextAdvVisPosition7;
        case LogicalParameter::ID::BtnTextAdvTotPosition7: return ParameterLine::ID::BtnTextAdvTotPosition7;
        case LogicalParameter::ID::BtnTextAdvKPSPosition7: return ParameterLine::ID::BtnTextAdvKPSPosition7;
        case LogicalParameter::ID::BtnTextAdvBPMPosition7: return ParameterLine::ID::BtnTextAdvBPMPosition7;
        case LogicalParameter::ID::BtnTextAdvVisPosition8: return ParameterLine::ID::BtnTextAdvVisPosition8;
        case LogicalParameter::ID::BtnTextAdvTotPosition8: return ParameterLine::ID::BtnTextAdvTotPosition8;
        case LogicalParameter::ID::BtnTextAdvKPSPosition8: return ParameterLine::ID::BtnTextAdvKPSPosition8;
        case LogicalParameter::ID::BtnTextAdvBPMPosition8: return ParameterLine::ID::BtnTextAdvBPMPosition8;
        case LogicalParameter::ID::BtnTextAdvVisPosition9: return ParameterLine::ID::BtnTextAdvVisPosition9;
        case LogicalParameter::ID::BtnTextAdvTotPosition9: return ParameterLine::ID::BtnTextAdvTotPosition9;
        case LogicalParameter::ID::BtnTextAdvKPSPosition9: return ParameterLine::ID::BtnTextAdvKPSPosition9;
        case LogicalParameter::ID::BtnTextAdvBPMPosition9: return ParameterLine::ID::BtnTextAdvBPMPosition9;
        case LogicalParameter::ID::BtnTextAdvVisPosition10: return ParameterLine::ID::BtnTextAdvVisPosition10;
        case LogicalParameter::ID::BtnTextAdvTotPosition10: return ParameterLine::ID::BtnTextAdvTotPosition10;
        case LogicalParameter::ID::BtnTextAdvKPSPosition10: return ParameterLine::ID::BtnTextAdvKPSPosition10;
        case LogicalParameter::ID::BtnTextAdvBPMPosition10: return ParameterLine::ID::BtnTextAdvBPMPosition10;
        case LogicalParameter::ID::BtnTextAdvVisPosition11: return ParameterLine::ID::BtnTextAdvVisPosition11;
        case LogicalParameter::ID::BtnTextAdvTotPosition11: return ParameterLine::ID::BtnTextAdvTotPosition11;
        case LogicalParameter::ID::BtnTextAdvKPSPosition11: return ParameterLine::ID::BtnTextAdvKPSPosition11;
        case LogicalParameter::ID::BtnTextAdvBPMPosition11: return ParameterLine::ID::BtnTextAdvBPMPosition11;
        case LogicalParameter::ID::BtnTextAdvVisPosition12: return ParameterLine::ID::BtnTextAdvVisPosition12;
        case LogicalParameter::ID::BtnTextAdvTotPosition12: return ParameterLine::ID::BtnTextAdvTotPosition12;
        case LogicalParameter::ID::BtnTextAdvKPSPosition12: return ParameterLine::ID::BtnTextAdvKPSPosition12;
        case LogicalParameter::ID::BtnTextAdvBPMPosition12: return ParameterLine::ID::BtnTextAdvBPMPosition12;
        case LogicalParameter::ID::BtnTextAdvVisPosition13: return ParameterLine::ID::BtnTextAdvVisPosition13;
        case LogicalParameter::ID::BtnTextAdvTotPosition13: return ParameterLine::ID::BtnTextAdvTotPosition13;
        case LogicalParameter::ID::BtnTextAdvKPSPosition13: return ParameterLine::ID::BtnTextAdvKPSPosition13;
        case LogicalParameter::ID::BtnTextAdvBPMPosition13: return ParameterLine::ID::BtnTextAdvBPMPosition13;
        case LogicalParameter::ID::BtnTextAdvVisPosition14: return ParameterLine::ID::BtnTextAdvVisPosition14;
        case LogicalParameter::ID::BtnTextAdvTotPosition14: return ParameterLine::ID::BtnTextAdvTotPosition14;
        case LogicalParameter::ID::BtnTextAdvKPSPosition14: return ParameterLine::ID::BtnTextAdvKPSPosition14;
        case LogicalParameter::ID::BtnTextAdvBPMPosition14: return ParameterLine::ID::BtnTextAdvBPMPosition14;
        case LogicalParameter::ID::BtnTextAdvVisPosition15: return ParameterLine::ID::BtnTextAdvVisPosition15;
        case LogicalParameter::ID::BtnTextAdvTotPosition15: return ParameterLine::ID::BtnTextAdvTotPosition15;
        case LogicalParameter::ID::BtnTextAdvKPSPosition15: return ParameterLine::ID::BtnTextAdvKPSPosition15;
        case LogicalParameter::ID::BtnTextAdvBPMPosition15: return ParameterLine::ID::BtnTextAdvBPMPosition15;
        case LogicalParameter::ID::BtnTextAdvVisPosition16: return ParameterLine::ID::BtnTextAdvVisPosition16;
        case LogicalParameter::ID::BtnTextAdvTotPosition16: return ParameterLine::ID::BtnTextAdvTotPosition16;
        case LogicalParameter::ID::BtnTextAdvKPSPosition16: return ParameterLine::ID::BtnTextAdvKPSPosition16;
        case LogicalParameter::ID::BtnTextAdvBPMPosition16: return ParameterLine::ID::BtnTextAdvBPMPosition16;
        case LogicalParameter::ID::BtnTextAdvVisPosition17: return ParameterLine::ID::BtnTextAdvVisPosition17;
        case LogicalParameter::ID::BtnTextAdvTotPosition17: return ParameterLine::ID::BtnTextAdvTotPosition17;
        case LogicalParameter::ID::BtnTextAdvKPSPosition17: return ParameterLine::ID::BtnTextAdvKPSPosition17;
        case LogicalParameter::ID::BtnTextAdvBPMPosition17: return ParameterLine::ID::BtnTextAdvBPMPosition17;
        case LogicalParameter::ID::BtnTextAdvVisPosition18: return ParameterLine::ID::BtnTextAdvVisPosition18;
        case LogicalParameter::ID::BtnTextAdvTotPosition18: return ParameterLine::ID::BtnTextAdvTotPosition18;
        case LogicalParameter::ID::BtnTextAdvKPSPosition18: return ParameterLine::ID::BtnTextAdvKPSPosition18;
        case LogicalParameter::ID::BtnTextAdvBPMPosition18: return ParameterLine::ID::BtnTextAdvBPMPosition18;        
        case LogicalParameter::ID::BtnTextAdvVisPosition19: return ParameterLine::ID::BtnTextAdvVisPosition19;
        case LogicalParameter::ID::BtnTextAdvTotPosition19: return ParameterLine::ID::BtnTextAdvTotPosition19;
        case LogicalParameter::ID::BtnTextAdvKPSPosition19: return ParameterLine::ID::BtnTextAdvKPSPosition19;
        case LogicalParameter::ID::BtnTextAdvBPMPosition19: return ParameterLine::ID::BtnTextAdvBPMPosition19;
        case LogicalParameter::ID::BtnTextAdvVisPosition20: return ParameterLine::ID::BtnTextAdvVisPosition20;
        case LogicalParameter::ID::BtnTextAdvTotPosition20: return ParameterLine::ID::BtnTextAdvTotPosition20;
        case LogicalParameter::ID::BtnTextAdvKPSPosition20: return ParameterLine::ID::BtnTextAdvKPSPosition20;
        case LogicalParameter::ID::BtnTextAdvBPMPosition20: return ParameterLine::ID::BtnTextAdvBPMPosition20;


        case LogicalParameter::ID::BtnTextPosAdvMode: return ParameterLine::ID::BtnTextPosAdvMode;
        case LogicalParameter::ID::BtnTextAdvClr1: return ParameterLine::ID::BtnTextAdvClr1;
        case LogicalParameter::ID::BtnTextAdvChSz1: return ParameterLine::ID::BtnTextAdvChSz1;
        case LogicalParameter::ID::BtnTextAdvOutThck1: return ParameterLine::ID::BtnTextAdvOutThck1;
        case LogicalParameter::ID::BtnTextAdvOutClr1: return ParameterLine::ID::BtnTextAdvOutClr1;
        case LogicalParameter::ID::BtnTextAdvPosition1: return ParameterLine::ID::BtnTextAdvPosition1;
        case LogicalParameter::ID::BtnTextAdvBounds1: return ParameterLine::ID::BtnTextAdvBounds1;
        case LogicalParameter::ID::BtnTextAdvBold1: return ParameterLine::ID::BtnTextAdvBold1;
        case LogicalParameter::ID::BtnTextAdvItal1: return ParameterLine::ID::BtnTextAdvItal1;
        case LogicalParameter::ID::BtnTextAdvClr2: return ParameterLine::ID::BtnTextAdvClr2;
        case LogicalParameter::ID::BtnTextAdvChSz2: return ParameterLine::ID::BtnTextAdvChSz2;
        case LogicalParameter::ID::BtnTextAdvOutThck2: return ParameterLine::ID::BtnTextAdvOutThck2;
        case LogicalParameter::ID::BtnTextAdvOutClr2: return ParameterLine::ID::BtnTextAdvOutClr2;
        case LogicalParameter::ID::BtnTextAdvPosition2: return ParameterLine::ID::BtnTextAdvPosition2;
        case LogicalParameter::ID::BtnTextAdvBounds2: return ParameterLine::ID::BtnTextAdvBounds2;
        case LogicalParameter::ID::BtnTextAdvBold2: return ParameterLine::ID::BtnTextAdvBold2;
        case LogicalParameter::ID::BtnTextAdvItal2: return ParameterLine::ID::BtnTextAdvItal2;
        case LogicalParameter::ID::BtnTextAdvClr3: return ParameterLine::ID::BtnTextAdvClr3;
        case LogicalParameter::ID::BtnTextAdvChSz3: return ParameterLine::ID::BtnTextAdvChSz3;
        case LogicalParameter::ID::BtnTextAdvOutThck3: return ParameterLine::ID::BtnTextAdvOutThck3;
        case LogicalParameter::ID::BtnTextAdvOutClr3: return ParameterLine::ID::BtnTextAdvOutClr3;
        case LogicalParameter::ID::BtnTextAdvPosition3: return ParameterLine::ID::BtnTextAdvPosition3;
        case LogicalParameter::ID::BtnTextAdvBounds3: return ParameterLine::ID::BtnTextAdvBounds3;
        case LogicalParameter::ID::BtnTextAdvBold3: return ParameterLine::ID::BtnTextAdvBold3;
        case LogicalParameter::ID::BtnTextAdvItal3: return ParameterLine::ID::BtnTextAdvItal3;
        case LogicalParameter::ID::BtnTextAdvClr4: return ParameterLine::ID::BtnTextAdvClr4;
        case LogicalParameter::ID::BtnTextAdvChSz4: return ParameterLine::ID::BtnTextAdvChSz4;
        case LogicalParameter::ID::BtnTextAdvOutThck4: return ParameterLine::ID::BtnTextAdvOutThck4;
        case LogicalParameter::ID::BtnTextAdvOutClr4: return ParameterLine::ID::BtnTextAdvOutClr4;
        case LogicalParameter::ID::BtnTextAdvPosition4: return ParameterLine::ID::BtnTextAdvPosition4;
        case LogicalParameter::ID::BtnTextAdvBounds4: return ParameterLine::ID::BtnTextAdvBounds4;
        case LogicalParameter::ID::BtnTextAdvBold4: return ParameterLine::ID::BtnTextAdvBold4;
        case LogicalParameter::ID::BtnTextAdvItal4: return ParameterLine::ID::BtnTextAdvItal4;
        case LogicalParameter::ID::BtnTextAdvClr5: return ParameterLine::ID::BtnTextAdvClr5;
        case LogicalParameter::ID::BtnTextAdvChSz5: return ParameterLine::ID::BtnTextAdvChSz5;
        case LogicalParameter::ID::BtnTextAdvOutThck5: return ParameterLine::ID::BtnTextAdvOutThck5;
        case LogicalParameter::ID::BtnTextAdvOutClr5: return ParameterLine::ID::BtnTextAdvOutClr5;
        case LogicalParameter::ID::BtnTextAdvPosition5: return ParameterLine::ID::BtnTextAdvPosition5;
        case LogicalParameter::ID::BtnTextAdvBounds5: return ParameterLine::ID::BtnTextAdvBounds5;
        case LogicalParameter::ID::BtnTextAdvBold5: return ParameterLine::ID::BtnTextAdvBold5;
        case LogicalParameter::ID::BtnTextAdvItal5: return ParameterLine::ID::BtnTextAdvItal5;
        case LogicalParameter::ID::BtnTextAdvClr6: return ParameterLine::ID::BtnTextAdvClr6;
        case LogicalParameter::ID::BtnTextAdvChSz6: return ParameterLine::ID::BtnTextAdvChSz6;
        case LogicalParameter::ID::BtnTextAdvOutThck6: return ParameterLine::ID::BtnTextAdvOutThck6;
        case LogicalParameter::ID::BtnTextAdvOutClr6: return ParameterLine::ID::BtnTextAdvOutClr6;
        case LogicalParameter::ID::BtnTextAdvPosition6: return ParameterLine::ID::BtnTextAdvPosition6;
        case LogicalParameter::ID::BtnTextAdvBounds6: return ParameterLine::ID::BtnTextAdvBounds6;
        case LogicalParameter::ID::BtnTextAdvBold6: return ParameterLine::ID::BtnTextAdvBold6;
        case LogicalParameter::ID::BtnTextAdvItal6: return ParameterLine::ID::BtnTextAdvItal6;
        case LogicalParameter::ID::BtnTextAdvClr7: return ParameterLine::ID::BtnTextAdvClr7;
        case LogicalParameter::ID::BtnTextAdvChSz7: return ParameterLine::ID::BtnTextAdvChSz7;
        case LogicalParameter::ID::BtnTextAdvOutThck7: return ParameterLine::ID::BtnTextAdvOutThck7;
        case LogicalParameter::ID::BtnTextAdvOutClr7: return ParameterLine::ID::BtnTextAdvOutClr7;
        case LogicalParameter::ID::BtnTextAdvPosition7: return ParameterLine::ID::BtnTextAdvPosition7;
        case LogicalParameter::ID::BtnTextAdvBounds7: return ParameterLine::ID::BtnTextAdvBounds7;
        case LogicalParameter::ID::BtnTextAdvBold7: return ParameterLine::ID::BtnTextAdvBold7;
        case LogicalParameter::ID::BtnTextAdvItal7: return ParameterLine::ID::BtnTextAdvItal7;
        case LogicalParameter::ID::BtnTextAdvClr8: return ParameterLine::ID::BtnTextAdvClr8;
        case LogicalParameter::ID::BtnTextAdvChSz8: return ParameterLine::ID::BtnTextAdvChSz8;
        case LogicalParameter::ID::BtnTextAdvOutThck8: return ParameterLine::ID::BtnTextAdvOutThck8;
        case LogicalParameter::ID::BtnTextAdvOutClr8: return ParameterLine::ID::BtnTextAdvOutClr8;
        case LogicalParameter::ID::BtnTextAdvPosition8: return ParameterLine::ID::BtnTextAdvPosition8;
        case LogicalParameter::ID::BtnTextAdvBounds8: return ParameterLine::ID::BtnTextAdvBounds8;
        case LogicalParameter::ID::BtnTextAdvBold8: return ParameterLine::ID::BtnTextAdvBold8;
        case LogicalParameter::ID::BtnTextAdvItal8: return ParameterLine::ID::BtnTextAdvItal8;
        case LogicalParameter::ID::BtnTextAdvClr9: return ParameterLine::ID::BtnTextAdvClr9;
        case LogicalParameter::ID::BtnTextAdvChSz9: return ParameterLine::ID::BtnTextAdvChSz9;
        case LogicalParameter::ID::BtnTextAdvOutThck9: return ParameterLine::ID::BtnTextAdvOutThck9;
        case LogicalParameter::ID::BtnTextAdvOutClr9: return ParameterLine::ID::BtnTextAdvOutClr9;
        case LogicalParameter::ID::BtnTextAdvPosition9: return ParameterLine::ID::BtnTextAdvPosition9;
        case LogicalParameter::ID::BtnTextAdvBounds9: return ParameterLine::ID::BtnTextAdvBounds9;
        case LogicalParameter::ID::BtnTextAdvBold9: return ParameterLine::ID::BtnTextAdvBold9;
        case LogicalParameter::ID::BtnTextAdvItal9: return ParameterLine::ID::BtnTextAdvItal9;
        case LogicalParameter::ID::BtnTextAdvClr10: return ParameterLine::ID::BtnTextAdvClr10;
        case LogicalParameter::ID::BtnTextAdvChSz10: return ParameterLine::ID::BtnTextAdvChSz10;
        case LogicalParameter::ID::BtnTextAdvOutThck10: return ParameterLine::ID::BtnTextAdvOutThck10;
        case LogicalParameter::ID::BtnTextAdvOutClr10: return ParameterLine::ID::BtnTextAdvOutClr10;
        case LogicalParameter::ID::BtnTextAdvPosition10: return ParameterLine::ID::BtnTextAdvPosition10;
        case LogicalParameter::ID::BtnTextAdvBounds10: return ParameterLine::ID::BtnTextAdvBounds10;
        case LogicalParameter::ID::BtnTextAdvBold10: return ParameterLine::ID::BtnTextAdvBold10;
        case LogicalParameter::ID::BtnTextAdvItal10: return ParameterLine::ID::BtnTextAdvItal10;
        case LogicalParameter::ID::BtnTextAdvClr11: return ParameterLine::ID::BtnTextAdvClr11;
        case LogicalParameter::ID::BtnTextAdvChSz11: return ParameterLine::ID::BtnTextAdvChSz11;
        case LogicalParameter::ID::BtnTextAdvOutThck11: return ParameterLine::ID::BtnTextAdvOutThck11;
        case LogicalParameter::ID::BtnTextAdvOutClr11: return ParameterLine::ID::BtnTextAdvOutClr11;
        case LogicalParameter::ID::BtnTextAdvPosition11: return ParameterLine::ID::BtnTextAdvPosition11;
        case LogicalParameter::ID::BtnTextAdvBounds11: return ParameterLine::ID::BtnTextAdvBounds11;
        case LogicalParameter::ID::BtnTextAdvBold11: return ParameterLine::ID::BtnTextAdvBold11;
        case LogicalParameter::ID::BtnTextAdvItal11: return ParameterLine::ID::BtnTextAdvItal11;
        case LogicalParameter::ID::BtnTextAdvClr12: return ParameterLine::ID::BtnTextAdvClr12;
        case LogicalParameter::ID::BtnTextAdvChSz12: return ParameterLine::ID::BtnTextAdvChSz12;
        case LogicalParameter::ID::BtnTextAdvOutThck12: return ParameterLine::ID::BtnTextAdvOutThck12;
        case LogicalParameter::ID::BtnTextAdvOutClr12: return ParameterLine::ID::BtnTextAdvOutClr12;
        case LogicalParameter::ID::BtnTextAdvPosition12: return ParameterLine::ID::BtnTextAdvPosition12;
        case LogicalParameter::ID::BtnTextAdvBounds12: return ParameterLine::ID::BtnTextAdvBounds12;
        case LogicalParameter::ID::BtnTextAdvBold12: return ParameterLine::ID::BtnTextAdvBold12;
        case LogicalParameter::ID::BtnTextAdvItal12: return ParameterLine::ID::BtnTextAdvItal12;
        case LogicalParameter::ID::BtnTextAdvClr13: return ParameterLine::ID::BtnTextAdvClr13;
        case LogicalParameter::ID::BtnTextAdvChSz13: return ParameterLine::ID::BtnTextAdvChSz13;
        case LogicalParameter::ID::BtnTextAdvOutThck13: return ParameterLine::ID::BtnTextAdvOutThck13;
        case LogicalParameter::ID::BtnTextAdvOutClr13: return ParameterLine::ID::BtnTextAdvOutClr13;
        case LogicalParameter::ID::BtnTextAdvPosition13: return ParameterLine::ID::BtnTextAdvPosition13;
        case LogicalParameter::ID::BtnTextAdvBounds13: return ParameterLine::ID::BtnTextAdvBounds13;
        case LogicalParameter::ID::BtnTextAdvBold13: return ParameterLine::ID::BtnTextAdvBold13;
        case LogicalParameter::ID::BtnTextAdvItal13: return ParameterLine::ID::BtnTextAdvItal13;
        case LogicalParameter::ID::BtnTextAdvClr14: return ParameterLine::ID::BtnTextAdvClr14;
        case LogicalParameter::ID::BtnTextAdvChSz14: return ParameterLine::ID::BtnTextAdvChSz14;
        case LogicalParameter::ID::BtnTextAdvOutThck14: return ParameterLine::ID::BtnTextAdvOutThck14;
        case LogicalParameter::ID::BtnTextAdvOutClr14: return ParameterLine::ID::BtnTextAdvOutClr14;
        case LogicalParameter::ID::BtnTextAdvPosition14: return ParameterLine::ID::BtnTextAdvPosition14;
        case LogicalParameter::ID::BtnTextAdvBounds14: return ParameterLine::ID::BtnTextAdvBounds14;
        case LogicalParameter::ID::BtnTextAdvBold14: return ParameterLine::ID::BtnTextAdvBold14;
        case LogicalParameter::ID::BtnTextAdvItal14: return ParameterLine::ID::BtnTextAdvItal14;
        case LogicalParameter::ID::BtnTextAdvClr15: return ParameterLine::ID::BtnTextAdvClr15;
        case LogicalParameter::ID::BtnTextAdvChSz15: return ParameterLine::ID::BtnTextAdvChSz15;
        case LogicalParameter::ID::BtnTextAdvOutThck15: return ParameterLine::ID::BtnTextAdvOutThck15;
        case LogicalParameter::ID::BtnTextAdvOutClr15: return ParameterLine::ID::BtnTextAdvOutClr15;
        case LogicalParameter::ID::BtnTextAdvPosition15: return ParameterLine::ID::BtnTextAdvPosition15;
        case LogicalParameter::ID::BtnTextAdvBounds15: return ParameterLine::ID::BtnTextAdvBounds15;
        case LogicalParameter::ID::BtnTextAdvBold15: return ParameterLine::ID::BtnTextAdvBold15;
        case LogicalParameter::ID::BtnTextAdvItal15: return ParameterLine::ID::BtnTextAdvItal15;
        case LogicalParameter::ID::BtnTextAdvClr16: return ParameterLine::ID::BtnTextAdvClr16;
        case LogicalParameter::ID::BtnTextAdvChSz16: return ParameterLine::ID::BtnTextAdvChSz16;
        case LogicalParameter::ID::BtnTextAdvOutThck16: return ParameterLine::ID::BtnTextAdvOutThck16;
        case LogicalParameter::ID::BtnTextAdvOutClr16: return ParameterLine::ID::BtnTextAdvOutClr16;
        case LogicalParameter::ID::BtnTextAdvPosition16: return ParameterLine::ID::BtnTextAdvPosition16;
        case LogicalParameter::ID::BtnTextAdvBounds16: return ParameterLine::ID::BtnTextAdvBounds16;
        case LogicalParameter::ID::BtnTextAdvBold16: return ParameterLine::ID::BtnTextAdvBold16;
        case LogicalParameter::ID::BtnTextAdvItal16: return ParameterLine::ID::BtnTextAdvItal16;
        case LogicalParameter::ID::BtnTextAdvClr17: return ParameterLine::ID::BtnTextAdvClr17;
        case LogicalParameter::ID::BtnTextAdvChSz17: return ParameterLine::ID::BtnTextAdvChSz17;
        case LogicalParameter::ID::BtnTextAdvOutThck17: return ParameterLine::ID::BtnTextAdvOutThck17;
        case LogicalParameter::ID::BtnTextAdvOutClr17: return ParameterLine::ID::BtnTextAdvOutClr17;
        case LogicalParameter::ID::BtnTextAdvPosition17: return ParameterLine::ID::BtnTextAdvPosition17;
        case LogicalParameter::ID::BtnTextAdvBounds17: return ParameterLine::ID::BtnTextAdvBounds17;
        case LogicalParameter::ID::BtnTextAdvBold17: return ParameterLine::ID::BtnTextAdvBold17;
        case LogicalParameter::ID::BtnTextAdvItal17: return ParameterLine::ID::BtnTextAdvItal17;
        case LogicalParameter::ID::BtnTextAdvClr18: return ParameterLine::ID::BtnTextAdvClr18;
        case LogicalParameter::ID::BtnTextAdvChSz18: return ParameterLine::ID::BtnTextAdvChSz18;
        case LogicalParameter::ID::BtnTextAdvOutThck18: return ParameterLine::ID::BtnTextAdvOutThck18;
        case LogicalParameter::ID::BtnTextAdvOutClr18: return ParameterLine::ID::BtnTextAdvOutClr18;
        case LogicalParameter::ID::BtnTextAdvPosition18: return ParameterLine::ID::BtnTextAdvPosition18;
        case LogicalParameter::ID::BtnTextAdvBounds18: return ParameterLine::ID::BtnTextAdvBounds18;
        case LogicalParameter::ID::BtnTextAdvBold18: return ParameterLine::ID::BtnTextAdvBold18;
        case LogicalParameter::ID::BtnTextAdvItal18: return ParameterLine::ID::BtnTextAdvItal18;
        case LogicalParameter::ID::BtnTextAdvClr19: return ParameterLine::ID::BtnTextAdvClr19;
        case LogicalParameter::ID::BtnTextAdvChSz19: return ParameterLine::ID::BtnTextAdvChSz19;
        case LogicalParameter::ID::BtnTextAdvOutThck19: return ParameterLine::ID::BtnTextAdvOutThck19;
        case LogicalParameter::ID::BtnTextAdvOutClr19: return ParameterLine::ID::BtnTextAdvOutClr19;
        case LogicalParameter::ID::BtnTextAdvPosition19: return ParameterLine::ID::BtnTextAdvPosition19;
        case LogicalParameter::ID::BtnTextAdvBounds19: return ParameterLine::ID::BtnTextAdvBounds19;
        case LogicalParameter::ID::BtnTextAdvBold19: return ParameterLine::ID::BtnTextAdvBold19;
        case LogicalParameter::ID::BtnTextAdvItal19: return ParameterLine::ID::BtnTextAdvItal19;
        case LogicalParameter::ID::BtnTextAdvClr20: return ParameterLine::ID::BtnTextAdvClr20;
        case LogicalParameter::ID::BtnTextAdvChSz20: return ParameterLine::ID::BtnTextAdvChSz20;
        case LogicalParameter::ID::BtnTextAdvOutThck20: return ParameterLine::ID::BtnTextAdvOutThck20;
        case LogicalParameter::ID::BtnTextAdvOutClr20: return ParameterLine::ID::BtnTextAdvOutClr20;
        case LogicalParameter::ID::BtnTextAdvPosition20: return ParameterLine::ID::BtnTextAdvPosition20;
        case LogicalParameter::ID::BtnTextAdvBounds20: return ParameterLine::ID::BtnTextAdvBounds20;
        case LogicalParameter::ID::BtnTextAdvBold20: return ParameterLine::ID::BtnTextAdvBold20;
        case LogicalParameter::ID::BtnTextAdvItal20: return ParameterLine::ID::BtnTextAdvItal20;

        case LogicalParameter::ID::BtnGfxDist: return ParameterLine::ID::BtnGfxDist;
        case LogicalParameter::ID::BtnGfxTxtr: return ParameterLine::ID::BtnGfxTxtr;
        case LogicalParameter::ID::BtnGfxTxtrSz: return ParameterLine::ID::BtnGfxTxtrSz;
        case LogicalParameter::ID::BtnGfxTxtrClr: return ParameterLine::ID::BtnGfxTxtrClr;
        
        case LogicalParameter::ID::BtnGfxAdvMode: return ParameterLine::ID::BtnGfxAdvMode;
        case LogicalParameter::ID::BtnGfxBtnPos1: return ParameterLine::ID::BtnGfxBtnPos1;
        case LogicalParameter::ID::BtnGfxSz1: return ParameterLine::ID::BtnGfxSz1;
        case LogicalParameter::ID::BtnGfxClr1: return ParameterLine::ID::BtnGfxClr1;
        case LogicalParameter::ID::BtnGfxBtnPos2: return ParameterLine::ID::BtnGfxBtnPos2;
        case LogicalParameter::ID::BtnGfxSz2: return ParameterLine::ID::BtnGfxSz2;
        case LogicalParameter::ID::BtnGfxClr2: return ParameterLine::ID::BtnGfxClr2;
        case LogicalParameter::ID::BtnGfxBtnPos3: return ParameterLine::ID::BtnGfxBtnPos3;
        case LogicalParameter::ID::BtnGfxSz3: return ParameterLine::ID::BtnGfxSz3;
        case LogicalParameter::ID::BtnGfxClr3: return ParameterLine::ID::BtnGfxClr3;
        case LogicalParameter::ID::BtnGfxBtnPos4: return ParameterLine::ID::BtnGfxBtnPos4;
        case LogicalParameter::ID::BtnGfxSz4: return ParameterLine::ID::BtnGfxSz4;
        case LogicalParameter::ID::BtnGfxClr4: return ParameterLine::ID::BtnGfxClr4;
        case LogicalParameter::ID::BtnGfxBtnPos5: return ParameterLine::ID::BtnGfxBtnPos5;
        case LogicalParameter::ID::BtnGfxSz5: return ParameterLine::ID::BtnGfxSz5;
        case LogicalParameter::ID::BtnGfxClr5: return ParameterLine::ID::BtnGfxClr5;
        case LogicalParameter::ID::BtnGfxBtnPos6: return ParameterLine::ID::BtnGfxBtnPos6;
        case LogicalParameter::ID::BtnGfxSz6: return ParameterLine::ID::BtnGfxSz6;
        case LogicalParameter::ID::BtnGfxClr6: return ParameterLine::ID::BtnGfxClr6;
        case LogicalParameter::ID::BtnGfxBtnPos7: return ParameterLine::ID::BtnGfxBtnPos7;
        case LogicalParameter::ID::BtnGfxSz7: return ParameterLine::ID::BtnGfxSz7;
        case LogicalParameter::ID::BtnGfxClr7: return ParameterLine::ID::BtnGfxClr7;
        case LogicalParameter::ID::BtnGfxBtnPos8: return ParameterLine::ID::BtnGfxBtnPos8;
        case LogicalParameter::ID::BtnGfxSz8: return ParameterLine::ID::BtnGfxSz8;
        case LogicalParameter::ID::BtnGfxClr8: return ParameterLine::ID::BtnGfxClr8;
        case LogicalParameter::ID::BtnGfxBtnPos9: return ParameterLine::ID::BtnGfxBtnPos9;
        case LogicalParameter::ID::BtnGfxSz9: return ParameterLine::ID::BtnGfxSz9;
        case LogicalParameter::ID::BtnGfxClr9: return ParameterLine::ID::BtnGfxClr9;
        case LogicalParameter::ID::BtnGfxBtnPos10: return ParameterLine::ID::BtnGfxBtnPos10;
        case LogicalParameter::ID::BtnGfxSz10: return ParameterLine::ID::BtnGfxSz10;
        case LogicalParameter::ID::BtnGfxClr10: return ParameterLine::ID::BtnGfxClr10;
        case LogicalParameter::ID::BtnGfxBtnPos11: return ParameterLine::ID::BtnGfxBtnPos11;
        case LogicalParameter::ID::BtnGfxSz11: return ParameterLine::ID::BtnGfxSz11;
        case LogicalParameter::ID::BtnGfxClr11: return ParameterLine::ID::BtnGfxClr11;
        case LogicalParameter::ID::BtnGfxBtnPos12: return ParameterLine::ID::BtnGfxBtnPos12;
        case LogicalParameter::ID::BtnGfxSz12: return ParameterLine::ID::BtnGfxSz12;
        case LogicalParameter::ID::BtnGfxClr12: return ParameterLine::ID::BtnGfxClr12;
        case LogicalParameter::ID::BtnGfxBtnPos13: return ParameterLine::ID::BtnGfxBtnPos13;
        case LogicalParameter::ID::BtnGfxSz13: return ParameterLine::ID::BtnGfxSz13;
        case LogicalParameter::ID::BtnGfxClr13: return ParameterLine::ID::BtnGfxClr13;
        case LogicalParameter::ID::BtnGfxBtnPos14: return ParameterLine::ID::BtnGfxBtnPos14;
        case LogicalParameter::ID::BtnGfxSz14: return ParameterLine::ID::BtnGfxSz14;
        case LogicalParameter::ID::BtnGfxClr14: return ParameterLine::ID::BtnGfxClr14;
        case LogicalParameter::ID::BtnGfxBtnPos15: return ParameterLine::ID::BtnGfxBtnPos15;
        case LogicalParameter::ID::BtnGfxSz15: return ParameterLine::ID::BtnGfxSz15;
        case LogicalParameter::ID::BtnGfxClr15: return ParameterLine::ID::BtnGfxClr15;
        case LogicalParameter::ID::BtnGfxBtnPos16: return ParameterLine::ID::BtnGfxBtnPos16;
        case LogicalParameter::ID::BtnGfxSz16: return ParameterLine::ID::BtnGfxSz16;
        case LogicalParameter::ID::BtnGfxClr16: return ParameterLine::ID::BtnGfxClr16;
        case LogicalParameter::ID::BtnGfxBtnPos17: return ParameterLine::ID::BtnGfxBtnPos17;
        case LogicalParameter::ID::BtnGfxSz17: return ParameterLine::ID::BtnGfxSz17;
        case LogicalParameter::ID::BtnGfxClr17: return ParameterLine::ID::BtnGfxClr17;
        case LogicalParameter::ID::BtnGfxBtnPos18: return ParameterLine::ID::BtnGfxBtnPos18;
        case LogicalParameter::ID::BtnGfxSz18: return ParameterLine::ID::BtnGfxSz18;
        case LogicalParameter::ID::BtnGfxClr18: return ParameterLine::ID::BtnGfxClr18;
        case LogicalParameter::ID::BtnGfxBtnPos19: return ParameterLine::ID::BtnGfxBtnPos19;
        case LogicalParameter::ID::BtnGfxSz19: return ParameterLine::ID::BtnGfxSz19;
        case LogicalParameter::ID::BtnGfxClr19: return ParameterLine::ID::BtnGfxClr19;
        case LogicalParameter::ID::BtnGfxBtnPos20: return ParameterLine::ID::BtnGfxBtnPos20;
        case LogicalParameter::ID::BtnGfxSz20: return ParameterLine::ID::BtnGfxSz20;
        case LogicalParameter::ID::BtnGfxClr20: return ParameterLine::ID::BtnGfxClr20;
        
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
        
        case LogicalParameter::ID::KeyPressVisAdvMode: return ParameterLine::ID::KeyPressVisAdvMode;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed1: return ParameterLine::ID::KeyPressVisAdvModeSpeed1;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation1: return ParameterLine::ID::KeyPressVisAdvModeRotation1;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen1: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen1;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig1: return ParameterLine::ID::KeyPressVisAdvModeOrig1;
        case LogicalParameter::ID::KeyPressVisAdvModeColor1: return ParameterLine::ID::KeyPressVisAdvModeColor1;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed2: return ParameterLine::ID::KeyPressVisAdvModeSpeed2;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation2: return ParameterLine::ID::KeyPressVisAdvModeRotation2;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen2: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen2;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig2: return ParameterLine::ID::KeyPressVisAdvModeOrig2;
        case LogicalParameter::ID::KeyPressVisAdvModeColor2: return ParameterLine::ID::KeyPressVisAdvModeColor2;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed3: return ParameterLine::ID::KeyPressVisAdvModeSpeed3;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation3: return ParameterLine::ID::KeyPressVisAdvModeRotation3;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen3: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen3;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig3: return ParameterLine::ID::KeyPressVisAdvModeOrig3;
        case LogicalParameter::ID::KeyPressVisAdvModeColor3: return ParameterLine::ID::KeyPressVisAdvModeColor3;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed4: return ParameterLine::ID::KeyPressVisAdvModeSpeed4;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation4: return ParameterLine::ID::KeyPressVisAdvModeRotation4;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen4: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen4;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig4: return ParameterLine::ID::KeyPressVisAdvModeOrig4;
        case LogicalParameter::ID::KeyPressVisAdvModeColor4: return ParameterLine::ID::KeyPressVisAdvModeColor4;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed5: return ParameterLine::ID::KeyPressVisAdvModeSpeed5;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation5: return ParameterLine::ID::KeyPressVisAdvModeRotation5;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen5: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen5;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig5: return ParameterLine::ID::KeyPressVisAdvModeOrig5;
        case LogicalParameter::ID::KeyPressVisAdvModeColor5: return ParameterLine::ID::KeyPressVisAdvModeColor5;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed6: return ParameterLine::ID::KeyPressVisAdvModeSpeed6;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation6: return ParameterLine::ID::KeyPressVisAdvModeRotation6;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen6: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen6;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig6: return ParameterLine::ID::KeyPressVisAdvModeOrig6;
        case LogicalParameter::ID::KeyPressVisAdvModeColor6: return ParameterLine::ID::KeyPressVisAdvModeColor6;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed7: return ParameterLine::ID::KeyPressVisAdvModeSpeed7;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation7: return ParameterLine::ID::KeyPressVisAdvModeRotation7;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen7: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen7;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig7: return ParameterLine::ID::KeyPressVisAdvModeOrig7;
        case LogicalParameter::ID::KeyPressVisAdvModeColor7: return ParameterLine::ID::KeyPressVisAdvModeColor7;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed8: return ParameterLine::ID::KeyPressVisAdvModeSpeed8;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation8: return ParameterLine::ID::KeyPressVisAdvModeRotation8;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen8: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen8;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig8: return ParameterLine::ID::KeyPressVisAdvModeOrig8;
        case LogicalParameter::ID::KeyPressVisAdvModeColor8: return ParameterLine::ID::KeyPressVisAdvModeColor8;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed9: return ParameterLine::ID::KeyPressVisAdvModeSpeed9;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation9: return ParameterLine::ID::KeyPressVisAdvModeRotation9;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen9: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen9;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig9: return ParameterLine::ID::KeyPressVisAdvModeOrig9;
        case LogicalParameter::ID::KeyPressVisAdvModeColor9: return ParameterLine::ID::KeyPressVisAdvModeColor9;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed10: return ParameterLine::ID::KeyPressVisAdvModeSpeed10;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation10: return ParameterLine::ID::KeyPressVisAdvModeRotation10;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen10: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen10;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig10: return ParameterLine::ID::KeyPressVisAdvModeOrig10;
        case LogicalParameter::ID::KeyPressVisAdvModeColor10: return ParameterLine::ID::KeyPressVisAdvModeColor10;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed11: return ParameterLine::ID::KeyPressVisAdvModeSpeed11;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation11: return ParameterLine::ID::KeyPressVisAdvModeRotation11;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen11: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen11;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig11: return ParameterLine::ID::KeyPressVisAdvModeOrig11;
        case LogicalParameter::ID::KeyPressVisAdvModeColor11: return ParameterLine::ID::KeyPressVisAdvModeColor11;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed12: return ParameterLine::ID::KeyPressVisAdvModeSpeed12;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation12: return ParameterLine::ID::KeyPressVisAdvModeRotation12;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen12: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen12;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig12: return ParameterLine::ID::KeyPressVisAdvModeOrig12;
        case LogicalParameter::ID::KeyPressVisAdvModeColor12: return ParameterLine::ID::KeyPressVisAdvModeColor12;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed13: return ParameterLine::ID::KeyPressVisAdvModeSpeed13;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation13: return ParameterLine::ID::KeyPressVisAdvModeRotation13;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen13: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen13;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig13: return ParameterLine::ID::KeyPressVisAdvModeOrig13;
        case LogicalParameter::ID::KeyPressVisAdvModeColor13: return ParameterLine::ID::KeyPressVisAdvModeColor13;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed14: return ParameterLine::ID::KeyPressVisAdvModeSpeed14;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation14: return ParameterLine::ID::KeyPressVisAdvModeRotation14;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen14: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen14;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig14: return ParameterLine::ID::KeyPressVisAdvModeOrig14;
        case LogicalParameter::ID::KeyPressVisAdvModeColor14: return ParameterLine::ID::KeyPressVisAdvModeColor14;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed15: return ParameterLine::ID::KeyPressVisAdvModeSpeed15;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation15: return ParameterLine::ID::KeyPressVisAdvModeRotation15;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen15: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen15;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig15: return ParameterLine::ID::KeyPressVisAdvModeOrig15;
        case LogicalParameter::ID::KeyPressVisAdvModeColor15: return ParameterLine::ID::KeyPressVisAdvModeColor15;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed16: return ParameterLine::ID::KeyPressVisAdvModeSpeed16;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation16: return ParameterLine::ID::KeyPressVisAdvModeRotation16;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen16: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen16;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig16: return ParameterLine::ID::KeyPressVisAdvModeOrig16;
        case LogicalParameter::ID::KeyPressVisAdvModeColor16: return ParameterLine::ID::KeyPressVisAdvModeColor16;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed17: return ParameterLine::ID::KeyPressVisAdvModeSpeed17;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation17: return ParameterLine::ID::KeyPressVisAdvModeRotation17;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen17: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen17;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig17: return ParameterLine::ID::KeyPressVisAdvModeOrig17;
        case LogicalParameter::ID::KeyPressVisAdvModeColor17: return ParameterLine::ID::KeyPressVisAdvModeColor17;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed18: return ParameterLine::ID::KeyPressVisAdvModeSpeed18;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation18: return ParameterLine::ID::KeyPressVisAdvModeRotation18;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen18: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen18;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig18: return ParameterLine::ID::KeyPressVisAdvModeOrig18;
        case LogicalParameter::ID::KeyPressVisAdvModeColor18: return ParameterLine::ID::KeyPressVisAdvModeColor18;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed19: return ParameterLine::ID::KeyPressVisAdvModeSpeed19;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation19: return ParameterLine::ID::KeyPressVisAdvModeRotation19;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen19: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen19;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig19: return ParameterLine::ID::KeyPressVisAdvModeOrig19;
        case LogicalParameter::ID::KeyPressVisAdvModeColor19: return ParameterLine::ID::KeyPressVisAdvModeColor19;
        case LogicalParameter::ID::KeyPressVisAdvModeSpeed20: return ParameterLine::ID::KeyPressVisAdvModeSpeed20;
        case LogicalParameter::ID::KeyPressVisAdvModeRotation20: return ParameterLine::ID::KeyPressVisAdvModeRotation20;
        case LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen20: return ParameterLine::ID::KeyPressVisAdvModeFadeLineLen20;
        case LogicalParameter::ID::KeyPressVisAdvModeOrig20: return ParameterLine::ID::KeyPressVisAdvModeOrig20;
        case LogicalParameter::ID::KeyPressVisAdvModeColor20: return ParameterLine::ID::KeyPressVisAdvModeColor20;

        case LogicalParameter::ID::OtherSaveStats: return ParameterLine::ID::OtherSaveStats;
        case LogicalParameter::ID::OtherShowOppOnAlt: return ParameterLine::ID::OtherShowOppOnAlt;
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
        case LogicalParameter::ID::SaveStatTotal16: return ParameterLine::ID::SaveStatTotal16;
        case LogicalParameter::ID::SaveStatTotal17: return ParameterLine::ID::SaveStatTotal17;
        case LogicalParameter::ID::SaveStatTotal18: return ParameterLine::ID::SaveStatTotal18;
        case LogicalParameter::ID::SaveStatTotal19: return ParameterLine::ID::SaveStatTotal19;
        case LogicalParameter::ID::SaveStatTotal20: return ParameterLine::ID::SaveStatTotal20;

        default: 
            std::cerr << "Non handled LogicalParameter::ID was passed - " << static_cast<size_t>(id) << std::endl;
            assert(false);
            return ParameterLine::ID::StatTextColl;
    }
}
