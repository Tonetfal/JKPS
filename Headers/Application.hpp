#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Calculation.hpp"
#include "Settings.hpp"
#include "KeyPressingManager.hpp"
#include "Statistics.hpp"
#include "Button.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

class Application
{
    public:
                                    Application(Settings& settings);
        void                        run();


    private:
        void					    processInput();
		void					    update(sf::Time dt);
		void					    render();

        void                        handleEvent(sf::Event event);


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
};