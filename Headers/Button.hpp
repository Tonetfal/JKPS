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

        LogKey *getLogKey();

        unsigned getIdx() const;
        static unsigned size();
        static bool parameterIdMatches(LogicalParameter::ID id);

        ~Button();


    private:
        const TextureHolder &mTextures;
        const FontHolder &mFonts;

        const unsigned mBtnIdx;
        static unsigned mSize;
};