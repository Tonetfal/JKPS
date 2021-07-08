#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>


class KPSWindow
{
    public:
                                KPSWindow();
        void                    handleEvent(sf::Event event);
        void                    handleOwnEvent();
        void                    update(std::size_t kps);
        void                    draw();

        void                    loadFont(const sf::Font &font);


    private:
        void                    moveWindow();
        void                    calculateWidthCenter(unsigned idx) const;


    private:
        sf::RenderWindow        mWindow;
        sf::Text                mKPSText;
        sf::Text                mKPSNumber;

        float*                  mDefaultWidthCenters;
};