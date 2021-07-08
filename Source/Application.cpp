#include "../Headers/Application.hpp"

const sf::Time Application::TimePerFrame = sf::seconds(1.f/Settings::mFramesPerSecond);

Application::Application(Settings& settings)
: mWindow(sf::VideoMode(getWindowWidth(), 150), "JKPS", sf::Style::None)
, mTextures()
, mFonts()
, mSettings(settings)
, mCalculation()
, mKeyPressingManager()
, mStatistics(mWindow)
, mButtons(mWindow)
, mBackground(mWindow)
{
	mWindow.setKeyRepeatEnabled(false);
    mWindow.setFramerateLimit(60);
    sf::Image icon;
    icon.loadFromMemory(iconHeaderArray, 53200);
    mWindow.setIcon(256, 256, icon.getPixelsPtr());

    loadAssets();

    mStatistics.loadFonts(mFonts);
    mButtons.loadTextures(mTextures);

    std::cout << getWindowHeight() << "\n";
    mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));
    mWindow.setView(sf::View(sf::FloatRect(0, 0, mWindow.getSize().x, mWindow.getSize().y)));

    mButtons.loadFonts(mFonts);
    mButtons.setupTextures();

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
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            {
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == Settings::KeyForEditMode)
                    {
                        mSettings.changeChangeability();
                        // Break to avoid problems if the user has Q as key
                        break;
                    }

                    if (event.key.code == Settings::KeyToClear)
                    {
                        mButtons.clear();
                        mStatistics.clear();
                        mCalculation.clear();
                        mKeyPressingManager.clear();
                    }

                    if (event.key.code == Settings::KeyExit)
                    {
                        mSettings.saveSettings();
                        mWindow.close();
                        return;
                    }
                }
            }

            if (event.type == sf::Event::Closed)
            {
                mSettings.saveSettings();
                mWindow.close();
                return;
            }

            if (Settings::IsChangeable)
            {
                mSettings.handleEvent(event);
                mButtons.handleHighlight(mSettings.getButtonToChangeIndex());

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
        mButtons.handleInput(mKeyPressingManager.mNeedToBeReleased, 
            mKeyPressingManager);
    }
}

void Application::update(sf::Time dt)
{
    mCalculation.update();
    mStatistics.update( mCalculation.getKeyPerSecond(), 
        mCalculation.getBeatsPerMinute(), mKeyPressingManager.mClickedKeys );
    mButtons.update(mKeyPressingManager.mNeedToBeReleased);
    mKeyPressingManager.clear();
    mSettings.update();
}

void Application::render()
{
    mWindow.clear();

    mBackground.draw();
    mStatistics.draw();
    mButtons.draw();
    mSettings.draw();
    
    mWindow.display();
}

void Application::handleEvent(sf::Event event)
{
    mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));

    sf::Vector2f windowSize(mWindow.getSize());
    sf::View view(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
    mWindow.setView(view);
}

void Application::loadAssets()
{
    std::string defaultName = "Default";

    if (Settings::ButtonTexturePath == defaultName)
        mTextures.loadFromMemory(Textures::KeyButton, Settings::DefaultButtonTexture, 7400);
    else
        mTextures.loadFromFile(Textures::KeyButton, Settings::ButtonTexturePath);
    if (Settings::AnimationTexturePath == defaultName) 
        mTextures.loadFromMemory(Textures::ButtonAnimation, Settings::DefaultAnimationTexture, 65600);
    else
        mTextures.loadFromFile(Textures::ButtonAnimation, Settings::AnimationTexturePath);

    if (Settings::BackgroundTexturePath == defaultName)
        mTextures.loadFromMemory(Textures::Background, Settings::DefaultBackgroundTexture, 14600);
    else
        mTextures.loadFromFile(Textures::Background, Settings::BackgroundTexturePath);
    
    
    if (Settings::KeyCountersFontPath == defaultName)
        mFonts.loadFromMemory(Fonts::KeyCounters, Settings::KeyCountersDefaultFont, 446100);
    else
        mFonts.loadFromFile(Fonts::KeyCounters, Settings::KeyCountersFontPath);

    if (Settings::StatisticsFontPath == defaultName)
        mFonts.loadFromMemory(Fonts::Statistics, Settings::StatisticsDefaultFont, 446100);
    else
        mFonts.loadFromFile(Fonts::Statistics, Settings::StatisticsFontPath);
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

unsigned int Application::getWindowWidth()
{
    return (Settings::ButtonDistance + Settings::ButtonTextureSize.x) * 
        Settings::ButtonAmount + Settings::SpaceBetweenButtonsAndStatistics + 
        Settings::SpaceBetweenButtonsAndStatistics + Settings::SpaceOnStatisticsRight + 
        Settings::WindowBonusSizeLeft + Settings::WindowBonusSizeRight + 1;
}

unsigned int Application::getWindowHeight()
{
    return Settings::ButtonDistance * 2 + Settings::ButtonTextureSize.y +
        Settings::WindowBonusSizeTop + Settings::WindowBonusSizeBottom;
    // return std::max(float(mStatistics.getTotalStatisticsHeight() + Settings::ButtonDistance * 2),
    //    Settings::ButtonDistance * 2 + Settings::ButtonTextureSize.y);
}
