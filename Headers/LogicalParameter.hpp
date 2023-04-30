#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <cassert>
#include <string>


struct LogicalParameter
{
    public:
        enum class Type
        {
            Empty,
            Collection,
            Hint,
            Unsigned,
            Int,
            Bool,
            Float,
            String,
            StringPath,
            Color,
            VectorU,
            VectorI,
            VectorF,
        };

        enum class ID
        {
            StatTextDist,
            StatPos,
            StatValPos,
            StatTextCenterOrigin,
            StatTextFont,
            StatTextClr,
            StatTextChSz,
            StatTextOutThck,
            StatTextOutClr,
            StatTextBold,
            StatTextItal,
            StatTextShow,
            StatTextShowKPS,
            StatTextShowTotal,
            StatTextShowBPM,

            StatTextAdvMode,
            StatTextAdvPos1,
            StatTextAdvValPos1,
            StatTextAdvCenterOrigin1,
            StatTextAdvClr1,
            StatTextAdvChSz1,
            StatTextAdvBold1,
            StatTextAdvItal1,
            StatTextAdvPos2,
            StatTextAdvCenterOrigin2,
            StatTextAdvClr2,
            StatTextAdvValPos2,
            StatTextAdvChSz2,
            StatTextAdvBold2,
            StatTextAdvItal2,
            StatTextAdvPos3,
            StatTextAdvCenterOrigin3,
            StatTextAdvValPos3,
            StatTextAdvClr3,
            StatTextAdvChSz3,
            StatTextAdvBold3,
            StatTextAdvItal3,
            
            StatTextKPSText,
            StatTextKPS2Text,
            StatTextTotalText,
            StatTextBPMText,

            BtnTextFont,
            BtnTextClr,
            BtnTextChSz,
            BtnTextOutThck,
            BtnTextOutClr,
            BtnTextPosition,
            BtnTextBoundsToggle,
            BtnTextBounds,
            BtnTextIgnoreBtnMovement,
            BtnTextBold,
            BtnTextItal,
            BtnTextShowVisKeys,
            BtnTextVisPosition,
            BtnTextShowTot,
            BtnTextTotPosition,
            BtnTextShowKps,
            BtnTextKPSPosition,
            BtnTextShowBpm,
            BtnTextBPMPosition,

            BtnTextSepPosAdvMode,
            BtnTextAdvVisPosition1,
            BtnTextAdvTotPosition1,
            BtnTextAdvKPSPosition1,
            BtnTextAdvBPMPosition1,
            BtnTextAdvVisPosition2,
            BtnTextAdvTotPosition2,
            BtnTextAdvKPSPosition2,
            BtnTextAdvBPMPosition2,
            BtnTextAdvVisPosition3,
            BtnTextAdvTotPosition3,
            BtnTextAdvKPSPosition3,
            BtnTextAdvBPMPosition3,
            BtnTextAdvVisPosition4,
            BtnTextAdvTotPosition4,
            BtnTextAdvKPSPosition4,
            BtnTextAdvBPMPosition4,
            BtnTextAdvVisPosition5,
            BtnTextAdvTotPosition5,
            BtnTextAdvKPSPosition5,
            BtnTextAdvBPMPosition5,
            BtnTextAdvVisPosition6,
            BtnTextAdvTotPosition6,
            BtnTextAdvKPSPosition6,
            BtnTextAdvBPMPosition6,
            BtnTextAdvVisPosition7,
            BtnTextAdvTotPosition7,
            BtnTextAdvKPSPosition7,
            BtnTextAdvBPMPosition7,
            BtnTextAdvVisPosition8,
            BtnTextAdvTotPosition8,
            BtnTextAdvKPSPosition8,
            BtnTextAdvBPMPosition8,
            BtnTextAdvVisPosition9,
            BtnTextAdvTotPosition9,
            BtnTextAdvKPSPosition9,
            BtnTextAdvBPMPosition9,
            BtnTextAdvVisPosition10,
            BtnTextAdvTotPosition10,
            BtnTextAdvKPSPosition10,
            BtnTextAdvBPMPosition10,
            BtnTextAdvVisPosition11,
            BtnTextAdvTotPosition11,
            BtnTextAdvKPSPosition11,
            BtnTextAdvBPMPosition11,
            BtnTextAdvVisPosition12,
            BtnTextAdvTotPosition12,
            BtnTextAdvKPSPosition12,
            BtnTextAdvBPMPosition12,
            BtnTextAdvVisPosition13,
            BtnTextAdvTotPosition13,
            BtnTextAdvKPSPosition13,
            BtnTextAdvBPMPosition13,
            BtnTextAdvVisPosition14,
            BtnTextAdvTotPosition14,
            BtnTextAdvKPSPosition14,
            BtnTextAdvBPMPosition14,
            BtnTextAdvVisPosition15,
            BtnTextAdvTotPosition15,
            BtnTextAdvKPSPosition15,
            BtnTextAdvBPMPosition15,
            BtnTextAdvVisPosition16,
            BtnTextAdvTotPosition16,
            BtnTextAdvKPSPosition16,
            BtnTextAdvBPMPosition16,
            BtnTextAdvVisPosition17,
            BtnTextAdvTotPosition17,
            BtnTextAdvKPSPosition17,
            BtnTextAdvBPMPosition17,
            BtnTextAdvVisPosition18,
            BtnTextAdvTotPosition18,
            BtnTextAdvKPSPosition18,
            BtnTextAdvBPMPosition18,
            BtnTextAdvVisPosition19,
            BtnTextAdvTotPosition19,
            BtnTextAdvKPSPosition19,
            BtnTextAdvBPMPosition19,
            BtnTextAdvVisPosition20,
            BtnTextAdvTotPosition20,
            BtnTextAdvKPSPosition20,
            BtnTextAdvBPMPosition20,

