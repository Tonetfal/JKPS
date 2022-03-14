#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Default media/Textures/vMark.hpp"
#include "../Headers/Default media/Textures/xMark.hpp"
#include "../Headers/Default media/Textures/RGB-Circle.hpp"
#include "../Headers/Default media/Textures/RefreshTexture.hpp"
#include "../Headers/Default media/Fonts/arial_monospaced.hpp"
#include "../Headers/Default media/Fonts/RobotoMono.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>

#include <limits.h>


Menu::Menu()
: mScrollSpeed(40.f)
, mSliderBar(sf::Vector2f(10.f, 200.f))
, mSliderBarDefaultColor(sf::Color(103,103,103))
, mSliderBarAimedColor(sf::Color(123,123,123))
, mSliderBarPressedColor(sf::Color(161,161,161))
, mSelectedTab(0u)
, mSelectedAdvKeyPressVisKey(0u)
{
    loadFonts();
    loadTextures();

    initCollectionNames();
    buildParametersMap();
    const auto read = ConfigHelper::readConfig(mParameters, mCollectionNames);
    buildParameterLines();
    buildMenuTabs();
    if (!read)
        ConfigHelper::saveConfig(mParameters, mParameterLines, nullptr, false);

    mSliderBar.setOrigin(mSliderBar.getSize() / 2.f);
    mSliderBar.setPosition(949.f, 100.f);
    mSliderBar.setFillColor(mSliderBarDefaultColor);

    mTabs.at(mSelectedTab)->mRect.setFillColor(GfxParameter::defaultSelectedRectColor);

    buildAdvKeys();
}

void Menu::processInput()
{
    handleEvent();
    handleRealtimeInput();
}

void Menu::handleEvent()
{
    auto switchTab = [this] (size_t index)
        {
            // Reset slider
            mSliderBar.setPosition(mSliderBar.getPosition().x, 100.f);

            // Deselect old tab
            mTabs[mSelectedTab]->mRect.setFillColor(GfxParameter::defaultRectColor);

            // Select new tab
            selectTab(index);
            mSelectedTab = index;

            mTabs[index]->mRect.setFillColor(GfxParameter::defaultSelectedRectColor);
        };

    sf::Event event;
    static bool scrollCursorClicked;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::KeyPressed
        ||  event.type == sf::Event::MouseWheelScrolled)
        {
            if (event.type == sf::Event::KeyPressed 
            &&  event.key.code == sf::Keyboard::Escape)
            {
                ParameterLine::deselectValue();
                continue;
            }

            auto isAnyLineSelected = false;

            const auto container = mTabParameters.at(mSelectedTab);
            auto it = mParameterLines.find(container.front());
            const auto end = mParameterLines.find(static_cast<ParameterLine::ID>(static_cast<size_t>(container.back()) + 1ul));
            assert(it != mParameterLines.end());
            assert(end != mParameterLines.end());

            for (; it != end; ++it)
            {
                if (it->second->handleEvent(event))
                {
                    isAnyLineSelected = true;
                    break;
                }
            }

            if (!isAnyLineSelected)
            {
                // If a key was pressed, but mouse aims at no box - deselect
                if (event.type != sf::Event::MouseWheelScrolled)
                    ParameterLine::deselectValue();

                // If the mouse wheel was scrolled and no value is selected - scroll
                if (event.type == sf::Event::MouseWheelScrolled || event.type == sf::Event::KeyPressed)
                {
                    auto offset = 0.f;

                    if (event.type == sf::Event::MouseWheelScrolled)
                    {
                        offset = -mScrollSpeed * event.mouseWheelScroll.delta;
                    }
                    if (event.type == sf::Event::KeyPressed)
                    {
                        const auto key = event.key.code;
                        const auto viewSize = mView.getSize();
                        if (key == sf::Keyboard::PageUp)
                            offset = -viewSize.y;
                        if (key == sf::Keyboard::PageDown)
                            offset = viewSize.y;
                    }

                    moveSliderBarButtons(offset);
                    returnViewInBounds();
                }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::MouseButtonReleased
        ||  event.type == sf::Event::MouseMoved)
        {
            auto mousePos = sf::Mouse::getPosition(mWindow);
            auto colorToSet = mSliderBarDefaultColor;
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
                scrollCursorClicked = false;
            if (mSliderBar.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)))
            {
                colorToSet = mSliderBarAimedColor;
                scrollCursorClicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            }
            if (scrollCursorClicked)
                colorToSet = mSliderBarPressedColor;

            mSliderBar.setFillColor(colorToSet);
            if (event.type == sf::Event::MouseMoved)
            {
                mousePos.x = event.mouseMove.x;
                mousePos.y = event.mouseMove.y;
            }
            if (scrollCursorClicked)
            {
                moveSliderBarMouse(mousePos);
            }
        }

        if (event.type == sf::Event::MouseButtonPressed 
        ||  event.type == sf::Event::MouseButtonReleased
        ||  event.type == sf::Event::MouseMoved)
        {
            const auto viewOrigin = mView.getCenter() - mView.getSize() / 2.f;
            const auto relCursorPos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow));
            const auto absCursorPos = viewOrigin + relCursorPos;

            unsigned idx = 0u;
            for (auto &tab : mTabs)
            {
                auto color = GfxParameter::defaultRectColor;
                if (tab->contains(relCursorPos))
                {
                    color = GfxParameter::defaultAimedRectColor;
                    if (event.type == sf::Event::MouseButtonPressed
                    &&  event.mouseButton.button == sf::Mouse::Left)
                    {
                        switchTab(idx);
                    }
                }

                if (mSelectedTab == idx)
                    color = GfxParameter::defaultSelectedRectColor;

                tab->mRect.setFillColor(color);
                ++idx;
            }
            
            for (auto &[id, block] : mKeyBlocks)
            {
                block->handleEvent(event, absCursorPos);
            }
        }

        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.code == sf::Keyboard::Tab)
            {
                const auto lCtrl = key.control;
                const auto lShift = key.shift;

                // Tab forward
                if (!lShift && lCtrl)
                {
                    const auto nextTabIdx = (mSelectedTab + 1) % mTabs.size();
                    switchTab(nextTabIdx);
                }

                // Tab backward
                else if (lShift && lCtrl)
                {
                    const auto size = mTabs.size();
                    const auto prevTabIdx = (mSelectedTab + size - 1ul) % size;
                    switchTab(prevTabIdx);
                }
            }
        }
        
        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.control && key.code == Settings::KeyExit)
            {
                closeWindow();
                return;
            }
        }

        if (event.type == sf::Event::Closed)
        {
            closeWindow();
            return;
        }
    }
}

