#include "../Headers/ParameterLine.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/ResourceIdentifiers.hpp"
#include "../Headers/StringHelper.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>
#include <cmath>
#include <mutex>


sf::RectangleShape ParameterLine::mCursor(sf::Vector2f(1, 21));
std::shared_ptr<ParameterLine> ParameterLine::mSelectedLine(nullptr);
std::shared_ptr<GraphicalParameter> ParameterLine::mSelectedValue(nullptr);
int ParameterLine::mSelectedValueIndex(-1);
Palette ParameterLine::mPalette(0);

ParameterLine::ParameterLine(
    std::shared_ptr<LogicalParameter> parameter,
    const FontHolder &fonts, 
    const TextureHolder &textures,
    sf::RenderWindow &window)
: mWindow(window)
, mType(parameter->mType)
, mParameter(parameter)
, mRectLine(sf::Vector2f(800, 40))
, mColorButtonP(nullptr)
, paramValWasChanged(false)
, mIsThRunning(false)
{
    mRectLine.setFillColor(pickColor(mType));
    mParameterName.setString(parameter->mParName);
    mParameterName.setFont(fonts.get(Fonts::Parameter));
    mParameterName.setCharacterSize(20);
    // Don't let text go beyound 1.8 of the button width
    while (mType != LogicalParameter::Type::Collection && mType != LogicalParameter::Type::Empty
    && mParameterName.getLocalBounds().width >= mRectLine.getSize().x / 1.8)
    {
        mParameterName.setCharacterSize(mParameterName.getCharacterSize() - 1);
    }

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

void ParameterLine::handleEvent(sf::Event event)
{
    handleButtonsInteractionEvent(event);
    handleValueModEvent(event);
}

void ParameterLine::handleButtonsInteractionEvent(sf::Event event)
{
    // Mouse/keyboard events to control buttons selection/deselection
    if (event.type == sf::Event::MouseButtonPressed 
    ||  event.type == sf::Event::KeyPressed)
    {
        float viewOffsetY = mWindow.getView().getCenter().y - mWindow.getSize().y / 2;
        sf::Vector2f mousePos(static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow)));
        mousePos.y += viewOffsetY;
        sf::Mouse::Button button = event.mouseButton.button;
        sf::Keyboard::Key key = event.key.code;

        // Check click on buttons
        for (const auto &elem : mParameterValues)
        {
            sf::FloatRect buttonBounds = elem->getTransform().transformRect(elem->getGlobalBounds());
            buttonBounds.top += getPosition().y;

            if (button == sf::Mouse::Left && buttonBounds.contains(mousePos))
            {
                if (mType == LogicalParameter::Type::Bool)
                {
                    mParameter->setValStr(LogicalParameter::getInverseBool(mParameter->getBool()));
                    elem->mValText.setString(mParameter->getValStr());
                    elem->setInverseMark();
                    deselect();
                    return;
                }
                if (mType != LogicalParameter::Type::Color && mPalette.isWindowOpen())
                    mPalette.closeWindow();
                deselect();
                select(elem);
            }

            if (key == sf::Keyboard::Escape || button == sf::Mouse::Right)
            {
                // reset to previous
                deselect();
            }

            if (key == sf::Keyboard::Enter)
            {
                // save
                deselect();
            }

            if (key == sf::Keyboard::Tab && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            {
                if (isItSelectedLine(shared_from_this()))
                {
                    for (auto it = mParameterValues.begin(); it < mParameterValues.end(); ++it)
                    {
                        if (*it == mSelectedValue)
                        {
                            deselect(); 
                            // is the previous element the first one ? ... : ...
                            select(it == mParameterValues.begin() ? mParameterValues.back() : *(it - 1));
                            // return in order to avoid further tabulation and segmentation fault
                            return;
                        }
                    }
                }
            }
            else if (key == sf::Keyboard::Tab)
            {
                if (isItSelectedLine(shared_from_this()))
                {
                    for (auto it = mParameterValues.begin(); it < mParameterValues.end(); ++it)
                    {
                        if (*it == mSelectedValue)
                        {
                            deselect(); 
                            // isn't next element the end ? ... : ...
                            select(it + 1 != mParameterValues.end() ? *(it + 1) : mParameterValues[0]);
                            // return in order to avoid further tabulation and segmentation fault
                            return;
                        }
                    }
                }
            }
        }

        // Check click on RGB circle
        if (mType == LogicalParameter::Type::Color)
        {
            sf::FloatRect rgbCircleBounds = mColorButtonP->getTransform().transformRect(
                mColorButtonP->rgbCircleSprite.getGlobalBounds());
            rgbCircleBounds.top += getPosition().y;

            if (button == sf::Mouse::Left
            &&  rgbCircleBounds.contains(mousePos))
            {
                // If there is already selected line, but it isn't this one
                if (mSelectedLine && !isItSelectedLine(shared_from_this()))
                    deselect();
                // If nothing is selected
                if (!mSelectedLine)
                    select(mParameterValues[0]);

                // mPalette.setColorOnPalette(lineToColor(mSelectedLine));
                mPalette.openWindow();
            }
        }
    }
}

