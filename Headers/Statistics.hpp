#pragma once

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "KeyPressingManager.hpp"
#include "ResourceIdentifiers.hpp"
#include "DataHolder.hpp"
#include "StringHelper.hpp"

#include <vector>
#include <string>
#include <stddef.h>
#include <math.h>
#include <cmath>

#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>

class Statistics
{
    public:
        enum ID
        {
            KPS,
            MaxKPS,
            TotalKeys,
            BPM,
        };

        enum KeyCountersType
        {
            Numbers,
            Letters,
        };

        
    public:
                                    Statistics(sf::RenderWindow& window);

        void                        update(size_t KeyPerSecond, size_t BeatsPerMinute, std::vector<int>& clickedKeys);
        void                        handleEvent(sf::Event event);
        void                        draw();

        void                        loadFonts(FontHolder& font);
        void                        setFonts();

        std::size_t                 getKeyPerSecond();
        std::size_t                 getMaxKeyPerSecond();
        std::size_t                 getTotalKeys();

        std::size_t                 getTotalStatisticsHeight();


    private:
        void                        setupText(ID id);
        void                        setupLong(ID id);
        void                        setupString(ID id, const std::string& name);

        unsigned int                getStatisticsWidth();
        unsigned int                getStatisticsHeight(ID id);


    private:
        sf::RenderWindow&           mWindow;

        sf::Font*                   mStatisticsFont;

        DataHolder<sf::Text, ID>    mTexts;
        DataHolder<long, ID>        mLongs;
        DataHolder<std::string, ID> mStrings;
};