#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Calculation.hpp"
#include "KeyPressingManager.hpp"
#include "Statistics.hpp"
#include "Button.hpp"
#include "Background.hpp"
#include "KPSWindow.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

class Menu;


class Application
{
    public:
                                    Application(Menu &menu);
        void                        run();


    private:
        void					    processInput();
        void                        handleEvent(sf::Event event);
		void					    update(sf::Time dt);
		void					    render();

        void                        loadAssets();

        void                        openWindow();
        void                        moveWindow();
        unsigned int                getWindowWidth();
        unsigned int                getWindowHeight();

        static bool                 parameterIdMatches(LogicalParameter::ID id);


    private:
        static const sf::Time       TimePerFrame;

        sf::RenderWindow            mWindow;
        TextureHolder               mTextures;
        FontHolder                  mFonts;

        Menu&                       mMenu;
        Settings&                   mSettings;
        Calculation                 mCalculation;
        KeyPressingManager          mKeyPressingManager;
        std::unique_ptr<Statistics> mStatistics;
        std::unique_ptr<Button>     mButtons;
        std::unique_ptr<Background> mBackground;
        std::unique_ptr<KPSWindow>  mKPSWindow;

        sf::Vector2i                mLastMousePosition;
};