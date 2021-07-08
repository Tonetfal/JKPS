#include "../Headers/KPSWindow.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Window/Event.hpp>

static unsigned maxLen = 4U;

KPSWindow::KPSWindow()
{
    mKPSText.setCharacterSize(Settings::KPSTextSize);
    mKPSText.setString("KPS");
    mKPSNumber.setCharacterSize(Settings::KPSNumberSize);
    mKPSNumber.setString("0");
    if (Settings::KPSWindowEnabledFromStart)
    {
        mWindow.create(sf::VideoMode(Settings::KPSWindowSize.x, 
                Settings::KPSWindowSize.y), "KPS Window", sf::Style::Close);
    }

    mDefaultWidthCenters = new float(maxLen);
}

void KPSWindow::handleEvent(sf::Event event)
{
    if (event.type == sf::Event::KeyPressed
    &&  event.key.code == Settings::KeyToOpenKPSWindow
    &&  sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
    {
        if (!mWindow.isOpen())
        {
            mWindow.create(sf::VideoMode(Settings::KPSWindowSize.x, 
                Settings::KPSWindowSize.y), "KPS Window", sf::Style::Close);
        }
        else
        {
            mWindow.close();
        }
    }
}

void KPSWindow::handleOwnEvent()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    { 
        if (event.type == sf::Event::Closed
        || (event.type == sf::Event::KeyPressed
        && event.key.code == Settings::KeyExit
        && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)))
        {
            mWindow.close();
        }
    }
}

void KPSWindow::update(std::size_t kps)
{
    if (mWindow.isOpen())
    {
        mKPSNumber.setString(std::to_string(kps));
        mKPSNumber.setOrigin((mKPSNumber.getLocalBounds().left + 
            mKPSNumber.getLocalBounds().width) / 2.f, mKPSNumber.getLocalBounds().top);
    }
}

void KPSWindow::draw()
{
    if (mWindow.isOpen())
    {
        sf::Transform textTransform = sf::Transform::Identity;
        sf::Transform numberTranform = sf::Transform::Identity;

        textTransform.translate((mWindow.getSize().x - mKPSText.getLocalBounds().width + 
            mKPSText.getLocalBounds().left) / 2.f, Settings::KPSWindowDistanceTop);

        numberTranform.translate(mWindow.getSize().x / 2.f - mKPSNumber.getLocalBounds().left, 
            mKPSText.getLocalBounds().height + Settings::KPSWindowDistanceBetween + Settings::KPSWindowDistanceTop);

        mWindow.clear(Settings::KPSWindowColor);
        mWindow.draw(mKPSText, textTransform);
        mWindow.draw(mKPSNumber, numberTranform);
        mWindow.display();
    }
}

void KPSWindow::loadFont(const sf::Font &font)
{
    mKPSText.setFont(font);
    mKPSNumber.setFont(font);
    mKPSText.setOrigin(mKPSText.getLocalBounds().left, mKPSText.getLocalBounds().top);
    mKPSNumber.setOrigin((mKPSNumber.getLocalBounds().left + mKPSNumber.getLocalBounds().width) / 2.f, 
        mKPSNumber.getLocalBounds().top);
}