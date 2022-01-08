#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "ResourceIdentifiers.hpp"

#include <array>
#include <memory>



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
            TextIdCounter,
            Nothing
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
        class RectEmitter : public sf::Drawable
        {
            public:
                RectEmitter();

                void update(bool buttonPressed);
                void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

                void setPosition(sf::Vector2f position);
                void create(sf::Vector2f buttonSize);


            private:
                sf::Transform getPressRectTransform(sf::Transform transform) const;
                float getVertexProgress(size_t vertexNumber, float vertexHeight) const;


            private:
                sf::VertexArray mVertecies;
                std::vector<size_t> mAvailableRectIndices;
                std::vector<size_t> mUsedRectIndices;
                sf::Vector2f mEmitterPosition;
                sf::Vector2f mLastRectSize;
        };


    private:
        const TextureHolder &mTextures;
        const FontHolder &mFonts;
        RectEmitter mEmitter;
        sf::RectangleShape mBounds;

        bool mLastKeyState;

        float mButtonsHeightOffset;

        const unsigned mBtnIdx;

        static bool mShowBounds;
};
