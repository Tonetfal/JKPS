#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "KeyPressingManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "Statistics.hpp"
#include "LogicalParameter.hpp"

#include <memory>
#include <vector>


class Button : public sf::Drawable, public sf::Transformable
{
    public:
        enum AnimationStyle
        {
            Light = 1,
            Press,
            AnimationCounter,
        };


    public:
                                    Button(const TextureHolder& textureHolder, const FontHolder& fontHolder);

        void                        update(std::vector<bool>& needToBeReleased);
        void                        handleInput(std::vector<bool>& clickedKeys, KeyPressingManager& container);
        void                        highlightKey(int buttonIndex);
        virtual void                draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void                        setFonts();
        void                        setupAssets(bool newTexture); 
        void                        setupKeyCounterTextVec();

        void                        resize();
        void                        clear();

        static bool                 parameterIdMatches(LogicalParameter::ID id);


    private:
        void                        updateAnimation(const std::vector<bool>& needToBeReleased);
        void                        updateButtonText();

        void                        setTextures(std::vector<std::unique_ptr<sf::Sprite>>& vector, sf::Texture& texture);
        void                        setColor(std::vector<std::unique_ptr<sf::Sprite>>& vector, const sf::Color& color);
        void                        scaleTexture(std::vector<std::unique_ptr<sf::Sprite>>& vector, const sf::Vector2u& texture);
        void                        centerOrigin(std::vector<std::unique_ptr<sf::Sprite>>& vector);
        void                        setButtonPositions(std::vector<std::unique_ptr<sf::Sprite>>& vector);
        sf::Vector2f                getDefaultScale() const;
        sf::Vector2f                getScaleForText() const;
        sf::Vector2f                getScaleAmountPerFrame() const;
        float                       getTextMaxWidth() const;
        float                       getTextMaxHeight() const;
        sf::Vector2f                getCenterOriginText(const sf::Text &text) const;

        void                        setupTextPosition(sf::Text &text, unsigned idx);
        void                        setupKeyCounterText(sf::Text &text, unsigned idx);
        bool                        isTextTooBig(const sf::Text &text) const;
        void                        decreaseTextCharacterSize(sf::Text &text);
        std::string                 getButtonText(sf::Text &text, unsigned idx);
        std::string                 getVisualStr(unsigned idx);

        void                        resizeVectors();
        bool                        isBeyondDefaultScale(const sf::Sprite &sprite) const;

        void                        lightUpKey(unsigned idx);
        void                        fadeKeyLight(unsigned idx);
        void                        raiseKey(unsigned idx);
        void                        lowerKey(unsigned idx);

        unsigned                    getKeyCountersWidth(const sf::Text &text, unsigned idx) const;
        unsigned                    getKeyCountersHeight(const sf::Text &text) const;

    
    private:
        const TextureHolder         &mTextures;
        const FontHolder            &mFonts;

        std::vector<long>           mKeyCounters;
        std::vector<std::unique_ptr<sf::Text>> mButtonsText;

        std::vector<std::unique_ptr<sf::Sprite>> mButtonsSprite;
        std::vector<std::unique_ptr<sf::Sprite>> mAnimationSprite;

        std::vector<float>          mButtonsYOffset;

        std::vector<float>          mCurDefaultTextHeight;
        std::vector<unsigned>       mKeyCounterDigits;
};