#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ResourceIdentifiers.hpp"

#include <array>
#include <memory>

#include <SFML/Graphics/RectangleShape.hpp>


class GfxButton : public sf::Drawable, public sf::Transformable
{
    protected:
        enum SpriteID
        {
            ButtonSprite,
            AnimationSprite,
            SpriteIdCounter,
        };
        enum TextID
        {
            VisualKey,
            KeyCounter,
            KeyPerSecond,
            BeatsPerMinute,
            TextIdCounter
        };


    public:
        GfxButton(const unsigned idx, const TextureHolder& textureHolder, const FontHolder& fontHolder);

        void update(bool buttonPressed);
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void updateAssets();
        void updateParameters();

        static void setShowBounds(bool b);
        static float getWidth(unsigned idx);
        static float getHeight(unsigned idx);
        static sf::Vector2f getTextCenter(const sf::Text &text);
        static TextID getTextIdToDisplay();

        virtual ~GfxButton();


    protected:
        void keepInBounds(sf::Text &text);
        void centerOrigins();


    private:
        void resetAssets();
        void scaleSprites();

        // Light animation
        void lightKey();
        void fadeKey();

        // Press animation
        void lowerKey();
        void raiseKey();

        sf::Vector2f getScaleStep() const;
        float getRiseStep() const;


    protected:
        std::array<std::unique_ptr<sf::Sprite>, SpriteID::SpriteIdCounter> mSprites;
        std::array<std::unique_ptr<sf::Text>, TextID::TextIdCounter> mTexts;


    private:
        const TextureHolder &mTextures;
        const FontHolder &mFonts;
        sf::RectangleShape mBounds;

        float mButtonsHeightOffset;

        const unsigned mBtnIdx;

        static bool mShowBounds;
};
