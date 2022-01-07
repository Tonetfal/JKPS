#include "../Headers/LogicalParameter.hpp"
#include "../Headers/ConfigHelper.hpp"

#include <string.h>
#include <iostream>


LogicalParameter::LogicalParameter(Type type, void *valPtr, const std::string &parName, const std::string &defVal, 
    float lowLimits, float highLimits, const std::string &val)
: mType(type)
, mParName(parName)
, mDefValStr(defVal)
, mLowLimits(lowLimits)
, mHighLimits(highLimits)
, mChanged(false)
{ 
    if (type == Type::Color)
        this->mHighLimits = 255;
    if (val == "") 
        this->mValStr = defVal; 
    
    switch(type)
    {
        case Type::Unsigned: 
            mVal.uP = static_cast<unsigned*>(valPtr); 
            setDigit<unsigned>(std::stoi(defVal)); 
            break;
            
        case Type::Int: 
            mVal.iP = static_cast<int*>(valPtr); 
            setDigit<int>(std::stoi(defVal)); 
            break;

        case Type::Bool: 
            mVal.bP = static_cast<bool*>(valPtr); 
            setBool(defVal); 
            break;

        case Type::Float: 
            mVal.fP = static_cast<float*>(valPtr); 
            setDigit<float>(std::stoi(defVal)); 
            break;

        case Type::String: 
        case Type::StringPath:
            mVal.sP = static_cast<std::string*>(valPtr); 
            setString(defVal); 
            break;

        case Type::Color: 
            mVal.cP = static_cast<sf::Color*>(valPtr); 
            setColor(ConfigHelper::readColorParameter(*this, defVal)); 
            break;

        case Type::VectorU: 
            mVal.vUp = static_cast<sf::Vector2u*>(valPtr); 
            setVector(ConfigHelper::readVectorParameter(*this, defVal)); 
            break;

        case Type::VectorI: 
            mVal.vIp = static_cast<sf::Vector2i*>(valPtr); 
            setVector(ConfigHelper::readVectorParameter(*this, defVal)); 
            break;

        case Type::VectorF: 
            mVal.vFp = static_cast<sf::Vector2f*>(valPtr); 
            setVector(ConfigHelper::readVectorParameter(*this, defVal)); 
            break;

        default: break; // Empty or Collection
    }
} 

void LogicalParameter::setColor(sf::Color color)
{
    assert(mType == Type::Color);

    *mVal.cP = color;

    mValStr = std::to_string(mVal.cP->r) + ',' + std::to_string(mVal.cP->g) + ',' + std::to_string(mVal.cP->b) + ',' + std::to_string(mVal.cP->a);
    mChanged = true;
}

void LogicalParameter::setColor(const std::string &str, unsigned idx)
{
    assert(mType == Type::Color);
    assert(idx >= 0 && idx <= 3);

    unsigned char c = static_cast<unsigned char>(std::stoi(str));
    switch (idx)
    {
        case 0: mVal.cP->r = c; break;
        case 1: mVal.cP->g = c; break;
        case 2: mVal.cP->b = c; break;
        case 3: mVal.cP->a = c; break;
    }
    mValStr = std::to_string(mVal.cP->r) + ',' + std::to_string(mVal.cP->g) + ',' + std::to_string(mVal.cP->b) + ',' + std::to_string(mVal.cP->a);
    mChanged = true;
}

void LogicalParameter::setVector(const std::string &str, unsigned idx)
{
    assert(mType == Type::VectorU || mType == Type::VectorI || mType == Type::VectorF);
    assert(idx == 0 || idx == 1);

    switch(mType)
    {
        case Type::VectorU: 
            switch(idx)
            {
                case 0: mVal.vUp->x = std::stoi(str); break;
                case 1: mVal.vUp->y = std::stoi(str); break;
            }
            mValStr = std::to_string(mVal.vUp->x) + ',' + std::to_string(mVal.vUp->y);
            break;
        case Type::VectorI: 
            switch(idx)
            {
                case 0: mVal.vIp->x = std::stoi(str); break;
                case 1: mVal.vIp->y = std::stoi(str); break;
            }
            mValStr = std::to_string(mVal.vIp->x) + ',' + std::to_string(mVal.vIp->y);
            break;
        case Type::VectorF: 
            switch(idx)
            {
                case 0: mVal.vFp->x = std::stoi(str); break;
                case 1: mVal.vFp->y = std::stoi(str); break;
            }
            mValStr = std::to_string(static_cast<int>(mVal.vFp->x)) + ',' + std::to_string(static_cast<int>(mVal.vFp->y));
            break;
    }
    mChanged = true;
}


