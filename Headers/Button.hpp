#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "KeyPressingManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "Settings.hpp"
#include "Statistics.hpp"

class Button
{
    public:
                                    Button(sf::RenderWindow& window);

        void                        update(std::vector<bool>& needToBeReleased);
        void                        handleInput(std::vector<bool>& clickedKeys, KeyPressingManager& container);
        void                        handleEvent(sf::Event event);
        void                        handleHighlight(int buttonIndex);
        void                        draw();

        void                        loadTextures(TextureHolder& textureHolder);
        void                        loadFonts(FontHolder& fontHolder);

        void                        setFonts();
        void                        setupTextures(); 

        void                        clear();

    private:
        void                        updateAnimation(const std::vector<bool>& needToBeReleased);
        void                        updateKeyCounters();

        void                        setTextures(std::vector<sf::Sprite>& vector, sf::Texture& texture);
        void                        setColor(std::vector<sf::Sprite>& vector, const sf::Color& color);
        void                        scaleTexture(std::vector<sf::Sprite>& vector, const sf::Vector2u& texture);
        void                        centerOrigin(std::vector<sf::Sprite>& vector);
        void                        setButtonPositions(std::vector<sf::Sprite>& vector);
        sf::Vector2f                getDefaultScale() const;
        sf::Vector2f                getScaleForText() const;
        sf::Vector2f                getScaleAmountPerFrame() const;

        void                        setupKeyCounterTextVec();
        void                        setupKeyCounterVec();
        void                        setupKeyCounterVec(size_t index); 
        void                        setupKeyCounterStrVec();
        void                        setupTextPosition(int index);
        void                        decreaseTextCharacterSize(int index);

        unsigned int                getKeyCountersWidth(size_t index) const;
        unsigned int                getKeyCountersHeight(size_t index) const;

    
    private:
        sf::RenderWindow&           mWindow;

        sf::Font*                   mKeyCountersFont;
        std::vector<long>           mKeyCounters;
        std::vector<sf::Text>       mKeyCountersText;

        sf::Texture*                mButtonTexture;
        sf::Texture*                mAnimationTexture;

        std::vector<sf::Sprite>     mButtonsSprite;
        std::vector<sf::Sprite>     mAnimationSprite;

        sf::CircleShape tmp;
};