void Menu::handleRealtimeInput()
{
    if (mWindow.hasFocus() && !ParameterLine::isValueSelected())
    {
        auto offset = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            offset = -mScrollSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            offset = mScrollSpeed;
        if (offset != 0.f)
        {
            moveSliderBarButtons(offset);
            returnViewInBounds();
        }
    }

    const auto container = mTabParameters.at(mSelectedTab);
    auto it = mParameterLines.find(container.front());
    const auto end = mParameterLines.find(static_cast<ParameterLine::ID>(static_cast<size_t>(container.back()) + 1ul));
    assert(it != mParameterLines.end());
    assert(end != mParameterLines.end());

    for (; it != end; ++it)
    {
        it->second->processInput();
    }

    for (auto &elem : mParameters)
    {
        if (elem.second->resetChangeState())
            mChangedParametersQueue.push(elem);
    }
}

void Menu::update()
{
}

void Menu::render() 
{
    mWindow.setView(mView);

    mWindow.clear(sf::Color(45,45,45));

    const auto container = mTabParameters.at(mSelectedTab);
    auto it = mParameterLines.find(container.front());
    const auto end = mParameterLines.find(static_cast<ParameterLine::ID>(static_cast<size_t>(container.back()) + 1ul));
    assert(it != mParameterLines.end());
    assert(end != mParameterLines.end());

    for (; it != end; ++it)
    {
        mWindow.draw(*it->second);
    }

    for (const auto &[id, block] : mKeyBlocks)
    {
        mWindow.draw(*block);
    }

    auto transform = sf::Transform::Identity;
    mWindow.setView(mWindow.getDefaultView());

    mWindow.draw(mTabsBackground);
    for (const auto &elem : mTabs)
    {
        mWindow.draw(*elem, transform);
    }
    mWindow.draw(mSliderBar);

    mWindow.setView(mView);

    mWindow.display();
}

void Menu::openWindow()
{
    sf::Uint32 style;
#ifdef _WIN32
    style = sf::Style::Close;
#elif linux
    style = sf::Style::Default;
#else
#error Unsupported compiler
#endif

    mWindow.create(sf::VideoMode(959, 700), "JKPS Menu", style);
    mView = mWindow.getView();
    selectTab(mSelectedTab);
    mSliderBar.setPosition(mSliderBar.getPosition().x, mSliderBar.getSize().y / 2);
}

void Menu::closeWindow()
{
    mWindow.close();
    ParameterLine::deselectValue();
}

bool Menu::isOpen() const
{
    return mWindow.isOpen();
}

LogicalParameter &Menu::getParameter(LogicalParameter::ID id)
{
    auto found = mParameters.find(id);
    assert(found != mParameters.end());

    return *found->second;
}

ChangedParametersQueue &Menu::getChangedParametersQueue()
{
    return mChangedParametersQueue;
}

void Menu::loadFonts()
{
    mFonts.loadFromMemory(Fonts::Parameter, RobotoMono, 1100000);
    mFonts.loadFromMemory(Fonts::Value, RobotoMono, 1100000);
}

void Menu::loadTextures()
{
    mTextures.loadFromMemory(Textures::rgbCircle, RGB_Circle, 10700);
    mTextures.loadFromMemory(Textures::vMark, vMark, 6000);
    mTextures.loadFromMemory(Textures::xMark, xMark, 6100);
    mTextures.loadFromMemory(Textures::Refresh, RefreshTexture, 6000);
}

void Menu::selectTab(unsigned idx)
{
    mView.setCenter(1000.f * static_cast<float>(idx) + static_cast<float>(mWindow.getSize().x) / 2.f, 0.f);
    mWindow.setView(mView);
}

void Menu::selectAdvKeyPressVisKey(unsigned idx)
{
    const auto prevIdx = mSelectedAdvKeyPressVisKey;
    auto increment = [] (ParameterLine::ID id, int val)
        {
            return static_cast<ParameterLine::ID>(static_cast<int>(id) + val);
        };
    auto lineToHide = increment(ParameterLine::ID::KeyPressVisAdvModeSpeed1, static_cast<int>(prevIdx * 5u));
    auto lineToShow = increment(ParameterLine::ID::KeyPressVisAdvModeSpeed1, static_cast<int>(idx * 5u));

    for (size_t i = 0; i < 5lu; ++i)
    {
        auto toHideFound = mParameterLines.find(lineToHide);
        auto toShowFound = mParameterLines.find(lineToShow);
        assert(toHideFound != mParameterLines.end());
        assert(toShowFound != mParameterLines.end());
        toShowFound->second->setPosition(toHideFound->second->getPosition());
        toHideFound->second->setPosition(sf::Vector2f(-1000.f, -1000.f));

        lineToHide = increment(lineToHide, 1);
        lineToShow = increment(lineToShow, 1);
    }
}

