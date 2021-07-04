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
class Statistic;
class Mode;

class Graphics : private sf::NonCopyable
{
    public:
        Graphics(sf::RenderWindow &window);
        void update(sf::Time dt);
        void draw();

        CommandQueue &getCommandQueue();

        static unsigned int getWindowWidth();
        static unsigned int getWindowHeight();
        // ~Graphics();


    private:
        void loadTextures();
        void loadFonts();
        void buildScene();

        void changeButtons();



    private:
        sf::RenderWindow &mWindow;
        sf::View mView;
        TextureHolder mTextures;
        FontHolder mFonts;

        SceneNode mSceneGraph;
        CommandQueue mCommandQueue;

        Statistic* mStatFirstLine;
        // Mode* mMode;
        // std::array<SceneNode*, 10> mButtons;
        bool mNeedToRelease;
};