            BtnTextPosAdvMode,
            BtnTextAdvClr1,
            BtnTextAdvChSz1,
            BtnTextAdvOutThck1,
            BtnTextAdvOutClr1,
            BtnTextAdvPosition1,
            BtnTextAdvBounds1,
            BtnTextAdvBold1,
            BtnTextAdvItal1,
            BtnTextAdvClr2,
            BtnTextAdvChSz2,
            BtnTextAdvOutThck2,
            BtnTextAdvOutClr2,
            BtnTextAdvPosition2,
            BtnTextAdvBounds2,
            BtnTextAdvBold2,
            BtnTextAdvItal2,
            BtnTextAdvClr3,
            BtnTextAdvChSz3,
            BtnTextAdvOutThck3,
            BtnTextAdvOutClr3,
            BtnTextAdvPosition3,
            BtnTextAdvBounds3,
            BtnTextAdvBold3,
            BtnTextAdvItal3,
            BtnTextAdvClr4,
            BtnTextAdvChSz4,
            BtnTextAdvOutThck4,
            BtnTextAdvOutClr4,
            BtnTextAdvPosition4,
            BtnTextAdvBounds4,
            BtnTextAdvBold4,
            BtnTextAdvItal4,
            BtnTextAdvClr5,
            BtnTextAdvChSz5,
            BtnTextAdvOutThck5,
            BtnTextAdvOutClr5,
            BtnTextAdvPosition5,
            BtnTextAdvBounds5,
            BtnTextAdvBold5,
            BtnTextAdvItal5,
            BtnTextAdvClr6,
            BtnTextAdvChSz6,
            BtnTextAdvOutThck6,
            BtnTextAdvOutClr6,
            BtnTextAdvPosition6,
            BtnTextAdvBounds6,
            BtnTextAdvBold6,
            BtnTextAdvItal6,
            BtnTextAdvClr7,
            BtnTextAdvChSz7,
            BtnTextAdvOutThck7,
            BtnTextAdvOutClr7,
            BtnTextAdvPosition7,
            BtnTextAdvBounds7,
            BtnTextAdvBold7,
            BtnTextAdvItal7,
            BtnTextAdvClr8,
            BtnTextAdvChSz8,
            BtnTextAdvOutThck8,
            BtnTextAdvOutClr8,
            BtnTextAdvPosition8,
            BtnTextAdvBounds8,
            BtnTextAdvBold8,
            BtnTextAdvItal8,
            BtnTextAdvClr9,
            BtnTextAdvChSz9,
            BtnTextAdvOutThck9,
            BtnTextAdvOutClr9,
            BtnTextAdvPosition9,
            BtnTextAdvBounds9,
            BtnTextAdvBold9,
            BtnTextAdvItal9,
            BtnTextAdvClr10,
            BtnTextAdvChSz10,
            BtnTextAdvOutThck10,
            BtnTextAdvOutClr10,
            BtnTextAdvPosition10,
            BtnTextAdvBounds10,
            BtnTextAdvBold10,
            BtnTextAdvItal10,
            BtnTextAdvClr11,
            BtnTextAdvChSz11,
            BtnTextAdvOutThck11,
            BtnTextAdvOutClr11,
            BtnTextAdvPosition11,
            BtnTextAdvBounds11,
            BtnTextAdvBold11,
            BtnTextAdvItal11,
            BtnTextAdvClr12,
            BtnTextAdvChSz12,
            BtnTextAdvOutThck12,
            BtnTextAdvOutClr12,
            BtnTextAdvPosition12,
            BtnTextAdvBounds12,
            BtnTextAdvBold12,
            BtnTextAdvItal12,
            BtnTextAdvClr13,
            BtnTextAdvChSz13,
            BtnTextAdvOutThck13,
            BtnTextAdvOutClr13,
            BtnTextAdvPosition13,
            BtnTextAdvBounds13,
            BtnTextAdvBold13,
            BtnTextAdvItal13,
            BtnTextAdvClr14,
            BtnTextAdvChSz14,
            BtnTextAdvOutThck14,
            BtnTextAdvOutClr14,
            BtnTextAdvPosition14,
            BtnTextAdvBounds14,
            BtnTextAdvBold14,
            BtnTextAdvItal14,
            BtnTextAdvClr15,
            BtnTextAdvChSz15,
            BtnTextAdvOutThck15,
            BtnTextAdvOutClr15,
            BtnTextAdvPosition15,
            BtnTextAdvBounds15,
            BtnTextAdvBold15,
            BtnTextAdvItal15,
            BtnTextAdvClr16,
            BtnTextAdvChSz16,
            BtnTextAdvOutThck16,
            BtnTextAdvOutClr16,
            BtnTextAdvPosition16,
            BtnTextAdvBounds16,
            BtnTextAdvBold16,
            BtnTextAdvItal16,
            BtnTextAdvClr17,
            BtnTextAdvChSz17,
            BtnTextAdvOutThck17,
            BtnTextAdvOutClr17,
            BtnTextAdvPosition17,
            BtnTextAdvBounds17,
            BtnTextAdvBold17,
            BtnTextAdvItal17,
            BtnTextAdvClr18,
            BtnTextAdvChSz18,
            BtnTextAdvOutThck18,
            BtnTextAdvOutClr18,
            BtnTextAdvPosition18,
            BtnTextAdvBounds18,
            BtnTextAdvBold18,
            BtnTextAdvItal18,
            BtnTextAdvClr19,
            BtnTextAdvChSz19,
            BtnTextAdvOutThck19,
            BtnTextAdvOutClr19,
            BtnTextAdvPosition19,
            BtnTextAdvBounds19,
            BtnTextAdvBold19,
            BtnTextAdvItal19,
            BtnTextAdvClr20,
            BtnTextAdvChSz20,
            BtnTextAdvOutThck20,
            BtnTextAdvOutClr20,
            BtnTextAdvPosition20,
            BtnTextAdvBounds20,
            BtnTextAdvBold20,
            BtnTextAdvItal20,

