#include "../Headers/Application.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/LogicalParameter.hpp"
#include "../Headers/ChangedParametersQueue.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/DefaultFiles.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/ConfigHelper.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Transformable.hpp>


const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60);

Application::Application()
{
    loadTextures();
    loadFonts();

    buildButtons();
    buildStatistics();

    openWindow();
    loadIcon();

    std::unique_ptr<GfxButtonSelector> keySelector(new GfxButtonSelector);
    mGfxButtonSelector = std::move(keySelector);

    std::unique_ptr<ButtonPositioner> buttonPositioner(new ButtonPositioner(&mButtons));
    mButtonsPositioner = std::move(buttonPositioner);
    (*mButtonsPositioner)();

    std::unique_ptr<StatisticsPositioner> statPositioner(new StatisticsPositioner(&mStatistics));
    mStatisticsPositioner = std::move(statPositioner);
    (*mStatisticsPositioner)();

    std::unique_ptr<Background> bg(new Background(mTextures, mWindow));
    mBackground = std::move(bg);

    std::unique_ptr<KPSWindow> kpsWindow(new KPSWindow(mFonts));
    mKPSWindow = std::move(kpsWindow);

    mMenu.saveConfig(mButtons);
}

void Application::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (mWindow.isOpen())
    {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;

            processInput();
            update();
        }

        render();
    }
}

void Application::processInput()
{
    // Open/close other windows, add/rm keys
    handleEvent();

    // Update changed parameters
    if (mMenu.isOpen())
        unloadChangesQueue();

    // Update assets if there is a request
    if (ParameterLine::resetRefreshState())
        resetAssets();
    
    // Take buttons realtime input
    for (auto &button : mButtons)
        button->processInput();

    if (!Settings::WindowTitleBar)
        moveWindow();

    // Make separate windows handle own events
    if (mMenu.isOpen())
        mMenu.processInput();
    if (mGfxButtonSelector->isOpen())
        mGfxButtonSelector->handleOwnInput();
    if (mKPSWindow->isOpen())
        mKPSWindow->handleOwnEvent();
}

void Application::handleEvent()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::MouseButtonPressed)
        {
            const sf::Mouse::Button button = event.mouseButton.button;
            if (button == sf::Mouse::Right)
            {
                unsigned idx;
                if (isPressPerformedOnButton(idx))
                {
                    mGfxButtonSelector->setKey(mButtons[idx]->getLogKey());
                    mGfxButtonSelector->openWindow(mWindow.getPosition());
                }
            }
        }
        
        if (event.type == sf::Event::KeyPressed)
        {
            const sf::Keyboard::Key key = event.key.code;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {
                bool btnAmtChanged = false;
                if (key == Settings::KeyToIncreaseKeys || key == Settings::AltKeyToIncreaseKeys)
                {
                    addButton(*new LogKey("A", "A", new sf::Keyboard::Key(sf::Keyboard::A), nullptr));
                    btnAmtChanged = true;
                }

                if (key == Settings::KeyToIncreaseButtons)
                {
                    addButton(*new LogKey("M Left", "M Left", nullptr, new sf::Mouse::Button(sf::Mouse::Left)));
                    btnAmtChanged = true;
                }

                if (key == Settings::KeyToDecreaseKeys || key == Settings::AltKeyToDecreaseKeys || key == Settings::KeyToDecreaseButtons)
                {
                    removeButton();
                    btnAmtChanged = true;
                }

                if (btnAmtChanged)
                {
                    (*mButtonsPositioner)();
                    resizeWindow();
                    mBackground->rescale();
                }

                if (key == Settings::KeyToOpenKPSWindow)
                {
                    if (mKPSWindow->isOpen())
                        mKPSWindow->closeWindow();
                    else
                        mKPSWindow->openWindow();
                }

                if (key == Settings::KeyToOpenMenuWindow)
                {
                    if (mMenu.isOpen())
                        mMenu.closeWindow();
                    else
                        mMenu.openWindow();
                }

                if (key == Settings::KeyToReset)
                {
                    for (auto &button : mButtons)
                        button->reset();
                }

                if (key == Settings::KeyExit)
                {
                    mMenu.saveConfig(mButtons);
                    mWindow.close();
                    return;
                }
            }
        }

        if (event.type == sf::Event::Closed)
        {
            mMenu.saveConfig(mButtons);
            mWindow.close();
            return;
        }
    }
}

