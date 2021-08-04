#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "ResourceIdentifiers.hpp"
#include "LogicalParameter.hpp"


class KPSWindow
{
    public:
        KPSWindow(const FontHolder &fonts);

        void handleOwnEvent();
        void update();
        void render();

        void openWindow();
        void closeWindow();
        bool isOpen() const;

        void updateParameters();
        void updateAssets();
        static bool parameterIdMatches(LogicalParameter::ID id);


    private:
        void moveWindow();
        void calculateWidthCenter(unsigned idx) const;


    private:
        sf::RenderWindow mWindow;

        const FontHolder &mFonts;
        sf::Text mKPSText;
        sf::Text mKPSNumber;
};