#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Color.hpp>

#include "GfxStatisticsLine.hpp"

#include <string>
#include <array>


namespace Settings
{
    const size_t SupportedAdvancedKeysNumber = 20ul;
    const size_t OldSupportedAdvancedKeysNumber = 15ul;
    template <typename T>
    using Container = std::array<T, SupportedAdvancedKeysNumber>;
    template <typename T>
    using StatisticsContainer = std::array<T, GfxStatisticsLine::StatisticsIdCounter>;

    // [Statistics text]
    extern float StatisticsTextDistance;
    extern sf::Vector2f StatisticsTextPosition;
    extern sf::Vector2f StatisticsTextValuePosition;
    extern bool StatisticsTextCenterOrigin;
    extern std::string StatisticsTextFontPath;
    extern sf::Color StatisticsTextColor;
    extern unsigned StatisticsTextCharacterSize;
    extern unsigned StatisticsTextOutlineThickness;
    extern sf::Color StatisticsTextOutlineColor;
    extern bool StatisticsTextBold;
    extern bool StatisticsTextItalic;
    extern bool ShowStatisticsText;
	extern bool ShowStatisticsKPS;
	extern bool ShowStatisticsMaxKPS;
    extern bool ShowStatisticsTotal;
    extern bool ShowStatisticsBPM;

    extern bool StatisticsTextAdvancedMode;
    extern StatisticsContainer<sf::Vector2f> StatisticsTextAdvPosition;
    extern StatisticsContainer<sf::Vector2f> StatisticsTextAdvValuePosition;
    extern StatisticsContainer<bool> StatisticsTextAdvCenterOrigin;
    extern StatisticsContainer<sf::Color> StatisticsTextAdvColor;
    extern StatisticsContainer<unsigned> StatisticsTextAdvCharacter;
    extern StatisticsContainer<bool> StatisticsTextAdvBold;
    extern StatisticsContainer<bool> StatisticsTextAdvItalic;
    extern std::string StatisticsKPSText;
    extern std::string StatisticsKPS2Text;
    extern std::string StatisticsTotalText;
    extern std::string StatisticsBPMText;

    // [Button text]
    extern std::string ButtonTextFontPath;
    extern sf::Color ButtonTextColor;
    extern unsigned ButtonTextCharacterSize;
    extern unsigned ButtonTextOutlineThickness;
    extern sf::Color ButtonTextOutlineColor;
    extern sf::Vector2f ButtonTextPosition;
    extern bool ButtonTextBoundsToggle;
    extern sf::Vector2f ButtonTextBounds;
    extern bool ButtonTextIgnoreBtnMovement;
    extern bool ButtonTextBold;
    extern bool ButtonTextItalic;
    extern bool ButtonTextShowVisualKeys;
    extern sf::Vector2f ButtonTextVisualKeysTextPosition;
    extern bool ButtonTextShowTotal;
    extern sf::Vector2f ButtonTextTotalTextPosition;
    extern bool ButtonTextShowKPS;
    extern sf::Vector2f ButtonTextKPSTextPosition;
    extern bool ButtonTextShowBPM;
    extern sf::Vector2f ButtonTextBPMTextPosition;

    // [Buttons text advanced settings]
    extern bool ButtonTextSepPosAdvancedMode;
    extern Container<sf::Vector2f> ButtonTextAdvVisualKeysTextPosition;
    extern Container<sf::Vector2f> ButtonTextAdvTotalTextPosition;
    extern Container<sf::Vector2f> ButtonTextAdvKPSTextPosition;
    extern Container<sf::Vector2f> ButtonTextAdvBPMTextPosition;
    
    extern bool ButtonTextAdvancedMode;
    extern Container<sf::Color> ButtonTextAdvColor;
    extern Container<unsigned> ButtonTextAdvCharacterSize;
    extern Container<unsigned> ButtonTextAdvOutlineThickness;
    extern Container<sf::Color> ButtonTextAdvOutlineColor;
    extern Container<sf::Vector2f> ButtonsTextAdvPosition;
    extern Container<sf::Vector2f> ButtonTextAdvBounds;
    extern Container<bool> ButtonTextAdvBold;
    extern Container<bool> ButtonTextAdvItalic;

    // [Button graphics]
    extern float GfxButtonDistance;
    extern std::string GfxButtonTexturePath;
    extern sf::Vector2u GfxButtonTextureSize;
    extern sf::Color GfxButtonTextureColor;

    // [Button graphics advanced settings]
    extern bool GfxButtonAdvancedMode;
    extern Container<sf::Vector2f> GfxButtonsBtnPositions;
    extern Container<sf::Vector2f> GfxButtonsSizes;
    extern Container<sf::Color> GfxButtonsColor;

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

    // [Key press visualization]
    extern bool KeyPressVisToggle;
    extern float KeyPressVisSpeed;
    extern float KeyPressVisRotation;
    extern float KeyPressVisFadeLineLen;
    extern sf::Vector2f KeyPressVisOrig;
    extern sf::Color KeyPressVisColor;
    extern float KeyPressWidthScale;
    extern float KeyPressFixedHeight;

    // [Key press visualization advanced settings]
    extern bool KeyPressVisAdvSettingsMode;
    extern Container<float> KeyPressVisAdvSpeed;
    extern Container<float> KeyPressVisAdvRotation;
    extern Container<float> KeyPressVisAdvFadeLineLen;
    extern Container<sf::Vector2f> KeyPressVisAdvOrig;
    extern Container<sf::Color> KeyPressVisAdvColor;
    extern Container<float> KeyPressAdvWidthScale;
    extern Container<float> KeyPressAdvFixedHeight;

    // [Other]
    extern bool SaveStats;
    extern bool ShowOppOnAlt;
    extern unsigned ButtonPressMultiplier;

    // Default assets
    extern unsigned char* KeyCountersDefaultFont;
    extern unsigned char* StatisticsDefaultFont;
    extern unsigned char* DefaultButtonTexture;
    extern unsigned char* DefaultAnimationTexture;
    extern unsigned char* DefaultBackgroundTexture;
    extern unsigned char* DefaultGreenscreenBackgroundTexture;
    extern unsigned char* DefaultKPSWindowFont;
    extern unsigned char* KeyPressVisTexture;

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
    extern sf::Keyboard::Key KeyToOpenGraphWindow;

    // Saved parameters
    extern float MaxKPS;
    extern unsigned Total;
    extern Container<unsigned> KeysTotal;
}
