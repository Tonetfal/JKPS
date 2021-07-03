#include "../Headers/Graphics.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Statistic.hpp"
#include "../Headers/Calculator.hpp"
#include "../Headers/Mode.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <algorithm>
#include <cmath>


Graphics::Graphics(sf::RenderWindow &window)
: mWindow(window)
, mView(window.getDefaultView())
, mTextures()
, mFonts()
{
    loadTextures();
    loadFonts();
    buildScene();
}

void Graphics::update(sf::Time dt)
{
    while (!mCommandQueue.isEmpty())
    {
        mSceneGraph.onCommand(mCommandQueue.pop(), dt);
    }
    
    mSceneGraph.update(dt);
}

void Graphics::draw()
{
    mWindow.setView(mView);
    mWindow.draw(mSceneGraph);
}

CommandQueue &Graphics::getCommandQueue()
{
    return mCommandQueue;
}

void Graphics::loadTextures()
{
    mTextures.loadFromFile(Textures::Button, "../Media/Textures/Button.png");
    mTextures.loadFromFile(Textures::Animation, "../Media/Textures/Animation.png");
}

void Graphics::loadFonts()
{
    mFonts.loadFromFile(Fonts::KeyCounter, "../Media/Fonts/neuropol.ttf");
    mFonts.loadFromFile(Fonts::Statistic, "../Media/Fonts/neuropol.ttf");
}

void Graphics::buildScene()
{
    // Calculator
    std::unique_ptr<Calculator> calculator(new Calculator());
    mSceneGraph.attachChild(std::move(calculator));

    // Mode
    std::unique_ptr<Mode> mode(new Mode());
    mode->setPosition(mWindow.getSize().x - Settings::AlertRadius, Settings::AlertRadius);
    mSceneGraph.attachChild(std::move(mode));
    
    // Statistics
    std::unique_ptr<Statistic> statistic = nullptr;
    Statistic *firstLine = nullptr;

    statistic = std::unique_ptr<Statistic>(new Statistic(mFonts, Statistic::KPS));
    statistic->setPosition(Statistic::getStartPosition(statistic->getHeight()));
    firstLine = statistic.get();
    mSceneGraph.attachChild(std::move(statistic));

    int decrement = 0;
    for (size_t i = 1; i < Statistic::StatisticCounter; ++i)
    {
        switch (static_cast<Statistic::Type>(i))
        {
            case Statistic::MaxKPS:
                if (!Settings::ShowMaxKPS)
                {
                    ++decrement;
                    continue;
                }
            case Statistic::BPM:
                if (!Settings::ShowBPMText)
                {
                    decrement++;
                    continue;
                }
            default:
                break;
        }
        statistic = std::unique_ptr<Statistic>(new Statistic(mFonts, static_cast<Statistic::Type>(i)));
        statistic->setPosition(0, (firstLine->getHeight() + Settings::StatisticsDistance) * (i - decrement));
        firstLine->attachChild(std::move(statistic));
    }

    // Buttons
    std::unique_ptr<Button> button(nullptr);
    sf::Vector2f textureCenter(static_cast<sf::Vector2f>(Settings::ButtonTextureSize) / 2.f);
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        button = std::unique_ptr<Button>(new Button(mTextures, mFonts, Settings::mKeys[i]));
        button->setPosition(sf::Vector2f(Button::getWidth(i), Button::getHeight(i)));
        mSceneGraph.attachChild(std::move(button));
    }
}