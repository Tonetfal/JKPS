#include "../Headers/GfxButton.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Application.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <algorithm>


bool GfxButton::mShowBounds(false);

GfxButton::GfxButton(const unsigned idx, const TextureHolder& textureHolder, const FontHolder& fontHolder)
: mTextures(textureHolder)
, mFonts(fontHolder)
, mLastKeyState(false)
, mButtonsHeightOffset(0.f)
, mBtnIdx(idx)
{
    for (unsigned i = 0; i < SpriteIdCounter; ++i)
    {
        std::unique_ptr<sf::Sprite> spritePtr(new sf::Sprite);
        mSprites[static_cast<SpriteID>(i)] = std::move(spritePtr);
    }

    for (unsigned i = 0; i < TextIdCounter; ++i)
    {
        std::unique_ptr<sf::Text> textPtr(new sf::Text);
        mTexts[static_cast<TextID>(i)] = std::move(textPtr);
    }

    updateAssets();
    updateParameters();

    mBounds.setFillColor(sf::Color::Transparent);
    mBounds.setOutlineColor(sf::Color::Magenta);
    mBounds.setOutlineThickness(1.f);
    mBounds.setSize(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    mBounds.setOrigin(mBounds.getSize() / 2.f);
}

void GfxButton::update(bool buttonPressed)
{
    if (Settings::LightAnimation)
        buttonPressed ? lightKey() : fadeKey();
    if (Settings::PressAnimation)
        buttonPressed ? lowerKey() : raiseKey();

    if (Settings::KeyPressVisToggle)
    {
        // Create a new rectangle on button press if last frame the button was not pressed
        if (!mLastKeyState && buttonPressed)
        {
            const auto &buttonSprite = *mSprites[ButtonSprite];
            const auto rect = buttonSprite.getGlobalBounds();
            mEmitter.create({ rect.width, rect.height });
        }
    }

    mEmitter.update(buttonPressed);

    mLastKeyState = buttonPressed;
}

void GfxButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform *= getTransform();

    // Key visualizer's graphics
    target.draw(mEmitter, states);

    // Key's graphics
    for (const auto &sprite : mSprites)
        target.draw(*sprite, states);

    // Key's text
    if (Settings::ButtonTextSepPosAdvancedMode)
    {
        if (Settings::ButtonTextShowVisualKeys) 
            target.draw(*mTexts[VisualKey], states);
        if (Settings::ButtonTextShowTotal)
            target.draw(*mTexts[KeyCounter], states);
        if (Settings::ButtonTextShowKPS)
            target.draw(*mTexts[KeyPerSecond], states);
        if (Settings::ButtonTextShowBPM)
            target.draw(*mTexts[BeatsPerMinute], states);
    }
    else
    {
        const TextID id = getTextIdToDisplay();
        if (id <= BeatsPerMinute)
            target.draw(*mTexts[id], states);
    }

    if (mShowBounds)
        target.draw(mBounds, states);
}

void GfxButton::lightKey()
{
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor);
    setScale(Settings::AnimationScale / 100.f);
}

void GfxButton::fadeKey()
{
    auto &animationSprite = *mSprites[AnimationSprite];
    const auto color = animationSprite.getColor();
    auto scale = getScale();
    if (scale.x == 1.f && scale.y == 1.f && color.a == 0.f)
        return;

    const sf::Color animationStep(0, 0, 0, 255 / Settings::AnimationFrames);
    const auto scaleStep = getScaleStep();

    animationSprite.setColor(color - animationStep);
    setScale(scale + scaleStep);

    scale = getScale();
    // Scaling can go beyond due to floating point issues
    if ((scaleStep.x > 0.f && scale.x > 1.f) || (scaleStep.x < 0 && scale.x < 1.f))
        setScale(1.f, getScale().y);
    if ((scaleStep.y > 0.f && scale.y > 1.f) || (scaleStep.y < 0 && scale.y < 1.f))
        setScale(getScale().x, 1.f);
}

void GfxButton::lowerKey()
{
    if (mButtonsHeightOffset == Settings::AnimationOffset)
        return;

    for (auto &sprite : mSprites)
    {
        const auto position = sprite->getPosition();
        sprite->setPosition(position.x, position.y + Settings::AnimationOffset - mButtonsHeightOffset);
    }
    if (!Settings::BtnTextIgnoreBtnMovement)
    {
        for (auto &label : mTexts)
        {
            const auto position = label->getPosition();
            label->setPosition(position.x, position.y + Settings::AnimationOffset - mButtonsHeightOffset);
        }
    }
    mButtonsHeightOffset = Settings::AnimationOffset;
}

