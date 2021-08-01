#include "../Headers/Application.hpp"
#include "../Headers/Menu.hpp"
#include "../Headers/LogicalParameter.hpp"
#include "../Headers/ChangedParametersQueue.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/DefaultFiles.hpp"
#include "../Headers/Menu.hpp"


const sf::Time Application::TimePerFrame = sf::seconds(1.f / Settings::mFramesPerSecond);

Application::Application(Menu &menu)
: mMenu(menu)
, mSettings(mMenu.getSettings())
{
    openWindow();
    loadAssets();

    std::unique_ptr<Button> buttons(new Button(mTextures, mFonts));
    mButtons = std::move(buttons);

    std::unique_ptr<Statistics> stats(new Statistics(mFonts));
    mStatistics = std::move(stats);

    std::unique_ptr<Background> bg(new Background(mTextures, mWindow));
    mBackground = std::move(bg);

    std::unique_ptr<KPSWindow> kpsWindow(new KPSWindow(mFonts));
    mKPSWindow = std::move(kpsWindow);

    mSettings.setWindowReference(mWindow);
    mSettings.setButtonsReference(*mButtons);
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
            update(TimePerFrame);
        }

        render();
    }
}

void Application::processInput()
{
    mKeyPressingManager.readClickedKeys();
    handleEvent();

    if (!Settings::WindowTitleBar)
        moveWindow();

    if (mSettings.wasButtonAmountChanged())
    {
        resizeWindow();
        mStatistics->resize();
        mButtons->resize();
        mKeyPressingManager.resize();
        mBackground->resize();
        mMenu.saveConfig();
    }

    if (mSettings.getButtonToChangeIndex() != -1)
        mButtons->highlightKey(mSettings.getButtonToChangeIndex());
    if (mSettings.wasButtonChanged())
    {
        mButtons->highlightKey(mSettings.getButtonToChangeIndex());
        mButtons->setupKeyCounterTextVec();
        mMenu.saveConfig();
    }

    if (mSettings.resetReloadAssetsRequest())
    {
        mFonts.clear();
        mTextures.clear();
        loadAssets();
        mButtons->setupAssets(true);
        mStatistics->setupText();
        mBackground->setupTexture();
        mMenu.saveConfig();
    }
    unloadChangesQueue();
    mKPSWindow->handleOwnEvent();
    mMenu.handleOwnEvent();
    mCalculation.handleInput(mKeyPressingManager);
    mButtons->handleInput(mKeyPressingManager.mNeedToBeReleased, mKeyPressingManager);
}

void Application::handleEvent()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            mMenu.saveConfig();
            mWindow.close();
            return;
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == Settings::KeyToClear)
                {
                    mButtons->clear();
                    mStatistics->clear();
                    mCalculation.clear();
                    mKeyPressingManager.clear();
                }

                if (event.key.code == Settings::KeyExit)
                {
                    mMenu.saveConfig();
                    mWindow.close();
                    return;
                }
            }
        }

        mMenu.handleEvent(event);
        mKPSWindow->handleEvent(event);
        mSettings.handleEvent(event);
    }
}

void Application::update(sf::Time dt)
{
    mCalculation.update();
    mStatistics->update(mCalculation.getKeyPerSecond(), mCalculation.getBeatsPerMinute(), mKeyPressingManager.mClickedKeys);
    mButtons->update(mKeyPressingManager.mNeedToBeReleased);
    mMenu.update();
    mKPSWindow->update(mCalculation.getKeyPerSecond());
    mKeyPressingManager.clear();
    mSettings.update();
}

void Application::render()
{
    mWindow.clear();

    mWindow.draw(*mBackground);
    mWindow.draw(*mStatistics);
    mWindow.draw(*mButtons);
    mMenu.render();
    mKPSWindow->render();
    
    mWindow.display();
}