void LogicalParameter::setBool(const std::string &str)
{
    assert(mType == Type::Bool); 
    assert(
        str == "true" || str == "false"
    ||  str == "True" || str == "False"
    ||  str == "TRUE" || str == "FALSE");

    // "true" - 4 characters. "false" - 5 characters
    *mVal.bP = str.size() == 4;
    mValStr = str;
    mChanged = true;
}

void LogicalParameter::setBool(bool b)
{
    assert(mType == Type::Bool);
    *mVal.bP = b;
    mValStr = b ? "True" : "False";
    mChanged = true;
}

void LogicalParameter::setString(const std::string &str)
{
    assert(mType == Type::String || mType == Type::StringPath);
    *mVal.sP = str;
    mValStr = str;
    mChanged = true;
}

bool LogicalParameter::getBool() const
{
    assert(mType == Type::Bool);
    return *mVal.bP;
}

std::string LogicalParameter::getString() const
{
    assert(mType == Type::String || mType == Type::StringPath);
    return *mVal.sP;
}

sf::Color LogicalParameter::getColor() const
{
    assert(mType == Type::Color);
    return *mVal.cP;
}

std::string LogicalParameter::getValPt(int pt) const
{
    assert(mType == Type::Color || mType == Type::VectorU 
    || mType == Type::VectorI || mType == Type::VectorF);

    assert(pt >= 0 && pt <= 3);
    // Don't allow to pass pt >= 2, when type is not color
    assert(!(pt >= 2 && mType != Type::Color));

    std::string res = mValStr; 
    while(--pt >= 0)
        res = res.substr(res.find(',') + 1); 

    if (res.find(',') != std::string::npos)
        res = res.substr(0, res.find(','));

    return res;
}

void LogicalParameter::setValStr(const std::string &str, unsigned idx)
{
    mValStr = str;
    switch (mType)
    {
        case LogicalParameter::Type::Unsigned: setDigit(std::stoi(str)); break;
        case LogicalParameter::Type::Int: setDigit(std::stoi(str)); break;
        case LogicalParameter::Type::Bool: setBool(str); break;
        case LogicalParameter::Type::Float: setDigit(std::stoi(str)); break;
        case LogicalParameter::Type::String:
        case LogicalParameter::Type::StringPath: setString(str); break;
        case LogicalParameter::Type::Color: setColor(str, idx); break;
        case LogicalParameter::Type::VectorU:
        case LogicalParameter::Type::VectorI:
        case LogicalParameter::Type::VectorF: setVector(str, idx); break;
    }
}


std::string LogicalParameter::getValStr() const
{
    return mValStr;
}

const std::string &LogicalParameter::getDefValStr() const
{
    return mDefValStr;
}

bool LogicalParameter::resetChangeState()
{
    return mChanged && !(mChanged = false);
}


std::string LogicalParameter::getInverseBool(bool b)
{
    return b ? "False" : "True";
}

void LogicalParameter::resetToDefaultValue()
{
    switch(mType)
    {
        case Type::Unsigned: 
            setDigit<unsigned>(std::stoi(mDefValStr)); 
            break;
            
        case Type::Int: 
            setDigit<int>(std::stoi(mDefValStr)); 
            break;

        case Type::Bool: 
            setBool(mDefValStr); 
            break;

        case Type::Float: 
            setDigit<float>(std::stoi(mDefValStr)); 
            break;

        case Type::String: 
        case Type::StringPath:
            setString(mDefValStr); 
            break;

        case Type::Color: 
            setColor(ConfigHelper::readColorParameter(*this, mDefValStr)); 
            break;

        case Type::VectorU: 
            setVector(ConfigHelper::readVectorParameter(*this, mDefValStr)); 
            break;

        case Type::VectorI: 
            setVector(ConfigHelper::readVectorParameter(*this, mDefValStr)); 
            break;

        case Type::VectorF: 
            setVector(ConfigHelper::readVectorParameter(*this, mDefValStr)); 
            break;

        default: break; // Empty or Collection
    }
}