void GfxButton::raiseKey()
{
    if (mButtonsHeightOffset <= 0.f)
        return;

    const float step = std::min(getRiseStep(), mButtonsHeightOffset);

    for (auto &sprite : mSprites)
    {
        const auto position = sprite->getPosition();
        sprite->setPosition(position.x, position.y - step);
    }
    if (!Settings::BtnTextIgnoreBtnMovement)
    {
        for (auto &label : mTexts)
        {
            const auto position = label->getPosition();
            label->setPosition(position.x, position.y - step);
        }
    }
    mButtonsHeightOffset = std::max(mButtonsHeightOffset - step, 0.f);
}

sf::Vector2f GfxButton::getScaleStep() const
{
    return (sf::Vector2f(1.f, 1.f) - Settings::AnimationScale / 100.f) / static_cast<float>(Settings::AnimationFrames);
}

float GfxButton::getRiseStep() const
{
    return Settings::AnimationOffset / Settings::AnimationFrames;
}

void GfxButton::updateAssets()
{
    resetAssets();
    scaleSprites();
    centerOrigins();
}

void GfxButton::updateParameters()
{
    scaleSprites();
    mSprites[ButtonSprite]->setColor(Settings::GfxButtonTextureColor);
    // Substraction by black (0,0,0,255) is needed to set alpha channel on 0 when any related animation key parameter is changed
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor - sf::Color::Black);

    unsigned idx = 0;
    for (auto &text : mTexts)
    {
        const bool isInRange = mBtnIdx < Settings::GfxButtonsBtnPositions.size();
        const sf::Vector2f advancedPos = !Settings::ButtonTextPosAdvancedMode || !isInRange ? 
            sf::Vector2f(Settings::ButtonTextPosition.x, -Settings::ButtonTextPosition.y) : 
            sf::Vector2f(Settings::ButtonsTextPositions[mBtnIdx].x, -Settings::ButtonsTextPositions[mBtnIdx].y);

        text->setFillColor(Settings::ButtonTextColor);
        text->setCharacterSize(Settings::ButtonTextCharacterSize);
        text->setPosition(advancedPos);
        text->setStyle((Settings::ButtonTextBold ? sf::Text::Bold : 0) | (Settings::ButtonTextItalic ? sf::Text::Italic : 0));
        text->setOutlineThickness(Settings::ButtonTextOutlineThickness / 10.f);
        text->setOutlineColor(Settings::ButtonTextOutlineColor);

        if (Settings::ButtonTextSepPosAdvancedMode)
        {
            switch(idx)
            {
                case VisualKey:      text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonVisualKeysTextPosition.x, -Settings::ButtonVisualKeysTextPosition.y)); break;
                case KeyCounter:     text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonTotalTextPosition.x, -Settings::ButtonTotalTextPosition.y)); break;
                case KeyPerSecond:   text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonKPSTextPosition.x, -Settings::ButtonKPSTextPosition.y)); break;
                case BeatsPerMinute: text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonBPMTextPosition.x, -Settings::ButtonBPMTextPosition.y)); break;
            }
        }

        const bool lAlt = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
        if (lAlt && idx == VisualKey && Settings::ButtonTextShowVisualKeys && !Settings::ButtonTextShowTotal)
            text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonTotalTextPosition.x, -Settings::ButtonTotalTextPosition.y));
        if (lAlt && idx == KeyCounter && !Settings::ButtonTextShowVisualKeys && Settings::ButtonTextShowTotal)
            text->setPosition(advancedPos + sf::Vector2f(Settings::ButtonVisualKeysTextPosition.x, -Settings::ButtonVisualKeysTextPosition.y));

        ++idx;
    }

    mBounds.setSize(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    mBounds.setOrigin(mBounds.getSize() / 2.f);
}

void GfxButton::resetAssets()
{
    mSprites[ButtonSprite]->setTexture(mTextures.get(Textures::Button), true);
    mSprites[AnimationSprite]->setTexture(mTextures.get(Textures::Animation), true);

    const sf::Font &font(mFonts.get(Fonts::ButtonValue));
    for (auto &text : mTexts)
        text->setFont(font);
}