void Application::update()
{
    for (auto &button : mButtons)
        button->update();
    for (auto &line : mStatistics)
        line->update();

    if (mMenu.isOpen())
        mMenu.update();
    
    if (mKPSWindow->isOpen())
        mKPSWindow->update();

    Button::movePointer();
}

void Application::render()
{
    mWindow.clear(sf::Color(45,45,45));

    mWindow.draw(*mBackground);

    for (const auto &elem : mButtons)
        mWindow.draw(*elem);
    for (const auto &elem : mStatistics)
        mWindow.draw(*elem);

    if (mMenu.isOpen())
        mMenu.render();
    if (mGfxButtonSelector->isOpen())
        mGfxButtonSelector->render();
    if (mKPSWindow->isOpen())
        mKPSWindow->render();
    
    mWindow.display();
}

void Application::unloadChangesQueue()
{
    ChangedParametersQueue &queue = mMenu.getChangedParametersQueue();
    while (!queue.isEmpty())
    {
        std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> pair = queue.pop();

        if (GfxStatisticsLine::parameterIdMatches(pair.first))
        {
            for (auto &line : mStatistics)
                line->updateParameters();
            (*mStatisticsPositioner)();
        }

        if (Button::parameterIdMatches(pair.first))
        {
            unsigned idx = 0;
            for (auto &button : mButtons)
            {
                button->updateParameters();
                ++idx;
            }
            (*mButtonsPositioner)();
        }

        if (KPSWindow::parameterIdMatches(pair.first))
        {
            mKPSWindow->updateParameters();
        }

        if (parameterIdMatches(pair.first))
        {
            mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));
            mWindow.setView(sf::View(sf::FloatRect(0, 0, mWindow.getSize().x, mWindow.getSize().y)));
            mMenu.requestFocus();
        }

        if (pair.first == LogicalParameter::ID::MainWndwTitleBar)
        {
            openWindow();
        }

        mBackground->rescale();
    }
}

void Application::resetAssets()
{
    mFonts.clear();
    mTextures.clear();

    loadTextures();
    loadFonts();

    unsigned idx = 0;
    for (auto &button : mButtons)
    {
        button->updateAssets();
        button->setPosition(Button::getWidth(idx), Button::getHeight(idx));
        ++idx;
    }

    for (auto &line : mStatistics)
        line->updateAsset();

    mBackground->updateAssets();

    mMenu.saveConfig(mButtons);
}

void Application::loadTextures()
{
    if (!mTextures.loadFromFile(Textures::Button, Settings::GfxButtonTexturePath))
        mTextures.loadFromMemory(Textures::Button, Settings::DefaultButtonTexture, 58700);

    if (!mTextures.loadFromFile(Textures::Animation, Settings::AnimationTexturePath))
        mTextures.loadFromMemory(Textures::Animation, Settings::DefaultAnimationTexture, 60100);


    Settings::isGreenscreenSet = Settings::BackgroundTexturePath == "GreenscreenBG.png";
    if (Settings::isGreenscreenSet)
        mTextures.loadFromMemory(Textures::Background, Settings::DefaultGreenscreenBackgroundTexture, 596);
    else
    {
        if (!mTextures.loadFromFile(Textures::Background, Settings::BackgroundTexturePath))
            mTextures.loadFromMemory(Textures::Background, Settings::DefaultBackgroundTexture, 2700);
    }
}

void Application::loadFonts()
{
    if (!mFonts.loadFromFile(Fonts::ButtonValue, Settings::ButtonTextFontPath))
        mFonts.loadFromMemory(Fonts::ButtonValue, Settings::KeyCountersDefaultFont, 446100);

    if (!mFonts.loadFromFile(Fonts::Statistics, Settings::StatisticsTextFontPath))
        mFonts.loadFromMemory(Fonts::Statistics, Settings::StatisticsDefaultFont, 446100);

    if (!mFonts.loadFromFile(Fonts::KPSText, Settings::KPSWindowTextFontPath))
        mFonts.loadFromMemory(Fonts::KPSText, Settings::DefaultKPSWindowFont, 459300);

    if (!mFonts.loadFromFile(Fonts::KPSNumber, Settings::KPSWindowNumberFontPath))
        mFonts.loadFromMemory(Fonts::KPSNumber, Settings::DefaultKPSWindowFont, 459300);
}

