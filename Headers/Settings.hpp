#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Color.hpp>

#include "GfxStatisticsLine.hpp"

#include <string>
#include <array>


namespace Settings
{
    // [Statistics text]
    extern float StatisticsTextDistance;
    extern sf::Vector2f StatisticsTextPosition;
    extern std::string StatisticsTextFontPath;
    extern sf::Color StatisticsTextColor;
    extern unsigned StatisticsTextCharacterSize;
    extern bool StatisticsTextBold;
    extern bool StatisticsTextItalic;
    extern bool ShowStatisticsText;
    extern bool ShowStatisticsKPS;
    extern bool ShowStatisticsTotal;
    extern bool ShowStatisticsBPM;

    extern bool StatisticsTextPositionsAdvancedMode;
    extern std::array<sf::Vector2f, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextPositions;
    extern bool StatisticsTextColorsAdvancedMode;
    extern std::array<sf::Color, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextColors;
    extern bool StatisticsTextChSzssAdvancedMode;
    extern std::array<unsigned, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextCharacterSizes;
    extern bool StatisticsTextBoldAdvancedMode;
    extern std::array<bool, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextBolds;
    extern bool StatisticsTextItalicAdvancedMode;
    extern std::array<bool, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextItalics;
    extern std::string StatisticsKPSText;
    extern std::string StatisticsKPS2Text;
    extern std::string StatisticsTotalText;
    extern std::string StatisticsBPMText;

    // [Button text]
    extern std::string ButtonTextFontPath;
    extern sf::Color ButtonTextColor;
    extern unsigned ButtonTextCharacterSize;
    extern sf::Vector2f ButtonTextPosition;
    extern sf::Vector2f ButtonTextBounds;
    extern bool ButtonTextBold;
    extern bool ButtonTextItalic;
    extern bool ButtonTextShowVisualKeys;
    extern bool ButtonTextShowTotal;
    extern bool ButtonTextShowKPS;
    extern bool ButtonTextShowBPM;
    extern bool ButtonTextSepPosAdvancedMode;
    extern sf::Vector2f ButtonVisualKeysTextPosition;
    extern sf::Vector2f ButtonTotalTextPosition;
    extern sf::Vector2f ButtonKPSTextPosition;
    extern sf::Vector2f ButtonBPMTextPosition;
    extern bool ButtonTextPosAdvancedMode;
    extern std::array<sf::Vector2f, 15> ButtonsTextPositions;

    // [Button graphics]
    extern float GfxButtonDistance;
    extern std::string GfxButtonTexturePath;
    extern sf::Vector2u GfxButtonTextureSize;
    extern sf::Color GfxButtonTextureColor;
    extern bool GfxButtonBtnPositionsAdvancedMode;
    extern std::array<sf::Vector2f, 15> GfxButtonsBtnPositions;
    extern bool GfxButtonSizesAdvancedMode;
    extern std::array<sf::Vector2f, 15> GfxButtonsSizes;

    // [Animation graphics]
    extern bool LightAnimation;
    extern bool PressAnimation;
    extern std::string AnimationTexturePath;
    extern unsigned AnimationFrames;
    extern sf::Vector2f AnimationScale;
    extern sf::Color AnimationColor;
    extern float AnimationOffset;

    // [Background]
    extern std::string BackgroundTexturePath;
    extern sf::Color BackgroundColor;
    extern bool ScaleBackground;

    // Non config parameters
    extern bool isGreenscreenSet;

    // [Main window]
    extern bool WindowTitleBar;
    extern int WindowBonusSizeTop;
    extern int WindowBonusSizeBottom;
    extern int WindowBonusSizeLeft;
    extern int WindowBonusSizeRight;

    // [KPS window]
    extern bool KPSWindowEnabledFromStart;
    extern sf::Vector2u KPSWindowSize;
    extern sf::Color KPSBackgroundColor;
    extern sf::Color KPSTextColor;
    extern sf::Color KPSNumberColor;
    extern std::string KPSWindowTextFontPath;
    extern std::string KPSWindowNumberFontPath;
    extern float KPSWindowTopPadding;
    extern float KPSWindowDistanceBetween;
    extern unsigned KPSTextSize;
    extern unsigned KPSNumberSize;

    // [Theme developer]
    extern int ButtonPressMultiplier;

    // Default assets
    extern unsigned char* KeyCountersDefaultFont;
    extern unsigned char* StatisticsDefaultFont;
    extern unsigned char* DefaultButtonTexture;
    extern unsigned char* DefaultAnimationTexture;
    extern unsigned char* DefaultBackgroundTexture;
    extern unsigned char* DefaultGreenscreenBackgroundTexture;
    extern unsigned char* DefaultKPSWindowFont;

    // Hot keys
    extern sf::Keyboard::Key KeyToIncreaseKeys;
    extern sf::Keyboard::Key AltKeyToIncreaseKeys;
    extern sf::Keyboard::Key KeyToDecreaseKeys;
    extern sf::Keyboard::Key AltKeyToDecreaseKeys;
    extern sf::Keyboard::Key KeyToIncreaseButtons;
    extern sf::Keyboard::Key KeyToDecreaseButtons;
    extern sf::Keyboard::Key KeyToReset;
    extern sf::Keyboard::Key KeyExit;
    extern sf::Keyboard::Key KeyToOpenKPSWindow;
    extern sf::Keyboard::Key KeyToOpenMenuWindow;

    // Saved parameters
    extern float MaxKPS;
    extern unsigned Total;
    extern std::array<unsigned, 15> KeysTotal;
};