void Menu::initCollectionNames()
{
    mCollectionNames.emplace_back("[Statistics text]");
    mCollectionNames.emplace_back("[Statistics text advanced settings]");
    mCollectionNames.emplace_back("[Statistics text strings]");
    mCollectionNames.emplace_back("[Buttons text]");
    mCollectionNames.emplace_back("[Buttons text advanced settings]");
    mCollectionNames.emplace_back("[Button graphics]");
    mCollectionNames.emplace_back("[Button graphics advanced settings]");
    mCollectionNames.emplace_back("[Common parameters]");
    mCollectionNames.emplace_back("[Light animation]");
    mCollectionNames.emplace_back("[Press animation]");
    mCollectionNames.emplace_back("[Main window]");
    mCollectionNames.emplace_back("[Extra KPS window]");
    mCollectionNames.emplace_back("[Key press visualization]");
    mCollectionNames.emplace_back("[Key press visualization advanced settings]");
    mCollectionNames.emplace_back("[Other]");
    mCollectionNames.emplace_back("[Statistics save]");
}

void Menu::buildMenuTabs()
{
    // Font must be loaded before tabs can be constructed

    std::unique_ptr<GfxParameter> tabPtr = nullptr;
    const sf::Vector2f offset(5.f, 5.f);
    const sf::Vector2f distBtw(0.f, 5.f);
    const sf::Vector2f tabSize(140.f, 25.f);
    const sf::Vector2f highTabSize(140.f, 50.f);

    auto addTab = [&] (const auto &str, sf::Vector2f tabSize)
        {
            auto ptr = std::make_unique<GfxParameter>(str, tabSize);
            auto prevTabPos = offset + tabSize / 2.f;
            auto prevTabSize = -tabSize;
            if (!mTabs.empty())
            {
                prevTabPos = mTabs.back()->getPosition();
                prevTabSize = mTabs.back()->mRect.getSize();
            }

            ptr->setPosition(prevTabPos + sf::Vector2f(0.f, tabSize.y / 2.f + prevTabSize.y / 2.f) + distBtw);
            mTabs.push_back(std::move(ptr));
        };

    addTab("Statistics text", tabSize);
    addTab("Buttons text", tabSize);
    addTab(" Button\ngraphics", highTabSize);
    addTab("Animations", tabSize);
    addTab("Main window", tabSize);
    addTab("Extra KPS\n  window", highTabSize);
    addTab("  Key press\nvisualization", highTabSize);
    addTab("Other", tabSize);
    addTab("Main info", tabSize);

    mTabsBackground.setSize(sf::Vector2f(tabSize.x + offset.x * 2.f, 700.f));
    mTabsBackground.setFillColor(sf::Color(35, 35, 35));
}

void buildTab(Menu::KeyBlock &container, sf::Vector2i tabCoords, size_t key)
{
    const sf::Vector2f distBtw(5.f, 5.f);
    const sf::Vector2f tabSize(72.f, 22.5f);
    const sf::Vector2f offset(5.f + tabSize.x / 2.f, distBtw.y);

    auto ptr = std::make_unique<GfxParameter>("Key " + std::to_string(key), tabSize);
    const auto position = offset + sf::Vector2f(
        (tabSize.x + distBtw.x) * static_cast<float>(tabCoords.x), 
        (tabSize.y + distBtw.y) * static_cast<float>(tabCoords.y));

    ptr->setPosition(position);
    container.push(std::move(ptr));
}

void buildTabAdv(Menu::KeyBlock &container, sf::Vector2i tabCoords, std::string string, sf::Vector2f tabSize)
{
    const sf::Vector2f distBtw(5.f, 5.f);
    const sf::Vector2f offset(5.f + tabSize.x / 2.f, distBtw.y);

    auto ptr = std::make_unique<GfxParameter>(string, tabSize);
    const auto position = offset + sf::Vector2f(
        (tabSize.x + distBtw.x) * static_cast<float>(tabCoords.x), 
        (tabSize.y + distBtw.y) * static_cast<float>(tabCoords.y));

    ptr->setPosition(position);
    container.push(std::move(ptr));
}

