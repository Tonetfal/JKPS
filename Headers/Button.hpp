#pragma once

#include "SceneNode.hpp"
#include "ResourceIdentifier.hpp"
#include "Settings.hpp"
#include "Keys.hpp"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#define TEXT_DEBUG
#ifdef TEXT_DEBUG
#include "GraphicalDebug.hpp"
#endif


class Button : public SceneNode
{
    private:
        enum Animation
        {
            Light,
            Press,
        };


    public:
        Button(const TextureHolder &textures, const FontHolder &fonts, Keys key);
		virtual void updateCurrent(sf::Time dt);

        void pressButton();

        Keys getKey() const;
		unsigned getCategory() const;

        static float getWidth(size_t index);
        static float getHeight(size_t index);
        
        void clear();


    private:
        virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const;

        void lightKey();
        void fadeKey();
        void reduceButtonSize();
        void increaseButtonSize();

        void setupText();
        void centerText();
        void centerOrigin();

        void makeFitText();

        void scaleTexture();
        void setupScaledScale(sf::Vector2f defaultScale);
        sf::Vector2f getDefaultScale() const;
        sf::Vector2f getScaleAmountPerFrame() const;
        sf::Vector2f getScaleAmountPerFrameText() const;


    private:
        sf::Sprite mButtonSprite;
        sf::Sprite mAnimationSprite;
        sf::Text mText;

        Keys mKey;
        size_t mCounter;
        bool mIsPressed;
        bool mIsAnimationActive;
        unsigned int mPrevModeState;

#ifdef TEXT_DEBUG
        GraphicalDebug mDebug;
#endif
};