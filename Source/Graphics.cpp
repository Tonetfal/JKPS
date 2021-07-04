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
, mStatFirstLine(nullptr)
// , mMode(nullptr)
, mNeedToRelease(false)
{
    loadTextures();
    loadFonts();
    buildScene();

    // for (auto &button : mButtons)
    //     button = nullptr;
}

void Graphics::update(sf::Time dt)
{
    if ((KeyCombination::isCombinationPressed(Settings::CombinationToIncrease)
    ||  KeyCombination::isCombinationPressed(Settings::CombinationToDecrease))
    &&  Mode::getState() == Mode::Edit)
    {
        changeButtons();
    }
    else
    {
        mNeedToRelease = false;
    }

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
    // mMode = mode.get();
    mode->setPosition(mWindow.getSize().x - Settings::AlertRadius, Settings::AlertRadius);
    mSceneGraph.attachChild(std::move(mode));
    
    // Statistics
    std::unique_ptr<Statistic> statistic = nullptr;

    statistic = std::unique_ptr<Statistic>(new Statistic(mFonts, Statistic::KPS));
    statistic->setPosition(Statistic::getStartPosition(statistic->getHeight()));
    mStatFirstLine = statistic.get();
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
        statistic->setPosition(0, (mStatFirstLine->getHeight() + Settings::StatisticsDistance) * (i - decrement));
        mStatFirstLine->attachChild(std::move(statistic));
    }

    // Buttons
    std::unique_ptr<Button> button(nullptr);
    sf::Vector2f textureCenter(static_cast<sf::Vector2f>(Settings::ButtonTextureSize) / 2.f);
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        button = std::unique_ptr<Button>(new Button(mTextures, mFonts, Settings::mKeys[i]));
        // mButtons[i] = button.get();
        button->setPosition(sf::Vector2f(Button::getWidth(i), Button::getHeight(i)));
        mSceneGraph.attachChild(std::move(button));
    }
}

void Graphics::changeButtons()
{
    if (mNeedToRelease)
        return;

    // if (KeyCombination::isCombinationPressed(Settings::CombinationToIncrease)
    // &&  Settings::maximumKeys > Settings::ButtonAmount + 1)
    // {
    //     unsigned newSize = Settings::ButtonAmount + 1;
    //     Settings::mKeys.push_back(Keys::A);
    //     std::unique_ptr<Button> button((new Button(mTextures, mFonts, Settings::mKeys[newSize])));
    //     mButtons[newSize - 1] = button.get();
    //     button->setPosition(sf::Vector2f(Button::getWidth(newSize - 1), Button::getHeight(newSize - 1)));
    //     mSceneGraph.attachChild(std::move(button));
    //     Settings::ButtonAmount = newSize;
    // }
    // if (KeyCombination::isCombinationPressed(Settings::CombinationToDecrease)
    // &&  Settings::minimumKeys < Settings::ButtonAmount - 1)
    // {
    //     unsigned newSize = Settings::ButtonAmount - 1;
    //     mSceneGraph.detachChild(*mButtons[newSize]);
    //     Settings::mKeys.resize(newSize);
    //     Settings::ButtonAmount = newSize;
    // }
    
    // Move statistics and resize the window
    mWindow.setSize(sf::Vector2u(getWindowWidth(), getWindowHeight()));
    mView.setSize(static_cast<sf::Vector2f>(mWindow.getSize()));
    mView.setCenter(static_cast<sf::Vector2f>(mWindow.getSize()) / 2.f);

    mStatFirstLine->setPosition(Statistic::getStartPosition(mStatFirstLine->getHeight()));
    // mMode->setPosition(mWindow.getSize().x - Settings::AlertRadius, Settings::AlertRadius);

    mNeedToRelease = true;
}

unsigned int Graphics::getWindowWidth()
{
    return (Settings::ButtonTextureSize.x + Settings::ButtonDistance) * Settings::ButtonAmount 
        + Settings::ButtonDistance + Settings::SpaceBetweenButtonsAndStatistics + Settings::SpaceOnStatisticsRight;
}

unsigned int Graphics::getWindowHeight()
{
    return Settings::ButtonTextureSize.y + Settings::ButtonDistance * 2;
}

// Graphics::~Graphics()
// {
    // std::cout << mStatFirstLine << "\n";
    // delete mStatFirstLine;
    // std::cout << mMode << "\n";
    // for (auto &elem : mButtons)
    //     std::cout << elem << "\n";
// }