void Menu::buildAdvKeys()
{
    // Font must be loaded before tabs can be constructed

    auto buildTabBlock = [&] (KeyBlock &container)
        {
            size_t key = 1ul;
            for (size_t i = 0ul; i < 10ul; i++, ++key)
                buildTab(container, sf::Vector2i(static_cast<int>(i), 0), key);
            for (size_t i = 0ul; i < 10ul; i++, ++key)
                buildTab(container, sf::Vector2i(static_cast<int>(i), 1), key);
        };
    
    auto getOrigin = [this] (ParameterLine::ID id)
        {
            const auto foundAdvBtnTextSepVal = mParameterLines.find(id);
            assert(foundAdvBtnTextSepVal != mParameterLines.end());
            return foundAdvBtnTextSepVal->second->getPosition();
        };

    mKeyBlocks.emplace(std::make_pair(AdvancedKeys::StatText, std::make_unique<KeyBlock>(mParameterLines, ParameterLine::ID::StatTextAdvPos1, 7ul)));
    mKeyBlocks.at(AdvancedKeys::StatText)->setPosition(getOrigin(ParameterLine::ID::StatTextSpace1));

    mKeyBlocks.emplace(std::make_pair(AdvancedKeys::BtnTextSepVal, std::make_unique<KeyBlock>(mParameterLines, ParameterLine::ID::BtnTextAdvVisPosition1, 4ul)));
    mKeyBlocks.at(AdvancedKeys::BtnTextSepVal)->setPosition(getOrigin(ParameterLine::ID::BtnTextAdvSpace1));

    mKeyBlocks.emplace(std::make_pair(AdvancedKeys::BtnText, std::make_unique<KeyBlock>(mParameterLines, ParameterLine::ID::BtnTextAdvClr1, 8ul)));
    mKeyBlocks.at(AdvancedKeys::BtnText)->setPosition(getOrigin(ParameterLine::ID::BtnTextAdvSpace3));

    mKeyBlocks.emplace(std::make_pair(AdvancedKeys::GfxBtn, std::make_unique<KeyBlock>(mParameterLines, ParameterLine::ID::BtnGfxBtnPos1, 3ul)));
    mKeyBlocks.at(AdvancedKeys::GfxBtn)->setPosition(getOrigin(ParameterLine::ID::BtnGfxSpace));

    mKeyBlocks.emplace(std::make_pair(AdvancedKeys::KeyPressVis, std::make_unique<KeyBlock>(mParameterLines, ParameterLine::ID::KeyPressVisAdvModeSpeed1, 6ul)));
    mKeyBlocks.at(AdvancedKeys::KeyPressVis)->setPosition(getOrigin(ParameterLine::ID::KeyPressVisAdvModeSpace));

    buildTabAdv(*mKeyBlocks.at(AdvancedKeys::StatText), sf::Vector2i(0, 0), "KPS Text", sf::Vector2f(255.f, 20.f));
    buildTabAdv(*mKeyBlocks.at(AdvancedKeys::StatText), sf::Vector2i(1, 0), "Total Text", sf::Vector2f(255.f, 20.f));
    buildTabAdv(*mKeyBlocks.at(AdvancedKeys::StatText), sf::Vector2i(2, 0), "BPM Text", sf::Vector2f(255.f, 20.f));

    buildTabBlock(*mKeyBlocks.at(AdvancedKeys::BtnTextSepVal));
    buildTabBlock(*mKeyBlocks.at(AdvancedKeys::BtnText));
    buildTabBlock(*mKeyBlocks.at(AdvancedKeys::KeyPressVis));
    buildTabBlock(*mKeyBlocks.at(AdvancedKeys::GfxBtn));
}

