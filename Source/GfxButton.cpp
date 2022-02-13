#include "../Headers/GfxButton.hpp"
#include "../Headers/ResourceHolder.hpp"
#include "../Headers/Settings.hpp"
#include "../Headers/Button.hpp"
#include "../Headers/Application.hpp"
#include "../Headers/Utility.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>

#include <algorithm>


bool GfxButton::mShowBounds(false);
int GfxButton::mSelectedKeyBounds(-1);

GfxButton::GfxButton(const unsigned idx, const TextureHolder &textureHolder, const FontHolder &fontHolder)
: mTextures(textureHolder)
, mFonts(fontHolder)
// , mEmitter(textureHolder.get(Textures::KeyPressVis))
, mEmitter(idx)
, mLastKeyState(false)
, mButtonsHeightOffset(0.f)
, mBtnIdx(idx)
{
    for (unsigned i = 0; i < SpriteIdCounter; ++i)
    {
        auto spritePtr = std::make_unique<sf::Sprite>();
        mSprites[static_cast<SpriteID>(i)] = std::move(spritePtr);
    }

    for (unsigned i = 0; i < TextIdCounter; ++i)
    {
        auto textPtr = std::make_unique<sf::Text>();
        mTexts[static_cast<TextID>(i)] = std::move(textPtr);
    }

    updateAssets();
    updateParameters();

    mBounds.setFillColor(sf::Color::Transparent);
    mBounds.setOutlineColor(sf::Color::Magenta);
    mBounds.setOutlineThickness(1.f);
}

void GfxButton::update(bool keyState)
{
    if (Settings::LightAnimation)
        keyState ? lightKey() : fadeKey();
    if (Settings::PressAnimation)
        keyState ? lowerKey() : raiseKey();

    if (Settings::KeyPressVisToggle)
    {
        // Create a new rectangle on button press if last frame the button was not pressed
        if (!mLastKeyState && keyState)
        {
            const auto &buttonSprite = *mSprites[ButtonSprite];
            const auto rect = buttonSprite.getGlobalBounds();
            mEmitter.create({ rect.width, rect.height });
        }
    }

    mEmitter.update(keyState, mLastKeyState);

    mLastKeyState = keyState;
}

void GfxButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    auto drawText = [this, &target] (const sf::Text &text, sf::RenderStates states)
        {
            target.draw(text, states);
            const auto boundsStates = states.transform.translate(text.getPosition());

            if (mShowBounds && (mSelectedKeyBounds == -1 || mSelectedKeyBounds == mBtnIdx))
            {
                target.draw(mBounds, boundsStates);
            }
        };
    states.transform *= getTransform();

    // Key visualizer's graphics
    target.draw(mEmitter, states);

    // Key's graphics
    for (const auto &sprite : mSprites)
        target.draw(*sprite, states);

    // Key's text
    if (Settings::ButtonTextShowVisualKeys) 
        drawText(*mTexts[VisualKey], states);
    if (Settings::ButtonTextShowTotal)
        drawText(*mTexts[KeyCounter], states);
    if (Settings::ButtonTextShowKPS)
        drawText(*mTexts[KeyPerSecond], states);
    if (Settings::ButtonTextShowBPM)
        drawText(*mTexts[BeatsPerMinute], states);

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

    const sf::Color animationStep(0, 0, 0, static_cast<sf::Uint8>(255 / Settings::AnimationFrames));
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
    if (!Settings::ButtonTextIgnoreBtnMovement)
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

    const auto step = std::min(getRiseStep(), mButtonsHeightOffset);

    for (auto &sprite : mSprites)
    {
        const auto position = sprite->getPosition();
        sprite->setPosition(position.x, position.y - step);
    }
    if (!Settings::ButtonTextIgnoreBtnMovement)
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
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto sepValAdvMode = isInSupportedRange && Settings::ButtonTextSepPosAdvancedMode;
    const auto advTextMode = isInSupportedRange && Settings::ButtonTextAdvancedMode;
    const auto advGfxMode = isInSupportedRange && Settings::GfxButtonAdvancedMode;

    const auto color = !advGfxMode ? Settings::GfxButtonTextureColor : Settings::GfxButtonsColor[mBtnIdx];

    scaleSprites();
    mSprites[ButtonSprite]->setColor(color);
    // Substraction by black (0,0,0,255) is needed to set alpha channel on 0 when any related animation key parameter is changed
    mSprites[AnimationSprite]->setColor(Settings::AnimationColor - sf::Color::Black);

    auto idx = 0ul;
    for (auto &text : mTexts)
    {
        const auto color = !advTextMode ? Settings::ButtonTextColor : Settings::ButtonTextAdvColor[mBtnIdx];
        const auto chSz = !advTextMode ? Settings::ButtonTextCharacterSize : Settings::ButtonTextAdvCharacterSize[mBtnIdx];
        const auto outThck = (!advTextMode ? Settings::ButtonTextOutlineThickness : Settings::ButtonTextAdvOutlineThickness[mBtnIdx]) / 10.f;
        const auto outColor = !advTextMode ? Settings::ButtonTextOutlineColor : Settings::ButtonTextAdvOutlineColor[mBtnIdx];
        const auto bold = !advTextMode ? Settings::ButtonTextBold : Settings::ButtonTextAdvBold[mBtnIdx];
        const auto italic = !advTextMode ? Settings::ButtonTextItalic : Settings::ButtonTextAdvItalic[mBtnIdx];
        const auto advPos = Settings::ButtonsTextAdvPosition[mBtnIdx];
        const auto origPos = Utility::swapY(Settings::ButtonTextPosition + (advTextMode ? advPos : sf::Vector2f()));
        auto pos = origPos;

        switch(idx)
        {
            case VisualKey:
                pos += Utility::swapY(Settings::ButtonTextVisualKeysTextPosition + (sepValAdvMode 
                    ? Settings::ButtonTextAdvVisualKeysTextPosition[mBtnIdx] : sf::Vector2f())); 
                break;

            case KeyCounter:
                pos += Utility::swapY(Settings::ButtonTextTotalTextPosition + (sepValAdvMode 
                    ? Settings::ButtonTextAdvTotalTextPosition[mBtnIdx] : sf::Vector2f())); 
                break;

            case KeyPerSecond:
                pos += Utility::swapY(Settings::ButtonTextKPSTextPosition + (sepValAdvMode 
                    ? Settings::ButtonTextAdvKPSTextPosition[mBtnIdx] : sf::Vector2f())); 
                break;

            case BeatsPerMinute:
                pos += Utility::swapY(Settings::ButtonTextBPMTextPosition + (sepValAdvMode 
                    ? Settings::ButtonTextAdvBPMTextPosition[mBtnIdx] : sf::Vector2f()));
                break;
        }

        text->setFillColor(color);
        text->setCharacterSize(chSz);
        text->setPosition(pos);
        text->setStyle(sf::Uint32((bold ? sf::Text::Bold : 0) | (italic ? sf::Text::Italic : 0)));
        text->setOutlineThickness(outThck);
        text->setOutlineColor(outColor);

        const auto lAlt = Settings::ShowOppOnAlt && sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt);
        if ((lAlt && idx == VisualKey && Settings::ButtonTextShowVisualKeys && !Settings::ButtonTextShowTotal)
        || (lAlt && idx == KeyCounter && !Settings::ButtonTextShowVisualKeys && Settings::ButtonTextShowTotal))
        {
            text->setPosition(pos);
        }

        ++idx;
    }

    const auto size = !advTextMode ? Settings::ButtonTextBounds : Settings::ButtonTextAdvBounds[mBtnIdx];
    mBounds.setSize(size);
    mBounds.setOrigin(size / 2.f);
}

void GfxButton::resetAssets()
{
    mSprites[ButtonSprite]->setTexture(mTextures.get(Textures::Button), true);
    mSprites[AnimationSprite]->setTexture(mTextures.get(Textures::Animation), true);

    const auto &font = mFonts.get(Fonts::ButtonValue);
    for (auto &text : mTexts)
        text->setFont(font);
}

