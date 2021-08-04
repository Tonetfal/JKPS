#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "Background.hpp"
#include "KPSWindow.hpp"
#include "Button.hpp"
#include "GfxStatisticsLine.hpp"
#include "StatisticLinesPositioner.hpp"
#include "GfxButtonSelector.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>

class Menu;


class Application
{
    public:
        Application(Menu &menu);
        void run();


    private:
        void processInput();
        void handleEvent();
		void update();
		void render();

        void unloadChangesQueue();

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

        unsigned getWindowWidth();
        unsigned getWindowHeight();

        static bool parameterIdMatches(LogicalParameter::ID id);


    private:
        static const sf::Time TimePerFrame;

        sf::RenderWindow mWindow;
        TextureHolder mTextures;
        FontHolder mFonts;

        Menu& mMenu;
        // Settings& mSettings;
        // std::unique_ptr<Statistics> mStatistics;
        // std::unique_ptr<Background> mBackground;
        // std::unique_ptr<KPSWindow> mKPSWindow;

        std::array<std::unique_ptr<GfxStatisticsLine>, GfxStatisticsLine::StatisticsIdCounter> mStatistics;
        std::unique_ptr<StatisticsPositioner> mStatisticsPositioner;
        std::vector<std::unique_ptr<Button>> mButtons;
        std::unique_ptr<GfxButtonSelector> mGfxButtonSelector;

        sf::Vector2i mLastMousePosition;
};