#include "../Headers/Palette.hpp"
#include "../Headers/MathHelper.hpp"
#include "../Headers/ParameterLine.hpp"
#include "../Headers/Settings.hpp"

#include <SFML/Window/Event.hpp>


float Palette::mDistance(0.2f);

Palette::Palette(int)
: mWindowOffset(5.f, 10.f)
, mLineSize(mLine.size())
, mNormilizedMousePos(0.f, 0.f)
, mIndicatorColor(sf::Color::White)
, mLineElemIdx(0)
, wasButtonPressedOnLine(false)
, wasButtonPressedOnCanvas(false)
{
    mWindow.setFramerateLimit(60);

    auto color = sf::Color::Red;
    float colorStep = mLineSize / 2, leftSide = 5.f, rightSide = 25.f;
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

    mLineIndicator.setSize(sf::Vector2f(25.f, 3.f));
    mLineIndicator.setOutlineThickness(1.f);
    mLineIndicator.setFillColor(sf::Color::Black);
    mLineIndicator.setOrigin(mLineIndicator.getSize() / 2.f);
    mLineIndicator.setPosition((mLine[0].position + mLine[1].position) / 2.f);

	const auto decrease = 5.f;
	const auto origin = sf::Vector2f(35.f, 0.f);
    mCanvas[0].position = origin + sf::Vector2f(decrease, decrease);
    mCanvas[0].color = sf::Color::White;
    mCanvas[1].position = origin + sf::Vector2f(0.f, mLineRect.height) + sf::Vector2f(decrease, -decrease);
    mCanvas[1].color = sf::Color::Black;
    mCanvas[2].position = origin + sf::Vector2f(300.f, mLineRect.height) + sf::Vector2f(-decrease, -decrease);
    mCanvas[2].color = sf::Color::Black;
    mCanvas[3].position = origin + sf::Vector2f(300.f, 0.f) - sf::Vector2f(decrease, -decrease);
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
    mCanvasIndicator.setPosition(mCanvas[0].position);
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
    auto mousePos = sf::Mouse::getPosition(mWindow) -
        static_cast<sf::Vector2i>(mWindowOffset);

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
    auto mousePos = sf::Mouse::getPosition(mWindow) -
        static_cast<sf::Vector2i>(mWindowOffset);

    // Make canvas indicator move even if cursor is outside the palette
    if (mousePos.x < mCanvasRect.left)
        mousePos.x = mCanvasRect.left;

    if (mousePos.x > mCanvasRect.width + mCanvasRect.left)
        mousePos.x = mCanvasRect.width + mCanvasRect.left;

    if (mousePos.y < mCanvasRect.top)
        mousePos.y = mCanvasRect.top;

    if (mousePos.y > mCanvasRect.height)
        mousePos.y = mCanvasRect.height;

    mNormilizedMousePos = sf::Vector2f(
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
        mNormilizedMousePos));

    ParameterLine::setColor(mIndicatorColor);
}

void Palette::processOwnEvents()
{
    auto event = sf::Event();
    while (mWindow.pollEvent(event))
    {
        // Move on the line by only one step
        if (event.type == sf::Event::KeyPressed)
        {
            const auto key = event.key;
            if (key.code == sf::Keyboard::Left)
                goUp();
            else if (key.code == sf::Keyboard::Right)
                goDown();

            if (key.control && key.code == Settings::KeyExit)
            {
                closeWindow();
                return;
            }

            mLineIndicator.setPosition(mLineIndicator.getPosition().x, mLine[mLineElemIdx].position.y);
            setColor();
        }

        // Don't move the indicator of anything if the left mouse button wasn't pressed on that area
        if (event.type == sf::Event::MouseButtonPressed)
        {
            const auto mousePos = 
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(mWindow)) -
                static_cast<sf::Vector2f>(mWindowOffset);

            if (mCanvasRect.contains(mousePos))
                wasButtonPressedOnCanvas = true;
            if (mLineRect.contains(mousePos))
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

    auto transform = sf::Transform::Identity;
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

void Palette::openWindow(sf::Vector2i position)
{
    if (!mWindow.isOpen())
    {
        sf::Uint32 style;
#ifdef _WIN32
        style = sf::Style::Close;
#elif linux
        style = sf::Style::Default;
#else
#error Unsupported compiler
#endif

        const auto width = 340.f + mWindowOffset.x * 2.f;
        const auto height = mDistance * (mLineSize - 1) / 2.f + mWindowOffset.y * 2.f;

        mWindow.create(sf::VideoMode(width, height), "JKPS RGB color selector", style);
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