void Menu::buildParametersMap()
{
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextDist,                new LogicalParameter(LogicalParameter::Type::Float,         &Settings::StatisticsTextDistance,                      "Distance between lines", "5", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatPos,                     new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextPosition,                      "Text position offset", "5,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatValPos,                  new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextValuePosition,                 "Value position offset", "0,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextCenterOrigin,        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextCenterOrigin,                  "Fixed position on value variation", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextFont,                new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::StatisticsTextFontPath,                      "Font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextClr,                 new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextColor,                         "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextChSz,                new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextCharacterSize,                 "Character size", "15", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextOutThck,             new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextOutlineThickness,              "Outline thickness", "0", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextOutClr,              new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextOutlineColor,                  "Outline color", "0,0,0,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBold,                new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextBold,                          "Bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextItal,                new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextItalic,                        "Italic", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShow,                new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsText,                          "Enabled", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowKPS,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsKPS,                           "Show KPS", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowTotal,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsTotal,                         "Show total", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextShowBPM,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowStatisticsBPM,                           "Show BPM", "True")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextAdvMode,             new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextAdvancedMode,                  "Enable advanced mode for statistics text", "False")));
    for (auto i = 0ul; i < GfxStatisticsLine::StatisticsIdCounter; ++i)
    {
        const auto parms = 7ul;
        const auto strPos = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvPos1) + i * parms);
        const auto valPos = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvValPos1) + i * parms);
        const auto cenOrig = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvCenterOrigin1) + i * parms);
        const auto clr = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvClr1) + i * parms);
        const auto chSz = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvChSz1) + i * parms);
        const auto bold = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvBold1) + i * parms);
        const auto italic = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::StatTextAdvItal1) + i * parms);
        const auto str = std::string(i == 0ul ? "KPS" : i == 1ul ? "Total" : "BPM");

        mParameters.emplace(std::make_pair(strPos,                                        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextAdvPosition[i],                str + " text position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(valPos,                                        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::StatisticsTextAdvValuePosition[i],           str + " value position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(cenOrig,                                       new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextAdvCenterOrigin[i],            str + " fixed position on value variation", "False")));
        mParameters.emplace(std::make_pair(clr,                                           new LogicalParameter(LogicalParameter::Type::Color,         &Settings::StatisticsTextAdvColor[i],                   str + " text color", "255,255,255")));
        mParameters.emplace(std::make_pair(chSz,                                          new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::StatisticsTextAdvCharacter[i],               str + " text character size", "15", 0, 500)));
        mParameters.emplace(std::make_pair(bold,                                          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextAdvBold[i],                    str + " bold", "False")));
        mParameters.emplace(std::make_pair(italic,                                        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::StatisticsTextAdvItalic[i],                  str + " italic", "False")));
    }

    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPSText,             new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsKPSText,                           "KPS text", "KPS: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextKPS2Text,            new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsKPS2Text,                          "KPS text when 0", "Max: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextTotalText,           new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsTotalText,                         "Total text", "Total: ")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::StatTextBPMText,             new LogicalParameter(LogicalParameter::Type::String,        &Settings::StatisticsBPMText,                           "BPM text", "BPM: ")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextFont,                 new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::ButtonTextFontPath,                          "Font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextClr,                  new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextColor,                             "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextChSz,                 new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextCharacterSize,                     "Character size", "20", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextOutThck,              new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextOutlineThickness,                  "Outline thickness", "0", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextOutClr,               new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextOutlineColor,                      "Outline color", "0,0,0,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextPosition,             new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextPosition,                          "Position offset", "0,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBoundsToggle,         new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextBoundsToggle,                      "Text bounds toggle", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBounds,               new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextBounds,                            "Text bounds", "20,20", 0, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextIgnoreBtnMovement,    new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextIgnoreBtnMovement,                 "Ignore button movement", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBold,                 new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextBold,                              "Bold", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextItal,                 new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextItalic,                            "Italic", "False")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowVisKeys,          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowVisualKeys,                    "Show key labels", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextVisPosition,          new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextVisualKeysTextPosition,            "Visual keys text position offset", "0,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowTot,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowTotal,                         "Show key counters", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextTotPosition,          new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextTotalTextPosition,                 "Key counters text position offset", "0,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowKps,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowKPS,                           "Show key KPS", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextKPSPosition,          new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextKPSTextPosition,                   "KPS text position offset", "0,0", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextShowBpm,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextShowBPM,                           "Show key BPM", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextBPMPosition,          new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextBPMTextPosition,                   "BPM text position offset", "0,0", -10000, 10000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextSepPosAdvMode,        new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextSepPosAdvancedMode,                "Enable advanced mode for separate button value positions", "False")));
    for (auto i = 0ul; i < Settings::SupportedAdvancedKeysNumber; ++i)
    {
        const auto parms = 4ul;
        const auto vis = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvVisPosition1) + i * parms);
        const auto tot = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvTotPosition1) + i * parms);
        const auto kps = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvKPSPosition1) + i * parms);
        const auto bpm = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvBPMPosition1) + i * parms);
        const auto iStr = std::to_string(i + 1);

        mParameters.emplace(std::make_pair(vis,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextAdvVisualKeysTextPosition[i],       iStr + ". Visual key text position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(tot,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextAdvTotalTextPosition[i],            iStr + ". Key counter text position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(kps,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextAdvKPSTextPosition[i],              iStr + ". KPS text position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(bpm,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextAdvBPMTextPosition[i],              iStr + ". BPM text position offset", "0,0", -10000, 10000)));
    }

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnTextPosAdvMode,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextAdvancedMode,                      "Enable advanced mode for button text", "False")));
    for (auto i = 0ul; i < Settings::SupportedAdvancedKeysNumber; ++i)
    {
        const auto parms = 8ul;
        const auto clr = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvClr1) + i * parms);
        const auto chSz = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvChSz1) + i * parms);
        const auto outThck = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvOutThck1) + i * parms);
        const auto outClr = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvOutClr1) + i * parms);
        const auto pos = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvPosition1) + i * parms);
        const auto bounds = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvBounds1) + i * parms);
        const auto bold = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvBold1) + i * parms);
        const auto ital = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnTextAdvItal1) + i * parms);
        const auto iStr = std::to_string(i + 1);

        mParameters.emplace(std::make_pair(clr,                                           new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextAdvColor[i],                       iStr + ". Text color", "255,255,255,255")));
        mParameters.emplace(std::make_pair(chSz,                                          new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextAdvCharacterSize[i],               iStr + ". Character size", "20", 0, 500)));
        mParameters.emplace(std::make_pair(outThck,                                       new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonTextAdvOutlineThickness[i],            iStr + ". Outline thickness", "0", 0, 500)));
        mParameters.emplace(std::make_pair(outClr,                                        new LogicalParameter(LogicalParameter::Type::Color,         &Settings::ButtonTextAdvOutlineColor[i],                iStr + ". Outline color", "0,0,0,255")));
        mParameters.emplace(std::make_pair(pos,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonsTextAdvPosition[i],                   iStr + ". Position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(bounds,                                        new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::ButtonTextAdvBounds[i],                      iStr + ". Text bounds", "20,20", 0, 10000)));
        mParameters.emplace(std::make_pair(bold,                                          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextAdvBold[i],                        iStr + ". Bold", "False")));
        mParameters.emplace(std::make_pair(ital,                                          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ButtonTextAdvItalic[i],                      iStr + ". Italic", "False")));
    }

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxDist,                  new LogicalParameter(LogicalParameter::Type::Float,         &Settings::GfxButtonDistance,                           "Distance between buttons", "4", -500, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtr,                  new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::GfxButtonTexturePath,                        "Texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrSz,                new LogicalParameter(LogicalParameter::Type::VectorU,       &Settings::GfxButtonTextureSize,                        "Texture size", "50,50", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxTxtrClr,               new LogicalParameter(LogicalParameter::Type::Color,         &Settings::GfxButtonTextureColor,                       "Texture color", "30,30,30,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BtnGfxAdvMode,               new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::GfxButtonAdvancedMode,                       "Enable advanced mode for button graphics", "False")));
    for (auto i = 0ul; i < Settings::SupportedAdvancedKeysNumber; ++i)
    {
        const auto parms = 3ul;
        const auto pos = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnGfxBtnPos1) + i * parms);
        const auto sz = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnGfxSz1) + i * parms);
        const auto clr = static_cast<LogicalParameter::ID>(static_cast<size_t>(LogicalParameter::ID::BtnGfxClr1) + i * parms);
        const auto iStr = std::to_string(i + 1);

        mParameters.emplace(std::make_pair(pos,                                           new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::GfxButtonsBtnPositions[i],                   iStr + ". Position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(sz,                                            new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::GfxButtonsSizes[i],                          iStr + ". Size", "50,50", 0, 500)));
        mParameters.emplace(std::make_pair(clr,                                           new LogicalParameter(LogicalParameter::Type::Color,         &Settings::GfxButtonsColor[i],                          iStr + ". Color", "30,30,30,255")));
    }
    
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxVel,                  new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::AnimationFrames,                             "Animation duration (frames)", "5", 1, 120)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxLight,                new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::LightAnimation,                              "Enabled", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxTxtr,                 new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::AnimationTexturePath,                        "Texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxScl,                  new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::AnimationScale,                              "Scale on click (%)", "100,100", 0, 200)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxClr,                  new LogicalParameter(LogicalParameter::Type::Color,         &Settings::AnimationColor,                              "Color", "255,180,0,255")));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxPress,                new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::PressAnimation,                              "Enabled", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::AnimGfxOffset,               new LogicalParameter(LogicalParameter::Type::Float,         &Settings::AnimationOffset,                             "Offset", "3", -100, 100)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgTxtr,                      new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::BackgroundTexturePath,                       "Background texture filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgClr,                       new LogicalParameter(LogicalParameter::Type::Color,         &Settings::BackgroundColor,                             "Background color", "140,140,140,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::BgScale,                     new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ScaleBackground,                             "Scale background texture if it does not fit", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTitleBar,            new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::WindowTitleBar,                              "Title bar", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwTop,                 new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeTop,                          "Bonus size top", "6", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwBot,                 new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeBottom,                       "Bonus size bottom", "6", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwLft,                 new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeLeft,                         "Bonus size left", "6", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::MainWndwRght,                new LogicalParameter(LogicalParameter::Type::Int,           &Settings::WindowBonusSizeRight,                        "Bonus size right", "105", -10000, 10000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwEn,                   new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::KPSWindowEnabledFromStart,                   "Enable from start", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwSz,                   new LogicalParameter(LogicalParameter::Type::VectorU,       &Settings::KPSWindowSize,                               "Window size", "300,300", 0, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtChSz,              new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KPSTextSize,                                 "Text character size:", "130", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumChSz,              new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KPSNumberSize,                               "Number character size", "100", 0, 500)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwBgClr,                new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSBackgroundColor,                          "Background color", "0,177,64,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtClr,               new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSTextColor,                                "Text color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumClr,               new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KPSNumberColor,                              "Number color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTxtFont,              new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::KPSWindowTextFontPath,                       "Text font filepath", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwNumFont,              new LogicalParameter(LogicalParameter::Type::StringPath,    &Settings::KPSWindowNumberFontPath,                     "Number font filepaht", "Default")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwTopPadding,           new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KPSWindowTopPadding,                         "Top padding", "20", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KPSWndwDistBtw,              new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KPSWindowDistanceBetween,                    "Distance between text", "50", -10000, 10000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisToggle,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::KeyPressVisToggle,                           "Enabled", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisSpeed,            new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisSpeed,                            "Speed", "60", -5000, 5000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisRotation,         new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisRotation,                         "Movement rotation", "0", -360, 360)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisOrig,             new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::KeyPressVisOrig,                             "Spawn position offset", "0,-5", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisFadeLineLen,      new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisFadeLineLen,                      "Fade out distance", "500", -10000, 10000)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisColor,            new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KeyPressVisColor,                            "Color", "255,255,255,255")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisWidthScale,       new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressWidthScale,                          "Bonus width scale (%)", "100", -1000, 1000)));
    
    mParameters.emplace(std::make_pair(LogicalParameter::ID::KeyPressVisAdvMode,          new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::KeyPressVisAdvSettingsMode,                  "Enable advanced mode for key press visualization", "False")));
    for (auto i = 0lu; i < Settings::SupportedAdvancedKeysNumber; ++i)
    {
        const auto parms = 6ul;
        const auto speed =             LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeSpeed1) + i * parms);
        const auto rot =               LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeRotation1) + i * parms);
        const auto len =               LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeFadeLineLen1) + i * parms);
        const auto orig =              LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeOrig1) + i * parms);
        const auto clr =               LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeColor1) + i * parms);
        const auto wScale =            LogicalParameter::ID(unsigned(LogicalParameter::ID::KeyPressVisAdvModeWidthScale1) + i * parms);
        const auto iStr = std::to_string(i + 1);

        mParameters.emplace(std::make_pair(speed,                                         new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisAdvSpeed[i],                      iStr + ". Speed", "60", -5000, 5000)));
        mParameters.emplace(std::make_pair(rot,                                           new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisAdvRotation[i],                   iStr + ". Rotation", "0", -360, 360)));
        mParameters.emplace(std::make_pair(orig,                                          new LogicalParameter(LogicalParameter::Type::VectorF,       &Settings::KeyPressVisAdvOrig[i],                       iStr + ". Spawn position offset", "0,0", -10000, 10000)));
        mParameters.emplace(std::make_pair(len,                                           new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressVisAdvFadeLineLen[i],                iStr + ". Fade line length", "500", -10000, 10000)));
        mParameters.emplace(std::make_pair(clr,                                           new LogicalParameter(LogicalParameter::Type::Color,         &Settings::KeyPressVisAdvColor[i],                      iStr + ". Color", "255,255,255,255")));
        mParameters.emplace(std::make_pair(wScale,                                        new LogicalParameter(LogicalParameter::Type::Float,         &Settings::KeyPressAdvWidthScale[i],                    iStr + ". Bonus width scale", "100", -1000, 1000)));
    }

    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherSaveStats,              new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::SaveStats,                                   "Update statistics on quit", "False")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherShowOppOnAlt,           new LogicalParameter(LogicalParameter::Type::Bool,          &Settings::ShowOppOnAlt,                                "Show opposite key values on alt press", "True")));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::OtherMultpl,                 new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::ButtonPressMultiplier,                       "Value to multiply on click", "1", 0, 1000000)));

    mParameters.emplace(std::make_pair(LogicalParameter::ID::SaveStatMaxKPS,              new LogicalParameter(LogicalParameter::Type::Float,         &Settings::MaxKPS,                                      "Saved max KPS", "0", 0u, UINT_MAX)));
    mParameters.emplace(std::make_pair(LogicalParameter::ID::SaveStatTotal,               new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::Total,                                       "Saved total", "0", 0u, UINT_MAX)));
    for (auto i = 0ul; i < Settings::SupportedAdvancedKeysNumber; ++i)
    {
        auto id =                      LogicalParameter::ID(unsigned(LogicalParameter::ID::SaveStatTotal1) + i);
        mParameters.emplace(std::make_pair(id,                                            new LogicalParameter(LogicalParameter::Type::Unsigned,      &Settings::KeysTotal[i],                                "Saved total " + std::to_string(i + 1), "0", 0, UINT_MAX)));
    }
}