void GfxButton::scaleSprites()
{
    auto &buttonSprite = *mSprites[ButtonSprite];
    auto &animationSprite = *mSprites[AnimationSprite];

    const auto origBtnTxtrSz = buttonSprite.getTexture()->getSize();
    const auto origAniTxtrSz = animationSprite.getTexture()->getSize();

    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::GfxButtonAdvancedMode;

    const auto btnTxtrSz = !advMode ? static_cast<sf::Vector2f>(Settings::GfxButtonTextureSize) : Settings::GfxButtonsSizes[mBtnIdx];
    const sf::Vector2f btnTxtrScale(btnTxtrSz.x / origBtnTxtrSz.x, btnTxtrSz.y / origBtnTxtrSz.y);
    const sf::Vector2f aniTxtrScale(btnTxtrSz.x / origAniTxtrSz.x, btnTxtrSz.y / origAniTxtrSz.y);

    buttonSprite.setScale(btnTxtrScale);
    animationSprite.setScale(aniTxtrScale);
}

bool isInBounds(sf::Vector2f bounds, sf::FloatRect rect)
{
    return rect.width > bounds.x || rect.height > bounds.y;
}

void GfxButton::keepInBounds(sf::Text &text)
{
    if (!Settings::ButtonTextBoundsToggle)
        return;
        
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::ButtonTextAdvancedMode;
    const auto bounds = !advMode ? Settings::ButtonTextBounds : Settings::ButtonTextAdvBounds[mBtnIdx];
    auto rect = text.getLocalBounds();

    for (auto chSz = text.getCharacterSize(); 
        chSz > 2u && isInBounds(bounds, rect); 
        --chSz)
    {
        text.setCharacterSize(chSz);
        rect = text.getLocalBounds();
    }
}

sf::Vector2f getTextCenter(const sf::Text &text)
{
    const auto rect = text.getLocalBounds();
    return { rect.left + rect.width / 2, rect.top + rect.height / 2};
}