void GfxButton::scaleSprites()
{
    auto &buttonSprite = *mSprites[ButtonSprite];
    auto &animationSprite = *mSprites[AnimationSprite];
    const auto origBtnTxtrSz = buttonSprite.getTexture()->getSize();
    const auto origAniTxtrSz = animationSprite.getTexture()->getSize();
    const bool isInRange = mBtnIdx < Settings::GfxButtonsBtnPositions.size();
    const sf::Vector2f btnTxtrSz(!Settings::GfxButtonSizesAdvancedMode || !isInRange ? sf::Vector2f(Settings::GfxButtonTextureSize) : Settings::GfxButtonsSizes[mBtnIdx]);
    const sf::Vector2f btnTxtrScale(btnTxtrSz.x / origBtnTxtrSz.x, btnTxtrSz.y / origBtnTxtrSz.y);
    const sf::Vector2f aniTxtrScale(btnTxtrSz.x / origAniTxtrSz.x, btnTxtrSz.y / origAniTxtrSz.y);

    buttonSprite.setScale(btnTxtrScale);
    animationSprite.setScale(aniTxtrScale);
}

void GfxButton::keepInBounds(sf::Text &text)
{
    const sf::Vector2f bounds(static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) - Settings::ButtonTextBounds);
    sf::FloatRect rect(text.getLocalBounds());
    unsigned chSz = text.getCharacterSize();;

    while ((rect.width > bounds.x || rect.height > bounds.y) && chSz > 2)
    {
        text.setCharacterSize(chSz - 1);

        chSz = text.getCharacterSize();
        rect = text.getLocalBounds();
    }
}

void GfxButton::centerOrigins()
{
    auto &buttonSprite = *mSprites[ButtonSprite];
    auto &animationSprite = *mSprites[AnimationSprite];
    auto buttonTextureSize = static_cast<sf::Vector2f>(buttonSprite.getTexture()->getSize());
    auto animationTextureSize = static_cast<sf::Vector2f>(animationSprite.getTexture()->getSize());
    buttonSprite.setOrigin(buttonTextureSize / 2.f);
    animationSprite.setOrigin(animationTextureSize / 2.f);

    for (auto &text : mTexts)
        text->setOrigin(getTextCenter(*text));
}

sf::Vector2f GfxButton::getTextCenter(const sf::Text &text)
{
    const sf::FloatRect rect = text.getLocalBounds();
    return { rect.left + rect.width / 2, rect.top + rect.height / 2};
}

float GfxButton::getWidth(unsigned idx)
{
    const float width = Settings::WindowBonusSizeLeft + 
        (Settings::GfxButtonTextureSize.x + Settings::GfxButtonDistance) * idx + 
        Settings::GfxButtonTextureSize.x / 2;
    return width;
}

float GfxButton::getHeight(unsigned idx)
{
    const float height = Settings::WindowBonusSizeTop + Settings::GfxButtonTextureSize.y / 2;
    return height;
}

GfxButton::TextID GfxButton::getTextIdToDisplay()
{
    if (Settings::ButtonTextShowVisualKeys) 
        return VisualKey;
    if (Settings::ButtonTextShowTotal)
        return KeyCounter;
    if (Settings::ButtonTextShowKPS)
        return KeyPerSecond;
    if (Settings::ButtonTextShowBPM)
        return BeatsPerMinute;
    
    return Nothing;
}

void GfxButton::setShowBounds(bool show)
{
    mShowBounds = show;
}

GfxButton::~GfxButton()
{    
}

GfxButton::RectEmitter::RectEmitter()
: mVertecies(sf::Quads, 400u)
{
    const auto count = mVertecies.getVertexCount() / 4.f;
    for (size_t i = 0; i < count; ++i)
        mAvailableRectIndices.emplace_back(i);
}

