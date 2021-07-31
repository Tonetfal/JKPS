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

        void moveScrollCursorButtons(float offset);
        void moveScrollCursorMouse(sf::Vector2i mousePos);
        void returnViewInBounds();


    private:
        sf::RenderWindow mWindow;

        FontHolder mFonts;
        TextureHolder mTextures;

        sf::View mView;
        const float mScrollSpeed;
        float mHighViewBounds, mLowViewBounds;

        Settings mSettings;

        sf::RectangleShape mScrollCursor;
        const sf::Color mScrollCursorDefaultColor;
        const sf::Color mScrollCursorAimedColor;
        const sf::Color mScrollCursorPressedColor;

        std::map<LogicalParameter::ID, std::shared_ptr<LogicalParameter>> mParameters;
        std::map<ParameterLine::ID, std::shared_ptr<ParameterLine>> mParameterLines;
        ChangedParametersQueue mChangedParametersQueue;

        static bool paramValWasChanged;
        static std::string mProgramVersion;
};