void GfxButton::centerOrigins()
{
    auto &buttonSprite = *mSprites[ButtonSprite];
    auto &animationSprite = *mSprites[AnimationSprite];
    const auto buttonTextureSize = static_cast<sf::Vector2f>(buttonSprite.getTexture()->getSize());
    const auto animationTextureSize = static_cast<sf::Vector2f>(animationSprite.getTexture()->getSize());
    buttonSprite.setOrigin(buttonTextureSize / 2.f);
    animationSprite.setOrigin(animationTextureSize / 2.f);

    for (auto &text : mTexts)
        text->setOrigin(getTextCenter(*text));
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

void GfxButton::setShowBounds(bool flag, int idx)
{
    mShowBounds = flag;
    mSelectedKeyBounds = idx;
}

GfxButton::~GfxButton()
{    
}

// GfxButton::RectEmitter::RectEmitter(const sf::Texture &texture)
GfxButton::RectEmitter::RectEmitter(unsigned btnIdx)
: mBtnIdx(btnIdx)
//, mTexture(texture) 
// , mTopVertecies(sf::Quads, 1000u)
, mMiddleVertecies(sf::Quads, 1000u)
// , mBottomVertecies(sf::Quads, 1000u)
{
    const auto count = mMiddleVertecies.getVertexCount() / 4;
    for (auto i = 0ul; i < count; ++i)
        mAvailableRectIndices.emplace_back(i);
}

void GfxButton::RectEmitter::update(bool keyState, bool prevKeyState)
{
    // Don't update anything if there is no active rectangles
    if (mUsedRectIndices.empty())
        return;

    std::vector<size_t> toRemove;

    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::GfxButtonAdvancedMode;
    const auto origSpeed = !advMode ? Settings::KeyPressVisSpeed : 
        Settings::KeyPressVisAdvSpeed[mBtnIdx];
    const auto speed = -origSpeed / 10.f;
    const auto len = !advMode ? Settings::KeyPressVisFadeLineLen : 
        Settings::KeyPressVisAdvFadeLineLen[mBtnIdx];

    // Iterate through all rectangles
    for (auto i : mUsedRectIndices)
    {
        // Flag which identifies if all the rectangle vertices are on the same height
        auto eachVertexIsOnLimit = true;

        // Iterate through all the rectangle vertices
        const auto vertexIndex = i * 4lu;
        for (size_t j = vertexIndex; j < vertexIndex + 4lu; ++j)
        {
            // Take vertex reference
            // auto &topSideVertex = mTopVertecies[j];
            auto &middleVertex = mMiddleVertecies[j];
            // auto &bottomSideVertex = mBottomVertecies[j];

            // Limit the square to go beyond the line length
            auto move = [len, speed] (sf::Vertex &vertex) 
                { 
                    vertex.position.y = -std::min(std::abs(vertex.position.y + speed), len);
                };
            // move(topSideVertex);
            move(middleVertex);
            // move(bottomSideVertex);

            // Check if the current vertex is on the max height, do so only if previous were so
            // if (eachVertexIsOnLimit)
            // {
            //     eachVertexIsOnLimit = std::abs(bottomSideVertex.position.y) 
            //         == len;
            // }
            if (eachVertexIsOnLimit)
            {
                eachVertexIsOnLimit = std::abs(middleVertex.position.y) == len;
            }

            // Set the right alpha channel depending on the progress to the end of the fade out length line
            // topSideVertex.color = getVertexColor(mTopVertecies, j);
            middleVertex.color = getVertexColor(mMiddleVertecies, j);
            // bottomSideVertex.color = getVertexColor(mBottomVertecies, j);
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
    if (keyState)
    {
        const auto offset = mUsedRectIndices.back() * 4;

        mMiddleVertecies[offset + 2].position.y = 
        mMiddleVertecies[offset + 3].position.y -= speed;

        // mBottomVertecies[offset + 0].position.y = 
        // mBottomVertecies[offset + 1].position.y -= speed;

        // mBottomVertecies[offset + 2].position.y = 
        // mBottomVertecies[offset + 3].position.y -= speed;
    }

    // // Move the nearest rectangle up on release
    if (prevKeyState && !keyState && !mUsedRectIndices.empty())
    {
        const auto offset = mUsedRectIndices.back() * 4ul;

        mMiddleVertecies[offset + 2].position.y = 
        mMiddleVertecies[offset + 3].position.y -= speed;
    }
}

void GfxButton::RectEmitter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    states.transform = getPressRectTransform(states.transform);

    target.draw(mMiddleVertecies, states);

    // states.texture = &mTexture;
    // target.draw(mTopVertecies, states);
    // target.draw(mBottomVertecies, states);
}

void GfxButton::RectEmitter::setPosition(sf::Vector2f position)
{
    mEmitterPosition = position;
}

void GfxButton::RectEmitter::pushVertecies(sf::VertexArray &vertexArray, sf::Vertex *toPush, size_t offset, sf::Vector2f buttonSize)
{
    for (size_t i = 0; i < 4 ; ++i)
    {
        // Take reference
        auto &vertex = toPush[i];

        // Move the position to the emitter's origin
        vertex.position += mEmitterPosition - sf::Vector2f(0.f, buttonSize.y / 2.f);

        const auto idx = offset + i;

        // Change the color
        vertex.color = getVertexColor(vertexArray, idx);
        
        // Assign the created vertex to the contrainer
        vertexArray[idx] = vertex;
    }
}

void GfxButton::RectEmitter::create(sf::Vector2f buttonSize)
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::GfxButtonAdvancedMode;
    const auto origSpeed = !advMode ? Settings::KeyPressVisSpeed : 
        Settings::KeyPressVisAdvSpeed[mBtnIdx];
    const auto speed = -origSpeed / 10.f;

    const auto rectSize = sf::Vector2f(buttonSize.x, speed);
    const auto halfRectSize = rectSize / 2.f;

    // const auto textureSize = static_cast<sf::Vector2f>(mTexture.getSize());

    const auto rectIndex = mAvailableRectIndices.back();
    const auto firstVertexIndex = rectIndex * 4ul;

    // 0 Top left, 1 Top right, 2 Bottom right, 3 Bottom left
    sf::Vertex middleVertices[4];

    // middleVertices[0].position = sf::Vector2f(-halfRectSize.x, -halfRectSize.y);
    // middleVertices[1].position = sf::Vector2f(+halfRectSize.x, -halfRectSize.y);
    // middleVertices[2].position = sf::Vector2f(+halfRectSize.x, -halfRectSize.y);
    // middleVertices[3].position = sf::Vector2f(-halfRectSize.x, -halfRectSize.y);

    middleVertices[0].position = sf::Vector2f(-halfRectSize.x, -rectSize.y);
    middleVertices[1].position = sf::Vector2f(+halfRectSize.x, -rectSize.y);
    middleVertices[2].position = sf::Vector2f(+halfRectSize.x, 0.f);
    middleVertices[3].position = sf::Vector2f(-halfRectSize.x, 0.f);

    pushVertecies(mMiddleVertecies, middleVertices, firstVertexIndex, buttonSize);


    // sf::Vertex topVertices[4];

    // topVertices[0].position = sf::Vector2f(-halfRectSize.x, -halfRectSize.y * 2.f);
    // topVertices[1].position = sf::Vector2f(+halfRectSize.x, -halfRectSize.y * 2.f);
    // topVertices[2].position = sf::Vector2f(+halfRectSize.x, -halfRectSize.y);
    // topVertices[3].position = sf::Vector2f(-halfRectSize.x, -halfRectSize.y);
    
    // // Make the texture be rotated
    // topVertices[0].texCoords = sf::Vector2f(+textureSize.x, +textureSize.y);
    // topVertices[1].texCoords = sf::Vector2f(0.f,            +textureSize.y);
    // topVertices[2].texCoords = sf::Vector2f(0.f,            0.f);
    // topVertices[3].texCoords = sf::Vector2f(+textureSize.x, 0.f);

    // pushVertecies(mTopVertecies, topVertices, firstVertexIndex, buttonSize);


    // sf::Vertex bottomVertices[4];

    // bottomVertices[0].position = sf::Vector2f(-halfRectSize.x, -halfRectSize.y);
    // bottomVertices[1].position = sf::Vector2f(+halfRectSize.x, -halfRectSize.y);
    // bottomVertices[2].position = sf::Vector2f(+halfRectSize.x, 0.f);
    // bottomVertices[3].position = sf::Vector2f(-halfRectSize.x, 0.f);
    
    // bottomVertices[0].texCoords = sf::Vector2f(0.f,            0.f);
    // bottomVertices[1].texCoords = sf::Vector2f(+textureSize.x, 0.f);
    // bottomVertices[2].texCoords = sf::Vector2f(+textureSize.x, +textureSize.y);
    // bottomVertices[3].texCoords = sf::Vector2f(0.f,            +textureSize.y);

    // pushVertecies(mBottomVertecies, bottomVertices, firstVertexIndex, buttonSize);

    
    // Remove the used index from the available rect indices list 
    // and push it to the used one
    mAvailableRectIndices.pop_back();
    mUsedRectIndices.emplace_back(rectIndex);
}

void GfxButton::RectEmitter::scaleTexture(sf::Vector2f buttonSize)
{
    // const auto size = static_cast<sf::Vector2f>(mTexture.getSize());
    // mTextureScale = sf::Vector2f(size.x / buttonSize.x, size.x / buttonSize.y);
}

sf::Transform GfxButton::RectEmitter::getPressRectTransform(sf::Transform transform) const
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::KeyPressVisAdvSettingsMode;
    const auto rot = !advMode ? Settings::KeyPressVisRotation : 
        Settings::KeyPressVisAdvRotation[mBtnIdx];
    const auto orig = Settings::KeyPressVisOrig + (advMode 
        ? Settings::KeyPressVisAdvOrig[mBtnIdx] : sf::Vector2f());

    transform.rotate(-rot);
    transform = transform.translate(orig.x, -orig.y);
    return transform;
}

float GfxButton::RectEmitter::getVertexProgress(size_t vertexNumber, float vertexHeight) const
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::KeyPressVisAdvSettingsMode;
    const auto len = !advMode ? Settings::KeyPressVisFadeLineLen : 
        Settings::KeyPressVisAdvFadeLineLen[mBtnIdx];

    return std::min(mEmitterPosition.y - vertexHeight / len, 1.f);
}

sf::Color GfxButton::RectEmitter::getVertexColor(const sf::VertexArray &vertexArray, size_t vertexIndex) const
{
    const auto isInSupportedRange = mBtnIdx < Settings::SupportedAdvancedKeysNumber;
    const auto advMode = isInSupportedRange && Settings::KeyPressVisAdvSettingsMode;
    auto color = !advMode ? Settings::KeyPressVisColor : 
        Settings::KeyPressVisAdvColor[mBtnIdx];

    color.a -= color.a * getVertexProgress(vertexIndex, vertexArray[vertexIndex].position.y);
    return color;
}