void Application::loadIcon()
{
    sf::Image icon;
    icon.loadFromMemory(IconTexture, 53200);
    mWindow.setIcon(256, 256, icon.getPixelsPtr());
}

void Application::buildStatistics()
{
    typedef std::unique_ptr<GfxStatisticsLine> Ptr;
    Ptr linePtr(nullptr);
    unsigned id = GfxStatisticsLine::StatisticsID::KPS;
    
    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsKPS, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;

    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsTotal, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;

    linePtr = Ptr(new GfxStatisticsLine(mFonts, Settings::ShowStatisticsBPM, static_cast<GfxStatisticsLine::StatisticsID>(id)));
    mStatistics[id] = std::move(linePtr);
    ++id;
}

void Application::buildButtons()
{
    std::queue<LogKey> logKeyQueue = ConfigHelper::getLogKeys();
    std::queue<LogKey> logKeyBtnsQueue = ConfigHelper::getLogButtons();
    while (logKeyBtnsQueue.size())
    {
        logKeyQueue.push(logKeyBtnsQueue.front());
        logKeyBtnsQueue.pop();
    }

    for (unsigned i = 0; logKeyQueue.size(); ++i)
    {
        addButton(logKeyQueue.front());
        logKeyQueue.pop();
    }
}

bool Application::isPressPerformedOnButton(unsigned &btnIdx) const
{
    const unsigned size = Button::size();
    for (unsigned i = 0; i < size; ++i)
    {
        if (isMouseInRange(i))
        {
            btnIdx = i;
            return true;
        }
    }
    return false;
}

bool Application::isMouseInRange(unsigned idx) const
{
    const sf::Vector2i mousePosition(sf::Mouse::getPosition(mWindow));
    const sf::Vector2f textureSize = static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize);
    const Button &button = *mButtons[idx];
    const sf::Vector2f buttonPosition = button.getPosition() - textureSize / 2.f;
    const sf::FloatRect buttonRectangle(buttonPosition, textureSize);

    return buttonRectangle.contains(static_cast<sf::Vector2f>(mousePosition));
}

void Application::addButton(LogKey &logKey)
{
    std::unique_ptr<Button> buttonPtr(new Button(logKey, mTextures, mFonts));
    mButtons.push_back(std::move(buttonPtr));
}

void Application::removeButton()
{
    mButtons.pop_back();
}

void Application::openWindow()
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

    mWindow.create(sf::VideoMode(getWindowWidth(), getWindowHeight()), 
        "JKPS", Settings::WindowTitleBar ? sf::Style::Default : sf::Style::None);
    mWindow.setPosition(sf::Vector2i(
        desktop.width / 2  - mWindow.getSize().x / 2, 
        desktop.height / 2 - mWindow.getSize().y / 2));
    mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(60);
}

void Application::resizeWindow()
{
    mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));

    sf::Vector2f windowSize(mWindow.getSize());
    sf::View view(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    mWindow.setView(view);
}

void Application::moveWindow()
{
    static sf::Vector2i mLastMousePosition;
    if (mWindow.hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        mWindow.setPosition(mWindow.getPosition() + 
            sf::Mouse::getPosition() - mLastMousePosition);
    }
    mLastMousePosition = sf::Mouse::getPosition();
}

unsigned Application::getWindowWidth()
{
    const unsigned width =  
        Settings::GfxButtonTextureSize.x * Button::size() + 
        (Button::size() - 1) * Settings::GfxButtonDistance + 
        Settings::WindowBonusSizeLeft + Settings::WindowBonusSizeRight;
    
    return width > 0 ? width : 100; 
}

unsigned Application::getWindowHeight()
{
    const unsigned height = 
        Settings::GfxButtonTextureSize.y + Settings::WindowBonusSizeTop + 
        Settings::WindowBonusSizeBottom;
    
    return height > 0 ? height : 100;
}

bool Application::parameterIdMatches(LogicalParameter::ID id)
{
    return
        id == LogicalParameter::ID::BtnGfxTxtrSz ||  
        id == LogicalParameter::ID::BtnTextChSz ||
        id == LogicalParameter::ID::BtnGfxDist  ||
        id == LogicalParameter::ID::MainWndwTop ||
        id == LogicalParameter::ID::MainWndwBot ||
        id == LogicalParameter::ID::MainWndwLft ||
        id == LogicalParameter::ID::MainWndwRght;
}
