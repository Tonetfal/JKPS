#pragma once

#include "SceneNode.hpp"
#include "ResourceIdentifier.hpp"

#include <SFML/Graphics/Text.hpp>

#define STATISTIC_DEBUG

#ifdef STATISTIC_DEBUG
#include "GraphicalDebug.hpp"
#endif

class Statistic : public SceneNode
{
    public:
        enum Type
        {
            KPS,
            MaxKPS,
            Total,
            BPM,
            StatisticCounter
        };


    public:
        Statistic(const FontHolder &fonts, Type type);
        virtual void updateCurrent(sf::Time dt);

        unsigned getCategory() const;
        static sf::Vector2f getStartPosition(float textHeight);

        float getWidth() const;
        float getHeight() const;

        void clear();


    private:
        virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const;

    private:
        sf::Text mText;
        std::string mStat;
        Type mType;

        std::function<int()> mFunction;

#ifdef STATISTIC_DEBUG
        GraphicalDebug mDebug;
#endif
};