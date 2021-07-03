#include "../Headers/Graphics.hpp"

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
    std::unique_ptr<Button> button(nullptr);
    
    sf::Vector2f textureCenter(static_cast<sf::Vector2f>(Settings::ButtonTextureSize) / 2.f);
    for (size_t i = 0; i < Settings::ButtonAmount; ++i)
    {
        button = std::unique_ptr<Button>(new Button(mTextures, mFonts, Settings::mKeys[i]));
        button->setPosition(sf::Vector2f(Button::getWidth(i), Button::getHeight(i)));
        mSceneGraph.attachChild(std::move(button));
    }
}