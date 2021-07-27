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
#include <climits>

class Settings
{
    public:
                                    Settings();
        void                        handleEvent(sf::Event event);
        void                        update();

        void                        setWindowReference(sf::RenderWindow& window);

        void                        changeChangeability();
        void                        setChangeabilityPosition();
        bool                        wasButtonAmountChanged();

        void                        saveSettings();

        static sf::Keyboard::Key    getButtonToChange();
        static int                  getButtonToChangeIndex();


    private:
        void                        addKey();
        void                        removeKey();
        void                        changeKey(sf::Keyboard::Key newKey);
        sf::Keyboard::Key           getDefaultKey(size_t index);
        bool                        isThereSameKey(sf::Keyboard::Key key, size_t& whichOne, size_t indexToIgnore);
        bool                        isInRange(size_t index);


    private:
        std::string                 findParameter(std::string parameterName);

        void                        setupColor(sf::Color& color, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        template <typename T>
        void                        setupVector(T& vector, int limitMin, int limitMax, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        template <typename T>
        void                        setupDigitParameter(T& parameter, int limitMin, int limitMax, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupFilePathParameter(std::string& parameter, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupKey(std::vector<sf::Keyboard::Key>& keys, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupMouseButton(std::vector<sf::Mouse::Button>& mouseButtons, const std::string information, const std::string parameterName, std::ofstream& errorLog);
        void                        setupBoolParameter(bool& parameter, const std::string information, const std::string parameterName, std::ofstream& errorLog);

        void                        createDefaultConfig();
        void                        writeKeys(std::ofstream& ofConfig);

    public:
        const static std::size_t    mFramesPerSecond;

        // [Keys] [Mouse]
        static std::vector<sf::Keyboard::Key> Keys;
        static std::vector<sf::Mouse::Button> MouseButtons;

        // Non config parameters
        static std::size_t          ButtonAmount;

        // [Statistics text]
        static float                StatisticsDistance;
        static float                SpaceBetweenButtonsAndStatistics;
        static std::string          StatisticsFontPath;
        static sf::Color            StatisticsTextColor;
        static std::size_t          StatisticsTextCharacterSize;
        static bool                 StatisticsBold;
        static bool                 StatisticsItalic;
        static bool                 ShowStatisticsText;
        static bool                 ShowKPS;
        static bool                 ShowMaxKPS;
        static bool                 ShowTotal;
        static bool                 ShowBPMText;

        // [Button text]
        static std::string          KeyCountersFontPath;
        static sf::Color            KeyCountersTextColor;
        static std::size_t          KeyCountersTextCharacterSize;
        static float                KeyCounterWidth;
        static float                KeyCounterHeight;
        static float                KeyCountersHorizontalBounds;
        static float                KeyCountersVerticalBounds;
        static bool                 KeyCountersBold;
        static bool                 KeyCountersItalic;
        static bool                 ShowSetKeysText;
        static bool                 ShowKeyCountersText;

        // [Button graphics]
        static float                ButtonDistance;
        static std::string          ButtonTexturePath;
        static sf::Vector2u         ButtonTextureSize;
        static sf::Color            ButtonTextureColor;

        // [Animation graphics]
        static int                  AnimationStyle;
        static std::string          AnimationTexturePath;
        static std::size_t          AnimationVelocity;
        static sf::Vector2f         AnimationScale;
        static sf::Color            AnimationColor;
        static float                AnimationOffset;

        // Non config parameters
        static sf::Color            AnimationOnClickTransparency;
        static sf::Vector2f         ScaledAnimationScale;

        // [Background]
        static std::string          BackgroundTexturePath;
        static sf::Color            BackgroundColor;
        static bool                 ScaleBackground;

        // [Edit mode]
        static sf::Color            HighlightedKeyColor;

        // [Main window]
        static bool                 WindowTitleBar;
        static unsigned             WindowBonusSizeTop;
        static unsigned             WindowBonusSizeBottom;
        static unsigned             WindowBonusSizeLeft;
        static unsigned             WindowBonusSizeRight;

        // [KPS window]
        static bool                 KPSWindowEnabledFromStart;
        static sf::Vector2u         KPSWindowSize;
        static sf::Color            KPSBackgroundColor;
        static sf::Color            KPSTextColor;
        static sf::Color            KPSNumberColor;
        static std::string          KPSWindowTextFontPath;
        static std::string          KPSWindowNumberFontPath;
        static float                KPSWindowTopPadding;
        static float                KPSWindowDistanceBetween;
        static unsigned             KPSTextSize;
        static unsigned             KPSNumberSize;

        // [Theme developer]
        static int                  ValueToMultiplyOnClick;

        static unsigned char*       KeyCountersDefaultFont;
        static unsigned char*       StatisticsDefaultFont;
        static unsigned char*       DefaultButtonTexture;
        static unsigned char*       DefaultAnimationTexture;
        static unsigned char*       DefaultBackgroundTexture;
        static unsigned char*       DefaultKPSWindowFont;

        // Hot keys
        static sf::Keyboard::Key    KeyToIncrease;
        static sf::Keyboard::Key    AltKeyToIncrease;
        static sf::Keyboard::Key    KeyToDecrease;
        static sf::Keyboard::Key    AltKeyToDecrease;
        static sf::Keyboard::Key    KeyToClear;
        static sf::Keyboard::Key    KeyExit;
        static sf::Keyboard::Key    KeyToOpenKPSWindow;
        static sf::Keyboard::Key    KeyToOpenMenuWindow;


    private:
        const size_t                minimumKeys;
        const size_t                maximumKeys;
        const size_t                minimumMouseButtons;
        const size_t                maximumMouseButtons;

        sf::RenderWindow*           mWindow;

        static bool                 mIsButtonSelected;
        static int                  mButtonToChangeIndex;
        static sf::Keyboard::Key    mButtonToChange;
        static bool                 mButtonAmountChanged;
};