            BtnGfxDist,
            BtnGfxTxtr,
            BtnGfxTxtrSz,
            BtnGfxTxtrClr,

            BtnGfxAdvMode,
            BtnGfxBtnPos1,
            BtnGfxSz1,
            BtnGfxClr1,
            BtnGfxBtnPos2,
            BtnGfxSz2,
            BtnGfxClr2,
            BtnGfxBtnPos3,
            BtnGfxSz3,
            BtnGfxClr3,
            BtnGfxBtnPos4,
            BtnGfxSz4,
            BtnGfxClr4,
            BtnGfxBtnPos5,
            BtnGfxSz5,
            BtnGfxClr5,
            BtnGfxBtnPos6,
            BtnGfxSz6,
            BtnGfxClr6,
            BtnGfxBtnPos7,
            BtnGfxSz7,
            BtnGfxClr7,
            BtnGfxBtnPos8,
            BtnGfxSz8,
            BtnGfxClr8,
            BtnGfxBtnPos9,
            BtnGfxSz9,
            BtnGfxClr9,
            BtnGfxBtnPos10,
            BtnGfxSz10,
            BtnGfxClr10,
            BtnGfxBtnPos11,
            BtnGfxSz11,
            BtnGfxClr11,
            BtnGfxBtnPos12,
            BtnGfxSz12,
            BtnGfxClr12,
            BtnGfxBtnPos13,
            BtnGfxSz13,
            BtnGfxClr13,
            BtnGfxBtnPos14,
            BtnGfxSz14,
            BtnGfxClr14,
            BtnGfxBtnPos15,
            BtnGfxSz15,
            BtnGfxClr15,
            BtnGfxBtnPos16,
            BtnGfxSz16,
            BtnGfxClr16,
            BtnGfxBtnPos17,
            BtnGfxSz17,
            BtnGfxClr17,
            BtnGfxBtnPos18,
            BtnGfxSz18,
            BtnGfxClr18,
            BtnGfxBtnPos19,
            BtnGfxSz19,
            BtnGfxClr19,
            BtnGfxBtnPos20,
            BtnGfxSz20,
            BtnGfxClr20,
            
