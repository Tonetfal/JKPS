#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Calculation.hpp"
#include "Settings.hpp"
#include "KeyPressingManager.hpp"
#include "Statistics.hpp"
#include "Button.hpp"
#include "Background.hpp"
#include "DefaultFiles.hpp"


class Application
{
    public:
                                    Application(Settings& settings);
        void                        run();


    private:
        void					    processInput();
        void                        handleEvent(sf::Event event);
		void					    update(sf::Time dt);
		void					    render();

        void                        loadAssets();

        void                        moveWindow();
        unsigned int                getWindowWidth();
        unsigned int                getWindowHeight();


    private:
        static const sf::Time       TimePerFrame;

        sf::RenderWindow            mWindow;
        TextureHolder               mTextures;
        FontHolder                  mFonts;

        Settings&                   mSettings;
        Calculation                 mCalculation;
        KeyPressingManager          mKeyPressingManager;
        Statistics                  mStatistics;
        Button                      mButtons;
        Background                  mBackground;

        sf::Vector2i                mLastMousePosition;
};