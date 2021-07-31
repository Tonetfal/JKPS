#include "../Headers/Palette.hpp"
#include "../Headers/MathHelper.hpp"
#include "../Headers/ParameterLine.hpp"

#include <SFML/Window/Event.hpp>


float Palette::mDistance(0.3f);

Palette::Palette(int)
: mWindowOffset(10.f, 20.f)
, mLineSize(mLine.size())
, mNormilizedMouseVec(1.f, 0.f)
, mIndicatorColor(sf::Color::Red)
, mLineElemIdx(mLineSize - 1)
, wasButtonPressedOnLine(false)
, wasButtonPressedOnCanvas(false)
{
    mWindow.setFramerateLimit(60);

    sf::Color color(sf::Color::Red);
    float colorStep = mLineSize / 2, leftSide = 10.f, rightSide = 50.f;
    int s = 0;
    for (unsigned i = 0; i < mLineSize; i += 2)
    {
        float y = mDistance * i / 2;
        mLine[i].position = sf::Vector2f(leftSide, y);
        mLine[i].color = color;
        mLine[i + 1].position = sf::Vector2f(rightSide, y);
        mLine[i + 1].color = color;

        color = rgb((i + 2) / 2 / colorStep);
    }

    mLineRect = sf::FloatRect(
        mLine[0].position.x, 
        mLine[0].position.y, 
        mLine[1].position.x - mLine[0].position.x, 
        mLine[mLineSize - 1].position.y - mLine[0].position.y);

    mLineIndicator.setSize(sf::Vector2f(45.f, 3.f));
    mLineIndicator.setOutlineThickness(1.f);
    mLineIndicator.setFillColor(sf::Color::Black);
    mLineIndicator.setOrigin(mLineIndicator.getSize() / 2.f);
    mLineIndicator.setPosition(sf::Vector2f(30.f, mLineRect.height));

    mCanvas[0].position = sf::Vector2f(75.f, 0.f);
    mCanvas[0].color = sf::Color::White;
    mCanvas[1].position = sf::Vector2f(75.f, mLineRect.height);
    mCanvas[1].color = sf::Color::Black;
    mCanvas[2].position = sf::Vector2f(475.f, mLineRect.height);
    mCanvas[2].color = sf::Color::Black;
    mCanvas[3].position = sf::Vector2f(475.f, 0.f);
    mCanvas[3].color = sf::Color::Red;

    mCanvasRect = sf::FloatRect(
        mCanvas[0].position.x, 
        mCanvas[0].position.y, 
        mCanvas[2].position.x - mCanvas[0].position.x, 
        mCanvas[2].position.y - mCanvas[0].position.y
    );

    float r = 6.f;
    mCanvasIndicator.setRadius(r);
    mCanvasIndicator.setFillColor(sf::Color::Transparent);
    mCanvasIndicator.setOutlineThickness(3.f);
    mCanvasIndicator.setOrigin(sf::Vector2f(r, r));
    mCanvasIndicator.setPosition(mCanvas[3].position);
}

void Palette::processInput()
{
    processOwnEvents();
    if (mWindow.hasFocus())
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)
        ||  sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
        ||  sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            moveLineIndicator();
            if (wasButtonPressedOnCanvas && sf::Mouse::isButtonPressed(sf::Mouse::Left))
                moveCanvasIndicator();
            setColor();
        }
    }

}