// Keyboard events which modify values
void ParameterLine::handleValueModEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed)
    {
        if (mSelectedValue == nullptr || !isSelectedValHere()) 
            return;

        std::string str(mSelectedValue->mValText.getString());
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
            str = mParameter->getValPt(btnIdx);
        }
        assert(!str.empty());
        sf::Keyboard::Key key = event.key.code;
        

        if (key == sf::Keyboard::Backspace)
        {
            if (mSelectedValueIndex != 0)
            {
                if (str.size() == 1 
                || (str.size() == 2 && str[0] == '-' && mSelectedValueIndex == 2))
                {
                    str.resize(str.size() - 1);
                    str.append(mParameter->mLowLimits <= 0 ? "0" : std::to_string(static_cast<int>(mParameter->mLowLimits)));
                    mSelectedValueIndex = str.size() + 1;
                }
                else
                {
                    rmChOnIdx(str, mSelectedValueIndex - 1);
                    // ex.: -500, remove 5, result "-0"
                    if (std::stoi(str) == 0)
                        str = std::string(str[0] == '-' ? "-" : "") + "0";
                }

                --mSelectedValueIndex;
            }
        }
        
        if (key == sf::Keyboard::Delete)
        {
            if (str.size() > mSelectedValueIndex)
            {
                if (str.size() == 1 
                || (str.size() == 2 && str[0] == '-' && mSelectedValueIndex == 1))
                {
                    str[str.size() - 1] = '0';
                }
                else
                {
                    rmChOnIdx(str, mSelectedValueIndex);
                    // ex.: -500, remove 5, result "-0"
                    if (std::stoi(str) == 0)
                        str = std::string(str[0] == '-' ? "-" : "") + "0";
                }
            }
        }
        
        if (key >= sf::Keyboard::Num0 &&  key <= sf::Keyboard::Num9)
        {
            int n = (key - sf::Keyboard::Num0);

            if (mSelectedValueIndex == 0 && (str[0] == '-' || n == 0))
                return;

            if (str.size() == 1 && str[0] == '0' || 
               (str.size() == 2 && str[1] == '0' && str[0] == '-'))
            {
                str[str.size() - 1] = n + '0';
                --mSelectedValueIndex;
            }
            else
            {
                std::string prevVal = str;
                addChOnIdx(str, mSelectedValueIndex, n);
                float check = stof(str); 
                str = std::to_string(static_cast<int>(check));

                if (checkLimits(check))
                {  
                    runThread(str, prevVal);
                    --mSelectedValueIndex;
                }
            }

            ++mSelectedValueIndex;
        }

        if (key == sf::Keyboard::Up)
        {
            std::string prevVal = str;
            float check = stof(str) + 1; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);
            mSelectedValueIndex += str.length() == prevVal.length() ? 0 : str.length() > prevVal.length() ? 1 : -1;
        }

        if (key == sf::Keyboard::Down)
        {
            std::string prevVal = str;
            float check = stof(str) - 1; 
            str = std::to_string(static_cast<int>(check));

            if (checkLimits(check))
                runThread(str, prevVal);
            mSelectedValueIndex += str.length() == prevVal.length() ? 0 : str.length() > prevVal.length() ? 1 : -1;
        }

        if (key == sf::Keyboard::Hyphen)
        {
            if (mType != LogicalParameter::Type::Unsigned && mType != LogicalParameter::Type::Color
            &&  mParameter->mLowLimits >= 0)
                return;

            if (str[0] != '-')
            {
                addChOnIdx(str, 0, '-');
                ++mSelectedValueIndex;
            }
            else if (str[0] == '-')
            {
                rmChOnIdx(str, 0);
                --mSelectedValueIndex;
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

        mParameter->setValStr(str, btnIdx);
        mSelectedValue->mValText.setString(str);
        paramValWasChanged = true;

        mSelectedValue->setupValPos();
        setCursorPos();

        // Doesn't work
        // If values were changed in menu, then they must be also changed on palette
        // if (mPalette.isWindowOpen())
        //     mPalette.setColorOnPalette(lineToColor(mSelectedLine));
    }
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
    std::shared_ptr<GraphicalParameter> val = nullptr; 
    if (mType == LogicalParameter::Type::Bool)
    {
        val = std::make_shared<GraphicalParameter>(readValue(valueStr, 0), textures);
        val->setPosition(sf::Vector2f(mRectLine.getSize().x - 
            GraphicalParameter::getPosX(), mRectLine.getSize().y / 2));
        mParameterValues.emplace_back(std::move(val));
        return;
    }

    for (unsigned i = 0; i < readAmountOfParms(valueStr); ++i)
    {
        val = std::make_shared<GraphicalParameter>(readValue(valueStr, i), fonts.get(Fonts::Value), i, textures);

        val->setPosition(val->getPosition() + sf::Vector2f(mRectLine.getSize().x - 
            GraphicalParameter::getPosX(), mRectLine.getSize().y / 2));
        
        if (mType == LogicalParameter::Type::Color && i == 0)
        {
            float dist = 65.f;
            mColorButtonP = std::make_unique<ColorButton>(textures.get(Textures::rgbCircle));
            mColorButtonP->setPosition(val->getPosition().x - dist, mRectLine.getSize().y / 2);
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


void ParameterLine::select(std::shared_ptr<GraphicalParameter> ptr)
{
    mSelectedValue = ptr;
    mSelectedLine = shared_from_this();
    mSelectedValue->mRect.setFillColor(GraphicalParameter::defaultSelectedRectColor);
    mSelectedValueIndex = mSelectedValue->mValText.getString().getSize();
    setCursorPos();
}

void ParameterLine::deselect()
{
    if (mSelectedValue == nullptr && mSelectedLine == nullptr)
        return;

    assert(mSelectedValue != nullptr || mSelectedLine != nullptr);

    mSelectedValue->mRect.setFillColor(sf::Color(120,120,120));
    mSelectedValue = nullptr;
    mSelectedLine = nullptr;
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
    std::shared_ptr<GraphicalParameter> gfxPar = mSelectedValue;
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
                gfxPar == mSelectedValue ? GraphicalParameter::defaultSelectedRectColor : GraphicalParameter::defaultRectColor);

            mtx.unlock();
        }
    }
    mtx.lock();
    gfxPar->mRect.setFillColor(gfxPar == mSelectedValue ? 
        GraphicalParameter::defaultSelectedRectColor : GraphicalParameter::defaultRectColor);
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
        case LogicalParameter::ID::SpaceBtwBtnAndStat: return ParameterLine::ID::SpaceBtwBtnAndStat;
        case LogicalParameter::ID::StatTextFont: return ParameterLine::ID::StatTextFont;
        case LogicalParameter::ID::StatTextClr: return ParameterLine::ID::StatTextClr;
        case LogicalParameter::ID::StatTextChSz: return ParameterLine::ID::StatTextChSz;
        case LogicalParameter::ID::StatTextBold: return ParameterLine::ID::StatTextBold;
        case LogicalParameter::ID::StatTextItal: return ParameterLine::ID::StatTextItal;
        case LogicalParameter::ID::StatTextShow: return ParameterLine::ID::StatTextShow;
        case LogicalParameter::ID::StatTextShowKPS: return ParameterLine::ID::StatTextShowKPS;
        case LogicalParameter::ID::StatTextShowMaxKPS: return ParameterLine::ID::StatTextShowMaxKPS;
        case LogicalParameter::ID::StatTextShowTotal: return ParameterLine::ID::StatTextShowTotal;
        case LogicalParameter::ID::StatTextShowBPM: return ParameterLine::ID::StatTextShowBPM;
        case LogicalParameter::ID::BtnTextFont: return ParameterLine::ID::BtnTextFont;
        case LogicalParameter::ID::BtnTextClr: return ParameterLine::ID::BtnTextClr;
        case LogicalParameter::ID::BtnTextChSz: return ParameterLine::ID::BtnTextChSz;
        case LogicalParameter::ID::BtnTextWidth: return ParameterLine::ID::BtnTextWidth;
        case LogicalParameter::ID::BtnTextHeight: return ParameterLine::ID::BtnTextHeight;
        case LogicalParameter::ID::BtnTextHorzBounds: return ParameterLine::ID::BtnTextHorzBounds;
        case LogicalParameter::ID::BtnTextVertBounds: return ParameterLine::ID::BtnTextVertBounds;
        case LogicalParameter::ID::BtnTextBold: return ParameterLine::ID::BtnTextBold;
        case LogicalParameter::ID::BtnTextItal: return ParameterLine::ID::BtnTextItal;
        case LogicalParameter::ID::BtnTextShowKeys: return ParameterLine::ID::BtnTextShowKeys;
        case LogicalParameter::ID::BtnTextShowKeyCtrs: return ParameterLine::ID::BtnTextShowKeyCtrs;
        case LogicalParameter::ID::BtnGfxDist: return ParameterLine::ID::BtnGfxDist;
        case LogicalParameter::ID::BtnGfxTxtr: return ParameterLine::ID::BtnGfxTxtr;
        case LogicalParameter::ID::BtnGfxTxtrSz: return ParameterLine::ID::BtnGfxTxtrSz;
        case LogicalParameter::ID::BtnGfxTxtrClr: return ParameterLine::ID::BtnGfxTxtrClr;
        case LogicalParameter::ID::AnimGfxStl: return ParameterLine::ID::AnimGfxStl;
        case LogicalParameter::ID::AnimGfxTxtr: return ParameterLine::ID::AnimGfxTxtr;
        case LogicalParameter::ID::AnimGfxVel: return ParameterLine::ID::AnimGfxVel;
        case LogicalParameter::ID::AnimGfxScl: return ParameterLine::ID::AnimGfxScl;
        case LogicalParameter::ID::AnimGfxClr: return ParameterLine::ID::AnimGfxClr;
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
        case LogicalParameter::ID::OtherHighText: return ParameterLine::ID::OtherHighText;
        case LogicalParameter::ID::ThemeDevMultpl: return ParameterLine::ID::ThemeDevMultpl;

        default: return ParameterLine::ID::StatTextColl;
    }
}