            AnimGfxVel,
            
            AnimGfxLight,
            AnimGfxTxtr,
            AnimGfxScl,
            AnimGfxClr,
            AnimGfxPress,
            AnimGfxOffset,

            BgTxtr,
            BgClr,
            BgScale,
            MainWndwTitleBar,
            MainWndwTop,
            MainWndwBot,
            MainWndwLft,
            MainWndwRght,

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

            KeyPressVisHint,
            KeyPressVisToggle,
            KeyPressVisSpeed,
            KeyPressVisRotation,
            KeyPressVisFadeLineLen,
            KeyPressVisOrig,
            KeyPressVisColor,
            KeyPressVisWidthScale,
            KeyPressVisFixedHeight,

            KeyPressVisAdvMode,
            KeyPressVisAdvModeSpeed1,
            KeyPressVisAdvModeRotation1,
            KeyPressVisAdvModeFadeLineLen1,
            KeyPressVisAdvModeOrig1,
            KeyPressVisAdvModeColor1,
            KeyPressVisAdvModeWidthScale1,
            KeyPressVisAdvModeFixedHeight1,
            KeyPressVisAdvModeSpeed2,
            KeyPressVisAdvModeRotation2,
            KeyPressVisAdvModeFadeLineLen2,
            KeyPressVisAdvModeOrig2,
            KeyPressVisAdvModeColor2,
            KeyPressVisAdvModeWidthScale2,
            KeyPressVisAdvModeFixedHeight2,
            KeyPressVisAdvModeSpeed3,
            KeyPressVisAdvModeRotation3,
            KeyPressVisAdvModeFadeLineLen3,
            KeyPressVisAdvModeOrig3,
            KeyPressVisAdvModeColor3,
            KeyPressVisAdvModeWidthScale3,
            KeyPressVisAdvModeFixedHeight3,
            KeyPressVisAdvModeSpeed4,
            KeyPressVisAdvModeRotation4,
            KeyPressVisAdvModeFadeLineLen4,
            KeyPressVisAdvModeOrig4,
            KeyPressVisAdvModeColor4,
            KeyPressVisAdvModeWidthScale4,
            KeyPressVisAdvModeFixedHeight4,
            KeyPressVisAdvModeSpeed5,
            KeyPressVisAdvModeRotation5,
            KeyPressVisAdvModeFadeLineLen5,
            KeyPressVisAdvModeOrig5,
            KeyPressVisAdvModeColor5,
            KeyPressVisAdvModeWidthScale5,
            KeyPressVisAdvModeFixedHeight5,
            KeyPressVisAdvModeSpeed6,
            KeyPressVisAdvModeRotation6,
            KeyPressVisAdvModeFadeLineLen6,
            KeyPressVisAdvModeOrig6,
            KeyPressVisAdvModeColor6,
            KeyPressVisAdvModeWidthScale6,
            KeyPressVisAdvModeFixedHeight6,
            KeyPressVisAdvModeSpeed7,
            KeyPressVisAdvModeRotation7,
            KeyPressVisAdvModeFadeLineLen7,
            KeyPressVisAdvModeOrig7,
            KeyPressVisAdvModeColor7,
            KeyPressVisAdvModeWidthScale7,
            KeyPressVisAdvModeFixedHeight7,
            KeyPressVisAdvModeSpeed8,
            KeyPressVisAdvModeRotation8,
            KeyPressVisAdvModeFadeLineLen8,
            KeyPressVisAdvModeOrig8,
            KeyPressVisAdvModeColor8,
            KeyPressVisAdvModeWidthScale8,
            KeyPressVisAdvModeFixedHeight8,
            KeyPressVisAdvModeSpeed9,
            KeyPressVisAdvModeRotation9,
            KeyPressVisAdvModeFadeLineLen9,
            KeyPressVisAdvModeOrig9,
            KeyPressVisAdvModeColor9,
            KeyPressVisAdvModeWidthScale9,
            KeyPressVisAdvModeFixedHeight9,
            KeyPressVisAdvModeSpeed10,
            KeyPressVisAdvModeRotation10,
            KeyPressVisAdvModeFadeLineLen10,
            KeyPressVisAdvModeOrig10,
            KeyPressVisAdvModeColor10,
            KeyPressVisAdvModeWidthScale10,
            KeyPressVisAdvModeFixedHeight10,
            KeyPressVisAdvModeSpeed11,
            KeyPressVisAdvModeRotation11,
            KeyPressVisAdvModeFadeLineLen11,
            KeyPressVisAdvModeOrig11,
            KeyPressVisAdvModeColor11,
            KeyPressVisAdvModeWidthScale11,
            KeyPressVisAdvModeFixedHeight11,
            KeyPressVisAdvModeSpeed12,
            KeyPressVisAdvModeRotation12,
            KeyPressVisAdvModeFadeLineLen12,
            KeyPressVisAdvModeOrig12,
            KeyPressVisAdvModeColor12,
            KeyPressVisAdvModeWidthScale12,
            KeyPressVisAdvModeFixedHeight12,
            KeyPressVisAdvModeSpeed13,
            KeyPressVisAdvModeRotation13,
            KeyPressVisAdvModeFadeLineLen13,
            KeyPressVisAdvModeOrig13,
            KeyPressVisAdvModeColor13,
            KeyPressVisAdvModeWidthScale13,
            KeyPressVisAdvModeFixedHeight13,
            KeyPressVisAdvModeSpeed14,
            KeyPressVisAdvModeRotation14,
            KeyPressVisAdvModeFadeLineLen14,
            KeyPressVisAdvModeOrig14,
            KeyPressVisAdvModeColor14,
            KeyPressVisAdvModeWidthScale14,
            KeyPressVisAdvModeFixedHeight14,
            KeyPressVisAdvModeSpeed15,
            KeyPressVisAdvModeRotation15,
            KeyPressVisAdvModeFadeLineLen15,
            KeyPressVisAdvModeOrig15,
            KeyPressVisAdvModeColor15,
            KeyPressVisAdvModeWidthScale15,
            KeyPressVisAdvModeFixedHeight15,
            KeyPressVisAdvModeSpeed16,
            KeyPressVisAdvModeRotation16,
            KeyPressVisAdvModeFadeLineLen16,
            KeyPressVisAdvModeOrig16,
            KeyPressVisAdvModeColor16,
            KeyPressVisAdvModeWidthScale16,
            KeyPressVisAdvModeFixedHeight16,
            KeyPressVisAdvModeSpeed17,
            KeyPressVisAdvModeRotation17,
            KeyPressVisAdvModeFadeLineLen17,
            KeyPressVisAdvModeOrig17,
            KeyPressVisAdvModeColor17,
            KeyPressVisAdvModeWidthScale17,
            KeyPressVisAdvModeFixedHeight17,
            KeyPressVisAdvModeSpeed18,
            KeyPressVisAdvModeRotation18,
            KeyPressVisAdvModeFadeLineLen18,
            KeyPressVisAdvModeOrig18,
            KeyPressVisAdvModeColor18,
            KeyPressVisAdvModeWidthScale18,
            KeyPressVisAdvModeFixedHeight18,
            KeyPressVisAdvModeSpeed19,
            KeyPressVisAdvModeRotation19,
            KeyPressVisAdvModeFadeLineLen19,
            KeyPressVisAdvModeOrig19,
            KeyPressVisAdvModeColor19,
            KeyPressVisAdvModeWidthScale19,
            KeyPressVisAdvModeFixedHeight19,
            KeyPressVisAdvModeSpeed20,
            KeyPressVisAdvModeRotation20,
            KeyPressVisAdvModeFadeLineLen20,
            KeyPressVisAdvModeOrig20,
            KeyPressVisAdvModeColor20,
            KeyPressVisAdvModeWidthScale20,
            KeyPressVisAdvModeFixedHeight20,

