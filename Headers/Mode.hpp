#pragma once

#include "SceneNode.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/CircleShape.hpp>

class Mode : public SceneNode, private sf::NonCopyable
{
    public:
        enum State
        {
            Normal,
            Edit
        };


    public:
        Mode();

        static State getState();
        

    private:
        static State mState;

        sf::CircleShape mAlert;
};