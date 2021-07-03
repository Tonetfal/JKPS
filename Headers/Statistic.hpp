#pragma once

#include "SceneNode.hpp"
#include "ResourceIdentifier.hpp"

#include <SFML/Graphics/Text.hpp>


class Statistic
{
    public:
        Statistic(const FontHolder &fonts);
        virtual void updateCurrent(sf::Time dt);

        unsigned getCategory() const;

        static float getWidth(size_t index);
        static float getHeight(size_t index);


    private:
        virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const;


    private:
        sf::Text mText;
};