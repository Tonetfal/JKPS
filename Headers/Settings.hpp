#pragma once

#include <SFML/Graphics/Color.hpp>

#include "StringHelper.hpp"
#include "KeySelector.hpp"

#include <vector>
#include <string>

namespace sf
{
    class Event;
    class RenderWindow;
}


class Settings
{
    public:
                                    Settings();
        void                        handleEvent(sf::Event event);
        void                        update();

        void                        setWindowReference(sf::RenderWindow& window);
        void                        buildKeySelector();

        void                        setChangeabilityPosition();
        static bool                 wasButtonChanged();
        static bool                 wasButtonAmountChanged();

        static sf::Keyboard::Key    getButtonToChange();
        static int                  getButtonToChangeIndex();

        static void                 requestToReloadAssets();
        bool                        resetReloadAssetsRequest() const;


    private:
        void                        changeKeysAmount(sf::Keyboard::Key clickedKey);
        void                        selectButton();
        void                        addKeyboardKey();
        void                        removeKeyboardKey();
        void                        addMouseButton();
        void                        removeMouseButton();
        bool                        isPressPerformedOnButton(unsigned &buttonIndex);            
        bool                        isMouseInRange(unsigned index);


    public:
        const static std::size_t    mFramesPerSecond;

        // [Keys] [Mouse]
        static std::vector<std::unique_ptr<LogicalKey>> LogicalKeys;
        static std::vector<std::unique_ptr<LogicalButton>> LogicalButtons;

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
        static std::string          StatisticsKPSText;
        static std::string          StatisticsKPS2Text;
        static std::string          StatisticsMaxKPSText;
        static std::string          StatisticsTotalText;
        static std::string          StatisticsBPMText;

        // [Button text]
        static std::string          KeyCountersFontPath;
        static sf::Color            KeyCountersTextColor;
        static std::size_t          KeyCountersTextCharacterSize;
        static sf::Vector2f         KeyCounterPosition;
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

        // Non config parameters
        static bool                 isGreenscreenSet;

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
        static unsigned char*       DefaultGreenscreenBackgroundTexture;
        static unsigned char*       DefaultKPSWindowFont;

        // Hot keys
        static sf::Keyboard::Key    KeyToIncreaseKeys;
        static sf::Keyboard::Key    AltKeyToIncreaseKeys;
        static sf::Keyboard::Key    KeyToDecreaseKeys;
        static sf::Keyboard::Key    AltKeyToDecreaseKeys;
        static sf::Keyboard::Key    KeyToIncreaseButtons;
        static sf::Keyboard::Key    KeyToDecreaseButtons;
        static sf::Keyboard::Key    KeyToClear;
        static sf::Keyboard::Key    KeyExit;
        static sf::Keyboard::Key    KeyToOpenKPSWindow;
        static sf::Keyboard::Key    KeyToOpenMenuWindow;


    private:
        sf::RenderWindow*           mWindow;
        std::unique_ptr<KeySelector> mKeySelector;

        static bool                 mIsButtonSelected;
        static int                  mButtonToChangeIndex;
        static sf::Keyboard::Key    mButtonToChange;
        static bool                 mButtonAmountChanged;
        static bool                 mKeyWasChanged;
        static bool                 mReloadAssetsRequest;
};