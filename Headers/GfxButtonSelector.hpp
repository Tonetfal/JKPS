#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "LogKey.hpp"
#include "GfxParameter.hpp"

#include <memory>
#include <array>

namespace sf
{
    class Event;
}


class GfxButtonSelector
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
        GfxButtonSelector();

        void handleOwnInput();
        void render();

        void setKey(LogKey *logKey);
        void openWindow(sf::Vector2i spawnPosition);
        bool isOpen();
        
        static bool isCharacter(sf::Keyboard::Key key);


    private:
        void handleButtonInteractionEvent(sf::Event event);
        // Clicks with keyboard to modify value of buttons
        void handleButtonModificationEvent(sf::Event event);

        void select(GfxParameter *ptr);
        void deselect();
        void saveKey();
        void setCursorPos();
        void resetVisualKeyGfxButton(const std::string &str1, const std::string &str2);
        

    private:
        sf::Vector2u mWindowSize;
        sf::RenderWindow mWindow;

        sf::Font mFont;
        std::array<std::unique_ptr<GfxParameter>, ButtonsCount> mButtons;

        static sf::RectangleShape mCursor;
        static int mSelectedBtnTextIndex;
        static GfxParameter *mSelectedBtn;
        static const std::string mDefaultVisualKeyStr;
        static const sf::Color mDefaultVisualKeyColor;

        LogKey *mLogKey;
        KeyType mKeyType;
};