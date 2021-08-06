#pragma once

#include "GfxButton.hpp"
#include "LogButton.hpp"
#include "ResourceIdentifiers.hpp"
#include "LogicalParameter.hpp"


class Button : public GfxButton, public LogButton
{
    public:
        Button(LogKey &key, const TextureHolder &textureHolder, const FontHolder &fontHolder);

        void update();
        void processInput();

        void reset();

        LogKey *getLogKey();

        unsigned getIdx() const;
        static unsigned size();
        static bool parameterIdMatches(LogicalParameter::ID id);

        ~Button();


    private:
        void setTextStrings();
        void controlBounds();


    private:
        const TextureHolder &mTextures;
        const FontHolder &mFonts;

        const unsigned mBtnIdx;
        static unsigned mSize;
};