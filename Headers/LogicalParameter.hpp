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

            StatTextPosAdvMode,
            StatTextPos1,
            StatTextPos2,
            StatTextPos3,
            StatTextValPosAdvMode,
            StatTextValPos1,
            StatTextValPos2,
            StatTextValPos3,
            StatTextClrAdvMode,
            StatTextClr1,
            StatTextClr2,
            StatTextClr3,
            StatTextChSzAdvMode,
            StatTextChSz1,
            StatTextChSz2,
            StatTextChSz3,
            StatTextBoldAdvMode,
            StatTextBold1,
            StatTextBold2,
            StatTextBold3,
            StatTextItalAdvMode,
            StatTextItal1,
            StatTextItal2,
            StatTextItal3,
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
            BtnTextBounds,
            BtnTextIgnoreBtnMovement,
            BtnTextBold,
            BtnTextItal,
            BtnTextShowVisKeys,
            BtnTextShowTot,
            BtnTextShowKps,
            BtnTextShowBpm,

            BtnTextSepPosAdvMode,
            BtnTextVisPosition,
            BtnTextTotPosition,
            BtnTextKPSPosition,
            BtnTextBPMPosition,
            BtnTextPosAdvMode,
            BtnTextPos1,
            BtnTextPos2,
            BtnTextPos3,
            BtnTextPos4,
            BtnTextPos5,
            BtnTextPos6,
            BtnTextPos7,
            BtnTextPos8,
            BtnTextPos9,
            BtnTextPos10,
            BtnTextPos11,
            BtnTextPos12,
            BtnTextPos13,
            BtnTextPos14,
            BtnTextPos15,

            BtnGfxDist,
            BtnGfxTxtr,
            BtnGfxTxtrSz,
            BtnGfxTxtrClr,

            BtnGfxBtnPosAdvMode,
            BtnGfxSzAdvMode,
            BtnGfxBtnPos1,
            BtnGfxSz1,
            BtnGfxBtnPos2,
            BtnGfxSz2,
            BtnGfxBtnPos3,
            BtnGfxSz3,
            BtnGfxBtnPos4,
            BtnGfxSz4,
            BtnGfxBtnPos5,
            BtnGfxSz5,
            BtnGfxBtnPos6,
            BtnGfxSz6,
            BtnGfxBtnPos7,
            BtnGfxSz7,
            BtnGfxBtnPos8,
            BtnGfxSz8,
            BtnGfxBtnPos9,
            BtnGfxSz9,
            BtnGfxBtnos10,
            BtnGfxSz10,
            BtnGfxBtnos11,
            BtnGfxSz11,
            BtnGfxBtnos12,
            BtnGfxSz12,
            BtnGfxBtnos13,
            BtnGfxSz13,
            BtnGfxBtnos14,
            BtnGfxSz14,
            BtnGfxBtnos15,
            BtnGfxSz15,
            
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

            OtherSaveStats,
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
