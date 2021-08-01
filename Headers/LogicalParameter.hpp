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
            SpaceBtwBtnAndStat,
            StatTextFont,
            StatTextClr,
            StatTextChSz,
            StatTextBold,
            StatTextItal,
            StatTextShow,
            StatTextShowKPS,
            StatTextShowMaxKPS,
            StatTextShowTotal,
            StatTextShowBPM,
            StatTextKPSText,
            StatTextKPS2Text,
            StatTextMaxKPSText,
            StatTextTotalText,
            StatTextBPMText,

            BtnTextFont,
            BtnTextClr,
            BtnTextChSz,
            BtnTextWidth,
            BtnTextHeight,
            BtnTextHorzBounds,
            BtnTextVertBounds,
            BtnTextBold,
            BtnTextItal,
            BtnTextShowKeys,
            BtnTextShowKeyCtrs,

            BtnGfxDist,
            BtnGfxTxtr,
            BtnGfxTxtrSz,
            BtnGfxTxtrClr,

            AnimGfxStl,
            AnimGfxTxtr,
            AnimGfxVel,
            AnimGfxScl,
            AnimGfxClr,
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

            OtherHighText,

            ThemeDevMultpl,
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
        const std::string mParName;
        const Type mType;
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
