#include "../Headers/Settings.hpp"
#include "../Headers/DefaultFiles.hpp"


namespace Settings
{

// [Statistics text]
float StatisticsTextDistance;
sf::Vector2f StatisticsTextPosition;
std::string StatisticsTextFontPath;
sf::Color StatisticsTextColor;
sf::Vector2f StatisticsTextValuePosition;
unsigned StatisticsTextCharacterSize;
unsigned StatisticsTextOutlineThickness;
sf::Color StatisticsTextOutlineColor;
bool StatisticsTextBold;
bool StatisticsTextItalic;
bool ShowStatisticsText;
bool ShowStatisticsKPS;
bool ShowStatisticsTotal;
bool ShowStatisticsBPM;

bool StatisticsTextPositionsAdvancedMode;
std::array<sf::Vector2f, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextPositions;
bool StatisticsTextValuePositionsAdvancedMode;
std::array<sf::Vector2f, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextValuePositions;
bool StatisticsTextColorsAdvancedMode;
std::array<sf::Color, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextColors;
bool StatisticsTextChSzssAdvancedMode;
std::array<unsigned, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextCharacterSizes;
bool StatisticsTextBoldAdvancedMode;
std::array<bool, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextBolds;
bool StatisticsTextItalicAdvancedMode;
std::array<bool, GfxStatisticsLine::StatisticsIdCounter> StatisticsTextItalics;
std::string StatisticsKPSText;
std::string StatisticsKPS2Text;
std::string StatisticsTotalText;
std::string StatisticsBPMText;

// [Button text]
std::string ButtonTextFontPath;
sf::Color ButtonTextColor;
unsigned ButtonTextCharacterSize;
unsigned ButtonTextOutlineThickness;
sf::Color ButtonTextOutlineColor;
sf::Vector2f ButtonTextPosition;
sf::Vector2f ButtonTextBounds;
bool ButtonTextBold;
bool ButtonTextItalic;
bool ButtonTextShowVisualKeys;
bool ButtonTextShowTotal;
bool ButtonTextShowKPS;
bool ButtonTextShowBPM;
bool ButtonTextSepPosAdvancedMode;
sf::Vector2f ButtonVisualKeysTextPosition;
sf::Vector2f ButtonTotalTextPosition;
sf::Vector2f ButtonKPSTextPosition;
sf::Vector2f ButtonBPMTextPosition;
bool ButtonTextPosAdvancedMode;
std::array<sf::Vector2f, 15> ButtonsTextPositions;

// [Button graphics]
float GfxButtonDistance;
std::string GfxButtonTexturePath;
sf::Vector2u GfxButtonTextureSize;
sf::Color GfxButtonTextureColor;
bool GfxButtonBtnPositionsAdvancedMode;
std::array<sf::Vector2f, 15> GfxButtonsBtnPositions;
bool GfxButtonSizesAdvancedMode;
std::array<sf::Vector2f, 15> GfxButtonsSizes;

// [Animation graphics]
bool LightAnimation;
bool PressAnimation;
std::string AnimationTexturePath;
unsigned AnimationFrames;
sf::Vector2f AnimationScale;
sf::Color AnimationColor;
float AnimationOffset;

// [Background]
std::string BackgroundTexturePath;
sf::Color BackgroundColor;
bool ScaleBackground;

// Non config parameters
bool isGreenscreenSet;

// [Main window]
bool WindowTitleBar;
int WindowBonusSizeTop;
int WindowBonusSizeBottom;
int WindowBonusSizeLeft;
int WindowBonusSizeRight;

// [KPS window]
bool KPSWindowEnabledFromStart;
sf::Vector2u KPSWindowSize;
sf::Color KPSBackgroundColor;
sf::Color KPSTextColor;
sf::Color KPSNumberColor;
std::string KPSWindowTextFontPath;
std::string KPSWindowNumberFontPath;
unsigned KPSTextSize;
unsigned KPSNumberSize;
float KPSWindowTopPadding;
float KPSWindowDistanceBetween;

// [Other]
bool SaveStats;
int ButtonPressMultiplier;

// Default assets
unsigned char* StatisticsDefaultFont = MainProgramFont;
unsigned char* KeyCountersDefaultFont = MainProgramFont;
unsigned char* DefaultButtonTexture = ButtonTexture;
unsigned char* DefaultAnimationTexture = AnimationTexture;
unsigned char* DefaultBackgroundTexture = BackgroundTexture;
unsigned char* DefaultGreenscreenBackgroundTexture = GreenscreenTexture;
unsigned char* DefaultKPSWindowFont = KPSWindowDefaultFont;

// Hot keys
sf::Keyboard::Key KeyToIncreaseKeys(sf::Keyboard::Equal);
sf::Keyboard::Key AltKeyToIncreaseKeys(sf::Keyboard::Add);
sf::Keyboard::Key KeyToDecreaseKeys(sf::Keyboard::Dash);
sf::Keyboard::Key AltKeyToDecreaseKeys(sf::Keyboard::Subtract);
sf::Keyboard::Key KeyToIncreaseButtons(sf::Keyboard::Period);
sf::Keyboard::Key KeyToDecreaseButtons(sf::Keyboard::Comma);
sf::Keyboard::Key KeyToReset(sf::Keyboard::X);
sf::Keyboard::Key KeyExit(sf::Keyboard::W);
sf::Keyboard::Key KeyToOpenKPSWindow(sf::Keyboard::K);
sf::Keyboard::Key KeyToOpenMenuWindow(sf::Keyboard::A);
sf::Keyboard::Key KeyToOpenGraphWindow(sf::Keyboard::G);

// Saved parameters
float MaxKPS;
unsigned Total;
std::array<unsigned, 15> KeysTotal;

} // !namespace Settings