void Menu::buildParameterLines()
{
    for (auto &pair : mParameters)
    {
        mParameterLines.emplace(std::make_pair(ParameterLine::parIdToParLineId(pair.first), 
            new ParameterLine(pair.second, mFonts, mTextures, mWindow)));
    }
    
    using sPtr = std::shared_ptr<LogicalParameter>;
    sPtr parP = nullptr;
    sPtr emptyP(new LogicalParameter(LogicalParameter::Type::Empty, nullptr));
    size_t collectionNameIdx = 0u;

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextSpace1, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvStrColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::StatTextAdvStrMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvSpace1, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvSpace2, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvSpace3, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnTextAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxAdvColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxSpace, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::BtnGfxAdvMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxLightColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxLightMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxPressColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::AnimGfxPressMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::MainWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KPSWndwMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Hint, nullptr, "Hint: Increase window size in \"Main window\" to see effect of this feature!"));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisHint, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines[ParameterLine::ID::KeyPressVisHint]->setCharacterSize(17u);

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisAdvModeColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisAdvModeSpace, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::KeyPressVisAdvModeMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::OtherMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, mCollectionNames.at(collectionNameIdx++)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::SaveStatColl, new ParameterLine(parP, mFonts, mTextures, mWindow)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::SaveStatMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));


    unsigned info = static_cast<unsigned>(ParameterLine::ID::InfoColl);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "                            Quick Guide"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Click with right mouse button on the key to edit it, then a menu"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "will pop out. It has 2 fields. First one means the logical key,"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "the second one is the visual one - how the key will be displayed."));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(info++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(ParameterLine::ID::InfoMty, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));


    unsigned hotKey = static_cast<unsigned>(ParameterLine::ID::HotKey1);
    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "[Hotkeys]"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + \"+/-\" - Add/remove keyboard keys"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + \"</>\" - Add/remove mouse buttons"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + W - Close the program"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + X - Reset all the statistics"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + A - Open graphical menu"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Ctrl + K - Open KPS extra window"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Alt (hold) - Show opposite buttons values"));
    mParameterLines.emplace(std::make_pair(static_cast<ParameterLine::ID>(hotKey++), new ParameterLine(parP, mFonts, mTextures, mWindow)));

    parP = sPtr(new LogicalParameter(LogicalParameter::Type::Collection, nullptr, "Program version: " + std::string(PROGRAM_VERSION)));
    mParameterLines.emplace(std::make_pair(ParameterLine::ID::ProgramVersion, new ParameterLine(parP, mFonts, mTextures, mWindow)));

    mParameterLines.emplace(std::make_pair(ParameterLine::ID::LastLine, new ParameterLine(emptyP, mFonts, mTextures, mWindow)));

    positionMenuLines();
}

