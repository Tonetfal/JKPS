#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "LogicalKey.hpp"
#include "LogicalButton.hpp"
#include "GraphicalParameter.hpp"

#include <memory>
#include <array>

namespace sf
{
    class Event;
}


class KeySelector
{
    private:
        enum Buttons
        {
            RealKeyButton,
            VisualKeyButton,
            AcceptButton,
            ButtonsCount
        };

        enum KeyType
        {
            Keyboard,
            Mouse,
        };


    public:
        KeySelector();

        void handleOwnInput();
        void render();

        void setKey(LogicalKey *key, LogicalButton *button);
        void openWindow();
        void closeWindow();
        bool isOpen();

        void setMainWindowPointer(sf::RenderWindow *mainWindow);


    private:
        void handleButtonInteractionEvent(sf::Event event);
        // Clicks with keyboard to modify value of buttons
        void handleButtonModificationEvent(sf::Event event);

        void select(GraphicalParameter *ptr);
        void deselect();
        void saveKey();
        void setCursorPos();
        void resetVisualKeyGfxButton();
        

    private:
        sf::Vector2u mWindowSize;
        sf::RenderWindow mWindow;
        sf::RenderWindow *mMainWindow;

        sf::Font mFont;
        std::array<std::unique_ptr<GraphicalParameter>, ButtonsCount> mButtons;

        static sf::RectangleShape mCursor;
        static int mSelectedBtnTextIndex;
        static GraphicalParameter *mSelectedBtn;
        static const std::string mDefaultVisualKeyStr;
        static const sf::Color mDefaultVisualKeyColor;

        LogicalKey *mKey;
        LogicalButton *mButton;
        KeyType mKeyType;
};