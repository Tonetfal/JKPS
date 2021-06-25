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
        void                        handleInput(KeyPressingManager& container);
        void                        handleHighlight(int buttonIndex);
        void                        draw();

        void                        loadFonts(FontHolder& font);
        void                        setFonts();
        void                        reset();

        std::size_t                 getKeyPerSecond();
        std::size_t                 getMaxKeyPerSecond();
        std::size_t                 getTotalKeys();
        long                        getKeyCounter(std::size_t index);


    private:
        void                        setupText(ID id);
        void                        setupTextVector();
        void                        setupTextPosition(int index);
        void                        setupLong(ID id);
        void                        setupLongVector();
        void                        setupLongVector(std::size_t index);
        void                        setupString(ID id, const std::string& name);
        void                        setupStringVector();

        void                        decreaseTextCharacterSize(int index);


        unsigned int                getStatisticsWidth();
        unsigned int                getStatisticsHeight(ID id);
        unsigned int                getKeyCountersWidth(size_t index);
        unsigned int                getKeyCountersHeight(size_t index);


    private:
        sf::RenderWindow&           mWindow;

        sf::Font*                   mStatisticsFont;
        sf::Font*                   mKeyCountersFont;

        DataHolder<sf::Text, ID>    mTexts;
        DataHolder<long, ID>        mLongs;
        DataHolder<std::string, ID> mStrings;

        std::vector<long>           mKeyCounters;
        std::vector<std::string>    mKeyCountersStrings;
        std::vector<sf::Text>       mKeyCountersText;
};