void Menu::positionMenuLines()
{
    const auto step = sf::Vector2f(1000.f, 50.f);
    const auto halfWindowSize = 400.f;
    const auto padding = 5.f;
    const auto offset = sf::Vector2f(155.f, 0.f);
    auto row = 1u, column = 0u;
    auto tabIdx = 0u;

    for (auto &[id, line] : mParameterLines)
    {
        if (ParameterLine::isEmpty(id))
        {
            ++tabIdx;
            if (id == ParameterLine::ID::LastLine)
            {
                mParameterLines.at(ParameterLine::ID::ProgramVersion)
                    ->move(0.f, halfWindowSize - step.y * 3.f + padding);
            }
            mBounds.push_back(step.y * (row - 2u) - halfWindowSize + padding);
            row = 0u;
            ++column;
        }
        mTabParameters[tabIdx].push_back(id);

        if (!ParameterLine::isToSkip(id))
        {
            const auto position = sf::Vector2f(
                step.x * static_cast<float>(column) + offset.x, 
                step.y * static_cast<float>(row)    + offset.y - halfWindowSize + padding
            );

            line->setPosition(position);
            ++row;
        }
        else
        {
            // Hide them
            line->setPosition(-1000.f, -1000.f);
        }
    }
}

// The distance parameter contains offset of the view, 
// but the sliderbar is relative to the window,
// so it's neccessary to normilize both offset and cursor Y position,
// and then project the sum on the window
void Menu::moveSliderBarButtons(float offset)
{
    if (mBounds[mSelectedTab] < mView.getSize().y / 4.f)
        return;

    const sf::Vector2f sliderbarSize = mSliderBar.getSize();
    const sf::Vector2f sliberbarPosition = mSliderBar.getPosition();
    const sf::Vector2u windowSize = mWindow.getSize();
    const float highBounds = sliderbarSize.y / 2;
    const float lowBounds = mWindow.getSize().y - sliderbarSize.y / 2.f;

    const float normilizedOffset = offset / mBounds[mSelectedTab] / 1.5f;
    const float normilizedCursorPosition = sliberbarPosition.y / windowSize.y;
    float projectedSliderbarPositionY = windowSize.y * (normilizedCursorPosition + normilizedOffset);

    if (projectedSliderbarPositionY < highBounds)
        projectedSliderbarPositionY = highBounds;
    if (projectedSliderbarPositionY > lowBounds)
        projectedSliderbarPositionY = lowBounds;

    const float sliderbarX = sliberbarPosition.x;
    const float sliderbarY = projectedSliderbarPositionY;

    mSliderBar.setPosition(sliderbarX, sliderbarY);
    mView.setCenter(mView.getCenter().x, mView.getCenter().y + offset);
}

