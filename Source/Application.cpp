#include "../Headers/Application.hpp"
#include "../Headers/Settings.hpp"


const sf::Time Application::TimePerFrame = sf::seconds(1.f / 60);

Application::Application()
: mWindow (sf::VideoMode(getWindowWidth(), getWindowHeight()), "KPS", sf::Style::None)
, mGraphics(mWindow)
, mUser()
{
    mWindow.setKeyRepeatEnabled(false);
}

void Application::run() 
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen()) 
    {
        timeSinceLastUpdate += clock.restart();
        while (timeSinceLastUpdate >= TimePerFrame) 
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
	CommandQueue& commands = mGraphics.getCommandQueue();

    sf::Event event;
    while (mWindow.pollEvent(event)) 
    {
        // Fill up the queue
        mUser.handleEvent(event, commands);
        
        if (event.type == sf::Event::Closed
        || KeyCombination::isCombinationPressed(Settings::CombinationToExit))
        {
            // save cfg
            mWindow.close();
        }

    }

    // Empty the queue
    mUser.handleRealtimeInput(commands);
    moveWindow(sf::Mouse::getPosition());
}

void Application::update(sf::Time elapsedTime) 
{
    mGraphics.update(elapsedTime);
}

void Application::render() 
{
    mWindow.clear();
    mGraphics.draw();
    mWindow.display();
}

void Application::moveWindow(sf::Vector2i mousePosition)
{
    static sf::Vector2i lastMousePosition;
    sf::Vector2i deltaMovement = lastMousePosition - mousePosition;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&  mWindow.hasFocus())
        mWindow.setPosition(mWindow.getPosition() - deltaMovement);

    lastMousePosition = mousePosition;
}

unsigned int Application::getWindowWidth() const
{
    return (Settings::ButtonTextureSize.x + Settings::ButtonDistance) * Settings::ButtonAmount 
        + Settings::ButtonDistance + Settings::SpaceBetweenButtonsAndStatistics + Settings::SpaceOnStatisticsRight;
}

unsigned int Application::getWindowHeight() const
{
    return Settings::ButtonTextureSize.y + Settings::ButtonDistance * 2;
}
