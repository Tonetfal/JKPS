#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Menu.hpp"
#include "Button.hpp"
#include "ButtonPositioner.hpp"
#include "GfxButtonSelector.hpp"
#include "GfxStatisticsLine.hpp"
#include "StatisticLinesPositioner.hpp"
#include "Background.hpp"
#include "KPSWindow.hpp"
#include "KeysPerSecondGraph.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>


class Application
{
    public:
        Application();
        void run();

        static unsigned getWindowWidth();
        static unsigned getWindowHeight();
        static sf::IntRect getWindowRect();

        static bool parameterIdMatches(LogicalParameter::ID id);


    private:
        void processInput();
        void handleEvent();
		void update();
		void render();

        void unloadChangesQueue();
        void resetAssets();

        void loadTextures();
        void loadFonts();
        void loadIcon();

        void buildStatistics();
        void buildButtons();

        // Function returns bool value and the index of button on which the click is performed
        bool isPressPerformedOnButton(unsigned &btnIdx) const;
        bool isMouseInRange(unsigned idx) const;

        void addButton(LogKey &logKey);
        void removeButton();

        void openWindow();
        void resizeWindow();
        void moveWindow();


    private:
        static const sf::Time TimePerFrame;

        sf::RenderWindow mWindow;
        
        TextureHolder mTextures;
        FontHolder mFonts;

        Menu mMenu;

        std::array<std::unique_ptr<GfxStatisticsLine>, GfxStatisticsLine::StatisticsIdCounter> mStatistics;
        std::unique_ptr<StatisticsPositioner> mStatisticsPositioner;

        std::vector<std::unique_ptr<Button>> mButtons;
        std::unique_ptr<ButtonPositioner> mButtonsPositioner;
        std::unique_ptr<GfxButtonSelector> mGfxButtonSelector;

        std::unique_ptr<Background> mBackground;
        std::unique_ptr<KPSWindow> mKPSWindow;
        std::unique_ptr<KeysPerSecondGraph> mGraph;
};