#pragma once

#include "SceneNode.hpp"
#include "Keys.hpp"

#include <SFML/Graphics/CircleShape.hpp>


class Mode : public SceneNode
{
    public:
        enum State
        {
            Normal,
            Edit
        };


    public:
        Mode();
        virtual void updateCurrent(sf::Time dt) override;

        void switchState();
        static void selectKey(Keys key);
        
        static Keys getSelectedKey();
        static State getState();
        unsigned getCategory() const;
        

    private:
        virtual void drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const override;


    private:
        static State mState;
        sf::CircleShape mAlert;
        static Keys mSelectedKey;
        static bool needToRelease;
};