            OtherSaveStats,
            OtherShowOppOnAlt,
            OtherMultpl,

            SaveStatMaxKPS,
            SaveStatTotal,
            SaveStatTotal1,
            SaveStatTotal2,
            SaveStatTotal3,
            SaveStatTotal4,
            SaveStatTotal5,
            SaveStatTotal6,
            SaveStatTotal7,
            SaveStatTotal8,
            SaveStatTotal9,
            SaveStatTotal10,
            SaveStatTotal11,
            SaveStatTotal12,
            SaveStatTotal13,
            SaveStatTotal14,
            SaveStatTotal15,
            SaveStatTotal16,
            SaveStatTotal17,
            SaveStatTotal18,
            SaveStatTotal19,
            SaveStatTotal20,
        };

        union Val
        {
            unsigned *uP;
            int *iP;
            bool *bP;
            float *fP;
            std::string *sP;
            sf::Color *cP;
            sf::Vector2u *vUp;
            sf::Vector2i *vIp;
            sf::Vector2f *vFp;
        };

        LogicalParameter(Type type, void *valPtr, const std::string &parName = "", const std::string &defVal = "", 
            float lowLimits = 0.f, float highLimits = 0.f, const std::string &val = "");

        template <typename T>
        void setDigit(T var);

        void setBool(const std::string &str);
        void setBool(bool b);
        void setString(const std::string &str);
        void setColor(sf::Color color);
        void setColor(const std::string &str, unsigned idx);

