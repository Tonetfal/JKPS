#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ResourceIdentifiers.hpp"
#include "DataHolder.hpp"
#include "LogicalParameter.hpp"

#include <string>
#include <vector>


class Statistics : public sf::Drawable, public sf::Transformable
{
    public:
        enum ID
        {
            KPS,
            MaxKPS,
            TotalKeys,
            BPM,
            StatisticsCounter
        };

        
    public:
                                    Statistics(const FontHolder& fonts);

        void                        update(size_t KeyPerSecond, size_t BeatsPerMinute, std::vector<int>& clickedKeys);
        void                        resize();
        virtual void                draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void                        setupText();

        std::size_t                 getKeyPerSecond();
        std::size_t                 getMaxKeyPerSecond();
        std::size_t                 getTotalKeys();

        std::size_t                 getTotalStatisticsHeight() const;

        void                        clear();

        static bool                 parameterIdMatches(LogicalParameter::ID id);


    private:
        void                        setFonts();
        void                        setupText(ID id);
        void                        setTextPositions();
        void                        setupLong(ID id);
        void                        setupString(ID id, const std::string& name);
        void                        setTextString(ID id);

        unsigned int                getStatisticsWidth() const;
        unsigned int                getStatisticsHeight(ID id) const;


    private:
        sf::Font*                   mStatisticsFont;

        DataHolder<sf::Text, ID>    mTexts;
        DataHolder<long, ID>        mLongs;
        DataHolder<std::string, ID> mStrings;
};