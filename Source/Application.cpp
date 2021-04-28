#include "../Headers/Application.hpp"

const sf::Time Application::TimePerFrame = sf::seconds(1.f/60);

Application::Application(Settings& settings)
: mWindow(sf::VideoMode(getWidth(), getHeight()), "KPS", sf::Style::None)
, mTextures()
, mFonts()
, mSettings(settings)
, mCalculation()
, mKeyPressingManager()
, mStatistics(mWindow)
, mButtons(mWindow)
, mBackground(mWindow)
, mMoveWindow(false)
{
	mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(60);
    sf::Image icon;
    icon.loadFromMemory(iconHeaderArray, 28000);
    mWindow.setIcon(256, 256, icon.getPixelsPtr());


    if (Settings::ButtonTexturePath == "Default")
        mTextures.loadFromMemory(Textures::KeyButton, Settings::DefaultButtonTexture, 148000);
    else
        mTextures.loadFromFile(Textures::KeyButton, Settings::ButtonTexturePath);

    if (Settings::BackgroundTexturePath == "Default")
        mTextures.loadFromMemory(Textures::Background, Settings::DefaultBackgroundTexture, 72000);
    else
        mTextures.loadFromFile(Textures::Background, Settings::BackgroundTexturePath);
    
    
    if (Settings::KeyCountersFontPath == "Default")
        mFonts.loadFromMemory(Fonts::KeyCounters, Settings::KeyCountersDefaultFont, 1800000);
    else
        mFonts.loadFromFile(Fonts::KeyCounters, Settings::KeyCountersFontPath);

    if (Settings::StatisticsFontPath == "Default")
        mFonts.loadFromMemory(Fonts::Statistics, Settings::StatisticsDefaultFont, 1800000);
    else
        mFonts.loadFromFile(Fonts::Statistics, Settings::StatisticsFontPath);


    mStatistics.loadFonts(mFonts);
    mStatistics.setFonts();
    mButtons.loadTextures(mTextures);

    mButtons.setupTexture();
    mButtons.setupAnimation();

    mSettings.setWindowReference(mWindow);
    mSettings.setChangeabilityPosition();

    mBackground.loadTextures(mTextures);
    mBackground.scaleTexture();
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
    sf::Event event;
    mKeyPressingManager.readClickedKeys();
    while (mWindow.pollEvent(event))
    {
        if (mWindow.hasFocus())
        {
            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                &&  sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
                {
                    mSettings.changeChangeability();
                    // Break to avoid problems if the user has Q as key
                    break;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                &&  sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                {
                    mSettings.saveSettings();
                    mWindow.close();
                    return;
                }
            }

            if (Settings::IsChangeable)
            {
                mSettings.handleEvent(event);
                mStatistics.handleHighlight(mSettings.getButtonToChangeIndex());

                if (mSettings.wasButtonAmountChanged())
                {
                    handleEvent(event);
                    mSettings.setChangeabilityPosition();
                    mStatistics.handleEvent(event);
                    mButtons.handleEvent(event);
                    mKeyPressingManager.handleEvent(event);
                    mBackground.handleEvent(event);
                }
            }
        }
    }

    moveWindow();
    
    if (!Settings::IsChangeable)
    {
        mCalculation.handleInput(mKeyPressingManager, Settings::Keys);
        mStatistics.handleInput(mKeyPressingManager);
        mButtons.handleInput(mKeyPressingManager.mNeedToBeReleased);
    }
}

void Application::update(sf::Time dt)
{
    mCalculation.update();
    mStatistics.update( mCalculation.getKeyPerSecond()
                    , mCalculation.getBeatsPerMinute()
                    , mKeyPressingManager.mClickedKeys );
    mButtons.update(mKeyPressingManager.mNeedToBeReleased);
    mKeyPressingManager.clear();
    mSettings.update();
}

void Application::render()
{
    mWindow.clear();
    mBackground.draw();
    mButtons.draw();
    mStatistics.draw();
    mSettings.draw();
    mWindow.display();
}

void Application::handleEvent(sf::Event event)
{
    mWindow.setSize(sf::Vector2u(getWidth(), getHeight()));

    mWindow.setView(sf::View(sf::FloatRect(0, 0, mWindow.getSize().x,
                                                    mWindow.getSize().y)));
}

void Application::moveWindow()
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
    &&  mWindow.hasFocus())
    {
        mWindow.setPosition(mWindow.getPosition()
                            + sf::Mouse::getPosition()
                            - mLastMousePosition);
    }
    mLastMousePosition = sf::Mouse::getPosition();
}

unsigned int Application::getWidth()
{
    return    Settings::Distance * Settings::ButtonAmount
            + Settings::ButtonTextureSize.x * Settings::ButtonAmount
            + Settings::SpaceBetweenButtonsAndStatistics
            + (Settings::StatisticsTextCharacterSize * 9) * Settings::ShowStatisticsText;
            // 9 is value by eye
}

unsigned int Application::getHeight()
{
    return    Settings::Distance * 2
            + Settings::ButtonTextureSize.y;
}
