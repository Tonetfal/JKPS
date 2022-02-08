#pragma once

#include "ParameterLine.hpp"
#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "LogicalParameter.hpp"
#include "Settings.hpp"
#include "ChangedParametersQueue.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <vector>


class Button;

class Menu 
{
    public:
        Menu();

        void processInput();
        void update();
        void render();

        void openWindow();
        void closeWindow();
        bool isOpen() const;

        LogicalParameter &getParameter(LogicalParameter::ID id);
        ChangedParametersQueue &getChangedParametersQueue();

        void saveConfig(const std::vector<std::unique_ptr<Button>> &mKeys);
        void requestFocus();
        void requestReloadAssets();
        bool resetReloadAssetsRequest();


    private:
        void handleEvent();
        void handleRealtimeInput();

        void loadFonts();
        void loadTextures();

        void selectTab(unsigned idx);
        void selectAdvKeyPressVisKey(unsigned idx);

        void initCollectionNames();
        void buildMenuTabs();
        void buildParametersMap();
        void buildParameterLines();
        void buildAdvKeyPressVisKeys();
        
        void positionMenuLines();

        void moveSliderBarButtons(float offset);
        void moveSliderBarMouse(sf::Vector2i mousePos);
        void returnViewInBounds();

        void updateSaveStatsStrings();


    private:
        sf::RenderWindow mWindow;

        FontHolder mFonts;
        TextureHolder mTextures;

        sf::View mView;
        const float mScrollSpeed;
        float mHighViewBounds;

        sf::RectangleShape mSliderBar;
        const sf::Color mSliderBarDefaultColor;
        const sf::Color mSliderBarAimedColor;
        const sf::Color mSliderBarPressedColor;

        unsigned mSelectedTab;
        std::vector<std::unique_ptr<GfxParameter>> mTabs;
        unsigned mSelectedAdvKeyPressVisKey;
        std::vector<std::unique_ptr<GfxParameter>> mAdvKeyPressVisKeys;
        std::vector<float> mBounds;
        sf::RectangleShape mTabsBackground;

        std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> mParameters;
        std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> mParameterLines;
        std::vector<std::string> mCollectionNames;
        ChangedParametersQueue mChangedParametersQueue;

        static bool paramValWasChanged;
        static std::string mProgramVersion;
};