        template <typename T>
        void setVector(T vec);
        void setVector(const std::string &str, unsigned idx);


        template <typename T>
        T getDigit() const;

        bool getBool() const;
        std::string getString() const;
        sf::Color getColor() const;

        template <typename T>
        T getVector() const;

        std::string getValPt(int pt) const;
        void resetToDefaultValue();


        void setValStr(const std::string &str, unsigned idx = 0);
        std::string getValStr() const;
        const std::string &getDefValStr() const;

        bool resetChangeState();

        static std::string getInverseBool(bool b);
        

    public:
        const Type mType;
        const std::string mParName;
        float mLowLimits, mHighLimits;
        bool mChanged;

    private:
        Val mVal;
        const std::string mDefValStr;
        std::string mValStr;
};

template <typename T>
void LogicalParameter::setDigit(T var)
{
    assert(mType == Type::Unsigned || mType == Type::Int || mType == Type::Float);

    switch(mType)
    {
        case Type::Unsigned: *mVal.uP = var; mValStr = std::to_string(static_cast<unsigned>(var)); break;
        case Type::Int: *mVal.iP = var; mValStr = std::to_string(static_cast<int>(var)); break;
        case Type::Float: *mVal.fP = var; mValStr = std::to_string(static_cast<int>(var)); /*+ 1 dec digit*/ break;
        
        default: break;
    }
    mChanged = true;
}

template <typename T>
void LogicalParameter::setVector(T vec)
{
    assert(mType == Type::VectorU || mType == Type::VectorI || mType == Type::VectorF);

    switch(mType)
    {
        case Type::VectorU: mVal.vUp->x = vec.x; mVal.vUp->y = vec.y; break;
        case Type::VectorI: mVal.vIp->x = vec.x; mVal.vIp->y = vec.y; break;
        case Type::VectorF: mVal.vFp->x = vec.x; mVal.vFp->y = vec.y; break;

        default: break;
    }

    mValStr = std::to_string(static_cast<int>(vec.x)) + ',' + std::to_string(static_cast<int>(vec.y));
    mChanged = true;
}

template <typename T>
T LogicalParameter::getDigit() const
{
    assert(mType == Type::Unsigned || mType == Type::Int || mType == Type::Bool || mType == Type::Float);

    switch(mType)
    {
        case Type::Unsigned: return *mVal.uP;
        case Type::Int: return *mVal.iP;
        case Type::Bool: return *mVal.bP;
        case Type::Float: return *mVal.fP;
    }

    return T(-1);
}

template <typename T>
T LogicalParameter::getVector() const
{
    assert(mType == Type::VectorU || mType == Type::VectorI || mType == Type::VectorF);

    switch(mType)
    {
        case Type::VectorU: return *mVal.vUp;
        case Type::VectorI: return *mVal.vIp;
        case Type::VectorF: return *mVal.vFp;
        default: return T(-1, -1);
    }
}