void GfxButton::RectEmitter::update(bool buttonPressed)
{
    // Don't update anything if there is no active rectangles
    if (mUsedRectIndices.empty())
        return;

    std::vector<size_t> toRemove;
    float speed = -Settings::KeyPressVisSpeed / 10.f;

    // Iterate through all rectangles
    for (auto i : mUsedRectIndices)
    {
        // Flag which identifies if all the rectangle vertices are on the same height
        bool eachVertexIsOnLimit = true;

        // Iterate through all the rectangle vertices
        const auto vertexIndex = i * 4;
        for (size_t j = vertexIndex; j < vertexIndex + 4; ++j)
        {
            // Take vertex reference
            auto &vertex = mVertecies[j];

            // Limit the square to go beyond the line length
            vertex.position.y = -std::min(std::abs(vertex.position.y + speed), Settings::KeyPressVisFadeLineLen);

            // Check if the current vertex is on the max height, do so only if previous were so
            if (eachVertexIsOnLimit)
            {
                eachVertexIsOnLimit = bool(Settings::KeyPressVisFadeLineLen 
                    == std::abs(vertex.position.y));
            }

            // Set the right alpha channel depending on the progress to the end of the fade out length line
            vertex.color = getVertexColor(j);
        }

        // All vertices are on the same height
        if (eachVertexIsOnLimit)
        {
            // Add the rectangle index to available index list, add to list of rectangles to remove
            mAvailableRectIndices.emplace_back(i);
            toRemove.emplace_back(i);
        }
    }

    // Iterate through all the indices of the rectangles to remove
    for (auto i : toRemove)
    {
        // Remove every index that is equal to i
        mUsedRectIndices.erase(std::remove(
                mUsedRectIndices.begin(), mUsedRectIndices.end(), i), 
            mUsedRectIndices.end());
    }

    // If a button is pressed don't let the spawning rectangle go away from the spawn point
    if (buttonPressed)
    {
        const auto lastCreatedRectIndex = mUsedRectIndices.back() * 4;
        mVertecies[lastCreatedRectIndex + 2].position.y = 
        mVertecies[lastCreatedRectIndex + 3].position.y -= speed; 
    }
}

void GfxButton::RectEmitter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform = getPressRectTransform(states.transform);

    target.draw(mVertecies, states);
}

void GfxButton::RectEmitter::setPosition(sf::Vector2f position)
{
    mEmitterPosition = position;
}

void GfxButton::RectEmitter::create(sf::Vector2f buttonSize)
{
    // 0 Top left, 1 Top right, 2 Bottom right, 3 Bottom left
    sf::Vertex vertices[4];

    auto rectSize = sf::Vector2f(buttonSize.x, Settings::KeyPressVisSpeed / 10.f);
    auto halfRectSize = rectSize / 2.f;
    
    // Create and position the 4 vertices
    vertices[0].position = sf::Vector2f(-halfRectSize.x, -rectSize.y);
    vertices[1].position = sf::Vector2f(+halfRectSize.x, -rectSize.y);
    vertices[2].position = sf::Vector2f(+halfRectSize.x, 0.f);
    vertices[3].position = sf::Vector2f(-halfRectSize.x, 0.f);

    // Iterate through created vertices
    const auto rectIndex = mAvailableRectIndices.back();
    const auto firstVertexIndex = rectIndex * 4;
    for (size_t i = 0; i < 4 ; ++i)
    {
        // Take reference
        auto &vertex = vertices[i];

        // Move the position to the emitter's origin
        vertex.position += mEmitterPosition - sf::Vector2f(0.f, buttonSize.y / 2.f);

        // Change the color
        vertex.color = getVertexColor(firstVertexIndex + i);
        
        // Assign the created vertex to the contrainer
        mVertecies[firstVertexIndex + i] = vertex;
    }
    
    // Remove the used index from the available rect indices list 
    // and push it to the used one
    mAvailableRectIndices.pop_back();
    mUsedRectIndices.emplace_back(rectIndex);
}

sf::Transform GfxButton::RectEmitter::getPressRectTransform(sf::Transform transform) const
{
    transform.rotate(-Settings::KeyPressVisRotation);
    transform = transform.translate(Settings::KeyPressVisOrig.x, -Settings::KeyPressVisOrig.y);
    return transform;
}

float GfxButton::RectEmitter::getVertexProgress(size_t vertexNumber, float vertexHeight) const
{
    float originOffset;
    float height = mLastRectSize.y / 2.f;
    if (vertexNumber % 4 == 0)
        originOffset = -height;
    else if (vertexNumber % 5 == 0)
        originOffset = -height;
    else if (vertexNumber % 6 == 0)
        originOffset = +height;
    else 
        originOffset = +height;

    return std::min(mEmitterPosition.y - vertexHeight / Settings::KeyPressVisFadeLineLen, 1.f);
}

sf::Color GfxButton::RectEmitter::getVertexColor(size_t vertexIndex) const
{
    auto color = Settings::KeyPressVisColor;
    color.a -= color.a * getVertexProgress(vertexIndex, mVertecies[vertexIndex].position.y);
    return color;
}