void Palette::moveLineIndicator()
{
    sf::Vector2i mousePos(sf::Mouse::getPosition(mWindow) -
        static_cast<sf::Vector2i>(mWindowOffset));

    if (wasButtonPressedOnLine)
    {
        // Make line indicator move even if cursor is outside the line
        if (mousePos.y < 0)
            mousePos.y = 0;

        if (mousePos.y > mLine[mLineSize - 1].position.y)
            mousePos.y = mLine[mLineSize - 1].position.y;

        if (mousePos.y >= 0 && mousePos.y <= mLine[mLineSize - 1].position.y)
            mLineElemIdx = positionToNumber(mousePos);

        mLineIndicator.setPosition(mLineIndicator.getPosition().x, mLine[mLineElemIdx].position.y);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        goUp();
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        goDown();

}

void Palette::goUp()
{
    if (mLineElemIdx - 2 >= 0)
        mLineElemIdx -= 2;
    else
        mLineElemIdx = 0;
}

void Palette::goDown()
{
    if (mLineElemIdx + 2 <= mLineSize - 1)
        mLineElemIdx += 2;
    else
        mLineElemIdx = mLineSize - 1;
}

void Palette::moveCanvasIndicator()
{
    sf::Vector2i mousePos(sf::Mouse::getPosition(mWindow) -
        static_cast<sf::Vector2i>(mWindowOffset));

    // Make canvas indicator move even if cursor is outside the palette
    if (mousePos.x < mCanvasRect.left)
        mousePos.x = mCanvasRect.left;

    if (mousePos.x > mCanvasRect.width + mCanvasRect.left)
        mousePos.x = mCanvasRect.width + mCanvasRect.left;

    if (mousePos.y < mCanvasRect.top)
        mousePos.y = mCanvasRect.top;

    if (mousePos.y > mCanvasRect.height)
        mousePos.y = mCanvasRect.height;

    mNormilizedMouseVec = sf::Vector2f(
        (mousePos.x - mCanvasRect.left) / mCanvasRect.width, 
        (mousePos.y - mCanvasRect.top) / mCanvasRect.height);
    
    mCanvasIndicator.setPosition(sf::Vector2f(mousePos));
}

void Palette::setColor()
{
    mCanvas[3].color = mLine[mLineElemIdx].color;

    mIndicatorColor = sf::Color(bilinearInterp(
        mCanvas[0].color,
        mCanvas[1].color,
        mCanvas[2].color,
        mCanvas[3].color,
        mNormilizedMouseVec));

    ParameterLine::setColor(mIndicatorColor);
}

void Palette::processOwnEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        // Move on the line by only one step
        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Left)
                goUp();

            if (event.key.code == sf::Keyboard::Right)
                goDown();

            mLineIndicator.setPosition(mLineIndicator.getPosition().x, mLine[mLineElemIdx].position.y);
            setColor();
        }

        // Don't move the indicator of something if the left mouse button wasn't pressed on that area
        if (event.type == sf::Event::MouseButtonPressed)
        {
            sf::Vector2i mousePos(sf::Mouse::getPosition(mWindow) -
                static_cast<sf::Vector2i>(mWindowOffset));

            if (mCanvasRect.contains(sf::Vector2f(mousePos)))
                wasButtonPressedOnCanvas = true;
            if (mLineRect.contains(sf::Vector2f(mousePos)))
                wasButtonPressedOnLine = true;
        }

        if (event.type == sf::Event::MouseButtonReleased)
            wasButtonPressedOnCanvas = wasButtonPressedOnLine = false;

        if (event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Palette::render()
{
    mWindow.clear(sf::Color(40,40,40));

    sf::Transform transform = sf::Transform::Identity;
    transform.translate(mWindowOffset);

    mWindow.draw(mLine.data(), mLineSize, sf::TriangleStrip, transform);
    mWindow.draw(mLineIndicator, transform);
    mWindow.draw(mCanvas.data(), 4, sf::Quads, transform);
    mWindow.draw(mCanvasIndicator, transform);

    mWindow.display();
}

void Palette::setColorOnPalette(sf::Color color)
{
    // set cursor on the palette
}

void Palette::openWindow()
{
    if (!mWindow.isOpen())
    {
        float width = 500 + mWindowOffset.x * 2;
        float height = mDistance * (mLineSize - 1) / 2 + mWindowOffset.y * 2;

        mWindow.create(sf::VideoMode(width, height), "JKPS RGB color selector", sf::Style::Close);
        mWindow.setKeyRepeatEnabled(false);
    }
}

void Palette::closeWindow()
{
    mWindow.close();
}

bool Palette::isWindowOpen() const
{
    return mWindow.isOpen();
}


sf::Color Palette::rgb(double ratio)
{
    // we want to normalize ratio so that it fits in to 6 regions
    // where each region is 256 units long
    int normalized = int(ratio * 256 * 6);

    // find the distance to the start of the closest region
    int x = normalized % 256;

    sf::Uint8 red = 0, grn = 0, blu = 0;
    switch(normalized / 256)
    {
    case 0: red = 255;      grn = 0;        blu = x;       break; // red -> magenta
    case 1: red = 255 - x;  grn = 0;        blu = 255;     break; // magenta -> blue
    case 2: red = 0;        grn = x;        blu = 255;     break; // blue -> cyan
    case 3: red = 0;        grn = 255;      blu = 255 - x; break; // cyan -> green
    case 4: red = x;        grn = 255;      blu = 0;       break; // green -> yellow
    case 5: red = 255;      grn = 255 - x;  blu = 0;       break; // yellow -> red
    }

    return { red, grn, blu };
}


int Palette::positionToNumber(sf::Vector2i position)
{
    return static_cast<int>(static_cast<float>(position.y) / mDistance * 2);
}