#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "GfxParameter.hpp"
#include "LogicalParameter.hpp"
#include "ResourceIdentifiers.hpp"
#include "ColorButton.hpp"
#include "Palette.hpp"

#include <memory>
#include <iostream>
#include <thread>


class Palette;

namespace sf
{
    class Event;
    class RenderWindow;
}

class ParameterLine : public sf::Drawable, public sf::Transformable, public std::enable_shared_from_this<ParameterLine>
{
    public:
        enum class ID
        {
            StatTextColl,
            StatTextDist,
            StatPos,
            StatTextFont,
            StatTextClr,
            StatTextChSz,
            StatTextBold,
            StatTextItal,
            StatTextShow,
            StatTextShowKPS,
            StatTextShowTotal,
            StatTextShowBPM,
            StatTextKPSText,
            StatTextKPS2Text,
            StatTextTotalText,
            StatTextBPMText,
            StatTextMty,

            BtnTextColl,
            BtnTextFont,
            BtnTextClr,
            BtnTextChSz,
            BtnTextPosition,
            BtnTextBounds,
            BtnTextBold,
            BtnTextItal,
            BtnTextShowVisKeys,
            BtnTextShowKeyCtrs,
            BtnTextShowTot,
            BtnTextShowKps,
            BtnTextShowBpm,
            BtnTextMty,

            BtnGfxColl,
            BtnGfxDist,
            BtnGfxTxtr,
            BtnGfxTxtrSz,
            BtnGfxTxtrClr,
            BtnGfxMty,

            AnimGfxColl,
            AnimGfxLight,
            AnimGfxPress,
            AnimGfxTxtr,
            AnimGfxVel,
            AnimGfxScl,
            AnimGfxClr,
            AnimGfxOffset,
            AnimGfxMty,

            BgColl,
            BgTxtr,
            BgClr,
            BgScale,
            BgMty,

            MainWndwColl,
            MainWndwTitleBar,
            MainWndwTop,
            MainWndwBot,
            MainWndwLft,
            MainWndwRght,
            MainWndwMty,

            KPSWndwColl,
            KPSWndwEn,
            KPSWndwSz,
            KPSWndwTxtChSz,
            KPSWndwNumChSz,
            KPSWndwBgClr,
            KPSWndwTxtClr,
            KPSWndwNumClr,
            KPSWndwTxtFont,
            KPSWndwNumFont,
            KPSWndwTopPadding,
            KPSWndwDistBtw,
            KPSWndwMty,

            ThemeDevColl,
            ThemeDevMultpl,
            ThemeDevMty,

            InfoColl,
            Info1,
            Info2,
            Info3,
            Info4,
            InfoMty,

            HotkeyColl,
            HotKey1,
            HotKey2,
            HotKey3,
            HotKey4,
            HotKey5,
            HotKey6,
            HotKey7,
            HotKey8,
            HotKey9,
            HotKey10,
            HotKey11,
            HotKey12,
            HotKey13,

            LastLine,

        };


    public:
        ParameterLine(
            std::shared_ptr<LogicalParameter> parameter,
            const FontHolder &fonts, 
            const TextureHolder &textures,
            sf::RenderWindow &window);

        void handleEvent(sf::Event event);
        void processInput();
        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

        bool resetState();

        const std::shared_ptr<LogicalParameter> getParameter() const;
        LogicalParameter::Type getType() const;

        static void setColor(sf::Color color);
        static ParameterLine::ID parIdToParLineId(LogicalParameter::ID id);
        static bool isCollection(ParameterLine::ID id);
        static void deselectValue();
        static bool isValueSelected();
        static bool resetRefreshState();


    private:
        // Clicks with keyboard to modify value of buttons
        void handleValueModEvent(sf::Event event);
        void handleButtonsInteractionEvent(sf::Event event);

        bool tabulation(sf::Keyboard::Key key);
        void selectRgbCircle(sf::Mouse::Button button, sf::Vector2f mousePos);

        void buildButtons(const std::string &valueStr, const FontHolder &fonts, const TextureHolder &textures);
        void buildLimits(const FontHolder &fonts);

        void select(std::shared_ptr<GfxParameter> ptr);
        void deselect();
        bool isSelectedValHere() const;
        bool isItSelectedLine(const std::shared_ptr<ParameterLine> val) const;
        static void setCursorPos();

        sf::Color pickColor(LogicalParameter::Type type) const;
        sf::Color lineToColor(const std::shared_ptr<ParameterLine> linePtr) const;

        bool checkLimits(float check) const;
        void runThread(std::string &curVal, const std::string &prevVal);
        static void warningVisualization(bool *isRunning, ParameterLine *parLine);


    private:
        sf::RenderWindow &mWindow;

        const LogicalParameter::Type mType;
        sf::RectangleShape mRectLine;
        sf::Text mParameterName;
        sf::Text mLimits;
        std::shared_ptr<LogicalParameter> mParameter;
        std::vector<std::shared_ptr<GfxParameter>> mParameterValues;
        std::unique_ptr<ColorButton> mColorButtonP;

        static sf::RectangleShape mCursor;
        static std::shared_ptr<LogicalParameter> mSelectedParameter;
        static std::shared_ptr<ParameterLine> mSelectedLine;
        static std::shared_ptr<GfxParameter> mSelectedValue;
        static int mSelectedValueIndex;
        bool paramValWasChanged;
        std::thread mWarningTh;
        bool mIsThRunning;

        static Palette mPalette;
        static bool mRefresh;
};