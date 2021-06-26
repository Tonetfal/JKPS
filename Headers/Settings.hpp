#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "StringHelper.hpp"
#include "DefaultFiles.hpp"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>

class Settings
{
    public:
                                    Settings();
        void                        handleEvent(sf::Event event);
        void                        update();
        void                        draw();

        void                        setWindowReference(sf::RenderWindow& window);

        void                        changeChangeability();
        void                        setChangeabilityPosition();
        bool                        wasButtonAmountChanged();

        void                        saveSettings();

        sf::Keyboard::Key           getButtonToChange();
        int                         getButtonToChangeIndex();


    private:
        bool                        isInRange(size_t index);


    private:
        std::string                 findParameter(std::string parameterName);

        void                        setDefaultKey(size_t index);

        void                        setupColor(sf::Color& color, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        template <typename T>
        void                        setupVector(T& vector, int limitMin, int limitMax, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        template <typename T>
        void                        setupDigitParameter(T& parameter, int limitMin, int limitMax, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupFilePathParameter(std::string& parameter, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupKey(std::vector<sf::Keyboard::Key>& keys, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupMouseButton(std::vector<sf::Mouse::Button>& mouseButtons, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupBoolParameter(bool& parameter, const std::string information, const std::string parameterName, std::ofstream& errorLog);

        bool                        isThereSameKey(sf::Keyboard::Key key, size_t& whichOne, size_t indexToIgnore);

        void                        createDefaultConfig();

        void                        writeKeys(std::ofstream& ofConfig);

    public:
        const static std::size_t    mFramesPerSecond;

        // [Keys] [Mouse]
        static std::vector<sf::Keyboard::Key> Keys;
        static std::vector<sf::Mouse::Button> MouseButtons;
        static std::size_t          KeyAmount;
        static std::size_t          MouseButtonAmount;
        static std::size_t          ButtonAmount;

        // [Statistics]
        static std::string          StatisticsFontPath;
        static std::string          KeyCountersFontPath;
        static sf::Color            StatisticsTextColor;
        static sf::Color            KeyCountersTextColor;
        static std::size_t          StatisticsTextCharacterSize;
        static std::size_t          KeyCountersTextCharacterSize;
        static bool                 ShowStatisticsText;
        static bool                 ShowSetKeysText;
        static bool                 ShowKeyCountersText;
        static bool                 ShowBPMText;

        // [Spacing]
        static float                ButtonDistance;
        static float                SpaceBetweenButtonsAndStatistics;
        static float                SpaceOnStatisticsRight;

        // [Button graphics]
        static std::string          ButtonTexturePath;
        static std::string          AnimationTexturePath;
        static sf::Vector2u         ButtonTextureSize;
        static sf::Color            AnimationTextureColor;
        static sf::Color            ButtonTextureColor;
        static std::size_t          AnimationVelocity;
        static sf::Vector2f         AnimationScale;
        static sf::Vector2f         ScaledAnimationScale;
        static sf::Color            AnimationColor;
        static sf::Color            AnimationOnClickTransparency;

        // [Background]
        static std::string          BackgroundTexturePath;
        static sf::Color            BackgroundColor;

        static bool                 IsChangeable;
        // [Other]
        static sf::Color            HighlightedKeyColor;
        static sf::Color            AlertColor;

        static unsigned char*       KeyCountersDefaultFont;
        static unsigned char*       StatisticsDefaultFont;
        static unsigned char*       DefaultButtonTexture;
        static unsigned char*       DefaultAnimationTexture;
        static unsigned char*       DefaultBackgroundTexture;

        static sf::Keyboard::Key    KeyToIncrease;
        static sf::Keyboard::Key    AltKeyToIncrease;
        static sf::Keyboard::Key    KeyToDecrease;
        static sf::Keyboard::Key    AltKeyToDecrease;


    private:
        const std::string           configPath;
        const std::string           errorLogPath;
        const std::string           tmpConfigPath;

        const size_t                minimumKeys;
        const size_t                maximumKeys;
        const size_t                minimumMouseButtons;
        const size_t                maximumMouseButtons;

        sf::RenderWindow*           mWindow;

        sf::CircleShape             mIsChangeableAlert;
        bool                        mButtonAmountChanged;

        sf::Keyboard::Key           mButtonToChange;
        int                         mButtonToChangeIndex;
};