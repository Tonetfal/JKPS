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


class Menu 
{
    public:
        Menu();

        void handleEvent(sf::Event event);
        void handleOwnEvent();
        void update();
        void render();

        Settings &getSettings();

        LogicalParameter &getParameter(LogicalParameter::ID id);
        ChangedParametersQueue &getChangedParametersQueue();

        void saveConfig();
        void requestFocus();


    private:
        void loadFonts();
        void loadTextures();
        void buildParametersMap();
        void buildParameterLines();

        void moveSliderBarButtons(float offset);
        void moveSliderBarMouse(sf::Vector2i mousePos);
        void returnViewInBounds();


    private:
        sf::RenderWindow mWindow;

        FontHolder mFonts;
        TextureHolder mTextures;

        sf::View mView;
        const float mScrollSpeed;
        float mHighViewBounds, mLowViewBounds;

        Settings mSettings;

        sf::RectangleShape mSliderBar;
        const sf::Color mSliderBarDefaultColor;
        const sf::Color mSliderBarAimedColor;
        const sf::Color mSliderBarPressedColor;

        std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> mParameters;
        std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> mParameterLines;
        ChangedParametersQueue mChangedParametersQueue;

        static bool paramValWasChanged;
        static std::string mProgramVersion;
};