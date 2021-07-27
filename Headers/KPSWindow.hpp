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
        void                    handleEvent(sf::Event event);
        void                    handleOwnEvent();
        void                    update(std::size_t kps);
        void                    render();

        void                    setupText();
        static bool             parameterIdMatches(LogicalParameter::ID id);


    private:
        void                    moveWindow();
        void                    calculateWidthCenter(unsigned idx) const;


    private:
        sf::RenderWindow        mWindow;
        sf::Font*               mTextFont;
        sf::Font*               mNumberFont;
        sf::Text                mKPSText;
        sf::Text                mKPSNumber;

        float*                  mDefaultWidthCenters;
};