// Real window height - 600, sliderbar height - 200
// Virtual window should be 400, since the whole sliderbar must be in the window
// Quick scatch - https://i.imgur.com/xBXP6II.png
// Red - sliderbar, dark green - virtual window height, lime - real window 
// The sliderbar height has to be normilized and projected on the real window
void Menu::moveSliderBarMouse(sf::Vector2i mousePos)
{
    if (mBounds[mSelectedTab] < mView.getSize().y / 4)
        return;
        
    const sf::Vector2f sliderbarSize = mSliderBar.getSize();
    const float highBounds = sliderbarSize.y / 2;
    const float lowBounds = mWindow.getSize().y - sliderbarSize.y / 2;
    if (mousePos.y < highBounds)
        mousePos.y = highBounds;
    if (mousePos.y > lowBounds)
        mousePos.y = lowBounds;

    const float sliderbarX = mSliderBar.getPosition().x;
    const float sliderbarY = mousePos.y;
    const float virtualWindowHeight = mWindow.getSize().y - sliderbarSize.y;
    const float virtualCursorPositionY = sliderbarY - sliderbarSize.y / 2.f;
    const float normilizedViewHeight = virtualCursorPositionY / virtualWindowHeight;

    mSliderBar.setPosition(sliderbarX, sliderbarY);
    mView.setCenter(mView.getCenter().x, mBounds[mSelectedTab] * normilizedViewHeight);
}

void Menu::returnViewInBounds()
{
    const bool tooHigh = mView.getCenter().y < mHighViewBounds;
    const bool tooLow = mView.getCenter().y > mBounds[mSelectedTab];

    if (mBounds[mSelectedTab] < mView.getSize().y / 4)
    {
        mView.setCenter(mView.getCenter().x, 0);
        return;
    }

    if (tooHigh)
        mView.setCenter(mView.getCenter().x, mHighViewBounds);
    if (tooLow)
        mView.setCenter(mView.getCenter().x, mBounds[mSelectedTab]);
}

void Menu::saveConfig(const std::vector<std::unique_ptr<Button>> &mKeys)
{
    if (Settings::SaveStats)
        updateSaveStatsStrings();
    ConfigHelper::saveConfig(mParameters, mParameterLines, &mKeys, true);
}

void Menu::requestFocus()
{
    mWindow.requestFocus();
}

void Menu::updateSaveStatsStrings()
{
    auto it = mParameters.find(LogicalParameter::ID::SaveStatMaxKPS);
    it->second->setValStr(std::to_string(it->second->getDigit<float>()));
    ++it;

    while (it->first <= LogicalParameter::ID::SaveStatTotal15 && it != mParameters.end())
    {
        it->second->setValStr(std::to_string(it->second->getDigit<unsigned>()));
        ++it;
    }
}

Menu::KeyBlock::KeyBlock(ParameterLinesContainer &parameterLines, ParameterLine::ID placeHolder, size_t parametersNumber)
: mParameterLines(parameterLines)
, mFirstParameterLineId(placeHolder)
, mParametersNumber(parametersNumber)
, mSelectedKeyIdx(0u)
, mContainingTabIdx(0u)
{
}

GfxParameter &Menu::KeyBlock::current()
{
    assert(mSelectedKeyIdx < mContainer.size());
    return *mContainer.at(mSelectedKeyIdx);
}

void Menu::KeyBlock::push(Ptr ptr)
{
    mContainer.push_back(std::move(ptr));
}

void Menu::KeyBlock::select(unsigned idx)
{
    assert(idx < mContainer.size());

    const auto prevIdx = mSelectedKeyIdx;
    if (idx == prevIdx)
        return;

    auto increment = [] (ParameterLine::ID id, int val)
        {
            return static_cast<ParameterLine::ID>(static_cast<int>(id) + val);
        };
    auto lineToHide = increment(mFirstParameterLineId, static_cast<int>(prevIdx * mParametersNumber));
    auto lineToShow = increment(mFirstParameterLineId, static_cast<int>(idx * mParametersNumber));

    for (size_t i = 0ul; i < mParametersNumber; ++i)
    {
        auto toHideFound = mParameterLines.find(lineToHide);
        auto toShowFound = mParameterLines.find(lineToShow);
        
        assert(toHideFound != mParameterLines.end());
        assert(toShowFound != mParameterLines.end());

        toShowFound->second->setPosition(toHideFound->second->getPosition());
        toHideFound->second->setPosition(sf::Vector2f(-1000.f, -1000.f));

        lineToHide = increment(lineToHide, 1);
        lineToShow = increment(lineToShow, 1);
    }
}

void Menu::KeyBlock::setContainingTab(unsigned idx)
{
    mContainingTabIdx = idx;
}

unsigned Menu::KeyBlock::getContainingTab() const
{
    return mContainingTabIdx;
}

void Menu::KeyBlock::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    for (const auto &elem : mContainer)
    {
        target.draw(*elem, states);
    }
}

void Menu::KeyBlock::handleEvent(sf::Event event, sf::Vector2f absCursorPos)
{
    auto idx = 0u;
    for (auto &elem : mContainer)
    {
        auto color = GfxParameter::defaultRectColor;
        const auto size = elem->mRect.getSize();
        const auto pos = getPosition() + elem->getPosition() - size / 2.f;;
        const auto rect = sf::FloatRect(pos, size);

        if (rect.contains(absCursorPos))
        {
            color = GfxParameter::defaultAimedRectColor;
            if (event.type == sf::Event::MouseButtonPressed
            &&  event.mouseButton.button == sf::Mouse::Left)
            {
                // Deselect old tab
                current().mRect.setFillColor(GfxParameter::defaultRectColor);

                // Select new tab
                select(idx);
                mSelectedKeyIdx = idx;
            }
        }
        
        if (mSelectedKeyIdx == idx)
            color = GfxParameter::defaultSelectedRectColor;

        elem->mRect.setFillColor(color);
        ++idx;
    }
}
