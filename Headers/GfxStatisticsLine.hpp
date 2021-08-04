#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "ResourceIdentifiers.hpp"
#include "LogicalParameter.hpp"



class GfxStatisticsLine : public sf::Drawable, public sf::Transformable
{
    public:
        enum StatisticsID
        {
            KPS,
            Total,
            BPM,
            StatisticsIdCounter
        };


    public:
        GfxStatisticsLine(const FontHolder& fontHolder, const bool &show, const StatisticsID identifier);

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        void update();
        void updateAsset();
        void updateParameters();

        bool getShowState() const;
        const sf::Text &getText() const;

        static GfxStatisticsLine::StatisticsID getPreviousLineID(StatisticsID current);
        static std::string getString(StatisticsID id);

        static bool parameterIdMatches(LogicalParameter::ID id);

    
    private:
        void centerOrigin();


    private:
        const FontHolder &mFonts;

        const StatisticsID mIdentifier;

        const bool &mShow;
        sf::Text mText;
};