#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "ResourceIdentifiers.hpp"

#include <string>
#include <vector>
#include <memory>


struct GraphicalParameter : public sf::Drawable, public sf::Transformable
{
    public:
        GraphicalParameter(const std::string &str, unsigned n, sf::Vector2f rectSize = sf::Vector2f(70.f, 25.f));
        GraphicalParameter(const std::string &str);
        GraphicalParameter(unsigned);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void setupValPos();
        sf::FloatRect getGlobalBounds() const;

        void setInverseMark();
        void setRightTexture();

        static float getPosX();


    public:
        static const TextureHolder *mTextures;
        static const FontHolder *mFonts;

        sf::Sprite mSprite;
        sf::RectangleShape mRect;
        static sf::Color defaultRectColor;
        static sf::Color defaultSelectedRectColor;
        sf::Text mValText;
};