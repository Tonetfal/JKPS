#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifier.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "CommandQueue.hpp"
#include "Command.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include <array>
#include <queue>


// Forward declaration
namespace sf
{
	class RenderWindow;
}

class Graphics : private sf::NonCopyable
{
    public:
        Graphics(sf::RenderWindow &window);
        void update(sf::Time dt);
        void draw();

        CommandQueue &getCommandQueue();


    private:
        void loadTextures();
        void loadFonts();
        void buildScene();


    private:
        sf::RenderWindow &mWindow;
        sf::View mView;
        TextureHolder mTextures;
        FontHolder mFonts;

        SceneNode mSceneGraph;
        CommandQueue mCommandQueue;
};