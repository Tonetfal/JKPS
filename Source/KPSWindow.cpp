#include "../Headers/KPSWindow.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/ResourceHolder.hpp"

#include <SFML/Window/Event.hpp>


static unsigned maxLen = 4U;

KPSWindow::KPSWindow(const FontHolder &fonts)
: mTextFont(&fonts.get(Fonts::KPSText))
, mNumberFont(&fonts.get(Fonts::KPSNumber))
{
    mKPSText.setFont(*mTextFont);
    mKPSNumber.setFont(*mNumberFont);

    mKPSText.setString("KPS");
    mKPSNumber.setString("0");
    if (Settings::KPSWindowEnabledFromStart)
    {
        mWindow.create(sf::VideoMode(Settings::KPSWindowSize.x, 
            Settings::KPSWindowSize.y), "KPS Window", sf::Style::Close);
    }

    setupText();
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

void KPSWindow::render()
{
    if (mWindow.isOpen())
    {
        sf::Transform textTransform = sf::Transform::Identity;
        sf::Transform numberTranform = sf::Transform::Identity;

        textTransform.translate((mWindow.getSize().x - mKPSText.getLocalBounds().width + 
            mKPSText.getLocalBounds().left) / 2.f, Settings::KPSWindowTopPadding);

        numberTranform.translate(mWindow.getSize().x / 2.f - mKPSNumber.getLocalBounds().left, 
            mKPSText.getLocalBounds().height + Settings::KPSWindowDistanceBetween + Settings::KPSWindowTopPadding);

        mWindow.clear(Settings::KPSBackgroundColor);
        mWindow.draw(mKPSText, textTransform);
        mWindow.draw(mKPSNumber, numberTranform);
        mWindow.display();
    }
}

void KPSWindow::setupText()
{
    mKPSText.setOrigin(mKPSText.getLocalBounds().left, mKPSText.getLocalBounds().top);
    mKPSNumber.setOrigin((mKPSNumber.getLocalBounds().left + mKPSNumber.getLocalBounds().width) / 2.f, 
        mKPSNumber.getLocalBounds().top);

    mKPSText.setCharacterSize(Settings::KPSTextSize);
    mKPSNumber.setCharacterSize(Settings::KPSNumberSize);

    mKPSText.setFillColor(Settings::KPSTextColor);
    mKPSNumber.setFillColor(Settings::KPSNumberColor);

    mWindow.setSize(sf::Vector2u(Settings::KPSWindowSize.x, Settings::KPSWindowSize.y));
    mWindow.setView(sf::View( { 0, 0, static_cast<float>(Settings::KPSWindowSize.x), static_cast<float>(Settings::KPSWindowSize.y) } ));
}   

bool KPSWindow::parameterIdMatches(LogicalParameter::ID id)
{
    return
        id == LogicalParameter::ID::KPSWndwSz ||
        id == LogicalParameter::ID::KPSWndwTxtChSz ||
        id == LogicalParameter::ID::KPSWndwNumChSz ||
        id == LogicalParameter::ID::KPSWndwBgClr ||
        id == LogicalParameter::ID::KPSWndwTxtClr ||
        id == LogicalParameter::ID::KPSWndwNumClr ||
        id == LogicalParameter::ID::KPSWndwTopPadding ||
        id == LogicalParameter::ID::KPSWndwDistBtw;
}