void Application::unloadChangesQueue()
{
    ChangedParametersQueue &queue = mMenu.getChangedParametersQueue();
    while (!queue.isEmpty())
    {
        std::pair<const LogicalParameter::ID, std::shared_ptr<LogicalParameter>> pair = queue.pop();
        if (Statistics::parameterIdMatches(pair.first))
        {
            mStatistics->setupText();
        }
        if (Button::parameterIdMatches(pair.first))
        {
            mButtons->setupAssets(false);
            mButtons->setupKeyCounterTextVec();
            mButtons->highlightKey(mSettings.getButtonToChangeIndex());
        }
        if (KPSWindow::parameterIdMatches(pair.first))
        {
            mKPSWindow->setupText();
        }
        if (pair.first == LogicalParameter::ID::MainWndwTitleBar)
        {
            openWindow();
        }
        if (Application::parameterIdMatches(pair.first))
        {
            mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));
            mWindow.setView(sf::View(sf::FloatRect(0, 0, mWindow.getSize().x, mWindow.getSize().y)));
            mMenu.requestFocus();
        }
        mBackground->setupTexture();
    }
}

void Application::loadAssets()
{
    if (!mTextures.loadFromFile(Textures::KeyButton, Settings::ButtonTexturePath))
        mTextures.loadFromMemory(Textures::KeyButton, Settings::DefaultButtonTexture, 58700);

    if (!mTextures.loadFromFile(Textures::ButtonAnimation, Settings::AnimationTexturePath))
        mTextures.loadFromMemory(Textures::ButtonAnimation, Settings::DefaultAnimationTexture, 60100);


    Settings::isGreenscreenSet = Settings::BackgroundTexturePath == "GreenscreenBG.png";
    if (Settings::isGreenscreenSet)
        mTextures.loadFromMemory(Textures::Background, Settings::DefaultGreenscreenBackgroundTexture, 596);
    else
    {
        if (!mTextures.loadFromFile(Textures::Background, Settings::BackgroundTexturePath))
            mTextures.loadFromMemory(Textures::Background, Settings::DefaultBackgroundTexture, 2700);
    }
    
    
    if (!mFonts.loadFromFile(Fonts::KeyCounters, Settings::KeyCountersFontPath))
        mFonts.loadFromMemory(Fonts::KeyCounters, Settings::KeyCountersDefaultFont, 446100);

    if (!mFonts.loadFromFile(Fonts::Statistics, Settings::StatisticsFontPath))
        mFonts.loadFromMemory(Fonts::Statistics, Settings::StatisticsDefaultFont, 446100);

    if (!mFonts.loadFromFile(Fonts::KPSText, Settings::KPSWindowTextFontPath))
        mFonts.loadFromMemory(Fonts::KPSText, Settings::DefaultKPSWindowFont, 459300);

    if (!mFonts.loadFromFile(Fonts::KPSNumber, Settings::KPSWindowNumberFontPath))
        mFonts.loadFromMemory(Fonts::KPSNumber, Settings::DefaultKPSWindowFont, 459300);

    sf::Image icon;
    icon.loadFromMemory(IconTexture, 53200);
    mWindow.setIcon(256, 256, icon.getPixelsPtr());
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
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
    &&  mWindow.hasFocus())
    {
        mWindow.setPosition(mWindow.getPosition() + 
            sf::Mouse::getPosition() - mLastMousePosition);
    }
    mLastMousePosition = sf::Mouse::getPosition();
}

unsigned Application::getWindowWidth()
{
    unsigned width =  
        Settings::ButtonTextureSize.x * Settings::ButtonAmount + 
        (int(Settings::ButtonAmount) - 1) * Settings::ButtonDistance + 
        Settings::WindowBonusSizeLeft + Settings::WindowBonusSizeRight;
    
    return width > 0 ? width : 1; 
}

unsigned Application::getWindowHeight()
{
    unsigned height = 
        Settings::ButtonTextureSize.y + Settings::WindowBonusSizeTop + 
        Settings::WindowBonusSizeBottom;
    
    return height > 0 ? height : 1;
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
