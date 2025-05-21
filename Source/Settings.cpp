#include "../Headers/Settings.hpp"
#include "../Headers/DefaultFiles.hpp"


namespace Settings
{

// [Statistics text]
float StatisticsTextDistance;
sf::Vector2f StatisticsTextPosition;
sf::Vector2f StatisticsTextValuePosition;
bool StatisticsTextCenterOrigin;
std::string StatisticsTextFontPath;
sf::Color StatisticsTextColor;
unsigned StatisticsTextCharacterSize;
unsigned StatisticsTextOutlineThickness;
sf::Color StatisticsTextOutlineColor;
bool StatisticsTextBold;
bool StatisticsTextItalic;
bool ShowStatisticsText;
bool ShowStatisticsKPS;
bool ShowStatisticsMaxKPS;
bool ShowStatisticsTotal;
bool ShowStatisticsBPM;

bool StatisticsTextAdvancedMode;
StatisticsContainer<sf::Vector2f> StatisticsTextAdvPosition;
StatisticsContainer<sf::Vector2f> StatisticsTextAdvValuePosition;
StatisticsContainer<bool> StatisticsTextAdvCenterOrigin;
StatisticsContainer<sf::Color> StatisticsTextAdvColor;
StatisticsContainer<unsigned> StatisticsTextAdvCharacter;
StatisticsContainer<bool> StatisticsTextAdvBold;
StatisticsContainer<bool> StatisticsTextAdvItalic;
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
bool ButtonTextBoundsToggle;
sf::Vector2f ButtonTextBounds;
bool ButtonTextIgnoreBtnMovement;
bool ButtonTextBold;
bool ButtonTextItalic;
bool ButtonTextShowVisualKeys;
sf::Vector2f ButtonTextVisualKeysTextPosition;
bool ButtonTextShowTotal;
sf::Vector2f ButtonTextTotalTextPosition;
bool ButtonTextShowKPS;
sf::Vector2f ButtonTextKPSTextPosition;
bool ButtonTextShowBPM;
sf::Vector2f ButtonTextBPMTextPosition;

// [Buttons text advanced settings]
bool ButtonTextSepPosAdvancedMode;
Container<sf::Vector2f> ButtonTextAdvVisualKeysTextPosition;
Container<sf::Vector2f> ButtonTextAdvTotalTextPosition;
Container<sf::Vector2f> ButtonTextAdvKPSTextPosition;
Container<sf::Vector2f> ButtonTextAdvBPMTextPosition;

bool ButtonTextAdvancedMode;
Container<sf::Color> ButtonTextAdvColor;
Container<unsigned> ButtonTextAdvCharacterSize;
Container<unsigned> ButtonTextAdvOutlineThickness;
Container<sf::Color> ButtonTextAdvOutlineColor;
Container<sf::Vector2f> ButtonsTextAdvPosition;
Container<sf::Vector2f> ButtonTextAdvBounds;
Container<bool> ButtonTextAdvIgnoreBtnMovement;
Container<bool> ButtonTextAdvBold;
Container<bool> ButtonTextAdvItalic;

// [Button graphics]
float GfxButtonDistance;
std::string GfxButtonTexturePath;
sf::Vector2u GfxButtonTextureSize;
sf::Color GfxButtonTextureColor;
bool GfxButtonAdvancedMode;
Container<sf::Vector2f> GfxButtonsBtnPositions;
Container<sf::Vector2f> GfxButtonsSizes;
Container<sf::Color> GfxButtonsColor;

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
unsigned RenderUpdateFrequency;
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

// [Key press visualization]
bool KeyPressVisToggle;
float KeyPressVisSpeed;
float KeyPressVisRotation;
float KeyPressVisFadeLineLen;
sf::Vector2f KeyPressVisOrig;
sf::Color KeyPressVisColor;
float KeyPressWidthScale;
float KeyPressFixedHeight;

// [Key press visualization advanced settings]
bool KeyPressVisAdvSettingsMode;
Container<float> KeyPressVisAdvSpeed;
Container<float> KeyPressVisAdvRotation;
Container<float> KeyPressVisAdvFadeLineLen;
Container<sf::Vector2f> KeyPressVisAdvOrig;
Container<sf::Color> KeyPressVisAdvColor;
Container<float> KeyPressAdvWidthScale;
Container<float> KeyPressAdvFixedHeight;

// [Other]
bool SaveStats;
bool ShowOppOnAlt;
unsigned ButtonPressMultiplier;

// Default assets
unsigned char* StatisticsDefaultFont = MainProgramFont;
unsigned char* KeyCountersDefaultFont = MainProgramFont;
unsigned char* DefaultButtonTexture = ButtonTexture;
unsigned char* DefaultAnimationTexture = AnimationTexture;
unsigned char* DefaultBackgroundTexture = BackgroundTexture;
unsigned char* DefaultGreenscreenBackgroundTexture = GreenscreenTexture;
unsigned char* DefaultKPSWindowFont = KPSWindowDefaultFont;
unsigned char* KeyPressVisTexture = KeyPressVisTextureH;

// Hot keys
sf::Keyboard::Key KeyToIncreaseKeys = sf::Keyboard::Equal;
sf::Keyboard::Key AltKeyToIncreaseKeys = sf::Keyboard::Add;
sf::Keyboard::Key KeyToDecreaseKeys = sf::Keyboard::Dash;
sf::Keyboard::Key AltKeyToDecreaseKeys = sf::Keyboard::Subtract;
sf::Keyboard::Key KeyToIncreaseButtons = sf::Keyboard::Period;
sf::Keyboard::Key KeyToDecreaseButtons = sf::Keyboard::Comma;
sf::Keyboard::Key KeyToReset = sf::Keyboard::X;
sf::Keyboard::Key KeyExit = sf::Keyboard::W;
sf::Keyboard::Key KeyToOpenKPSWindow = sf::Keyboard::K;
sf::Keyboard::Key KeyToOpenMenuWindow = sf::Keyboard::A;
sf::Keyboard::Key KeyToOpenGraphWindow = sf::Keyboard::G;

// Saved parameters
float MaxKPS;
unsigned Total;
Container<unsigned> KeysTotal;

} // !namespace Settings
