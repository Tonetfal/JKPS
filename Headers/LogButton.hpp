#pragma once

#include "LogKey.hpp"

#include <memory>
#include <array>


class LogButton
{
    public:
        LogButton(LogKey &key);

        void processRealtimeInput();

        static void movePointer();
        static unsigned resetPressesInCurrentFrame();
        bool isButtonPressed() const;

        void reset();

        static float getKeysPerSecond();
        static float getMaxKeysPerSecond();
        static unsigned getTotal();
        static float getBeatsPerMinute();
    

    protected:
        float getLocalBeatsPerMinute() const;


    public:
        static float statKeysPerSecond;
        static float statMaxKeysPerSecond;
        static unsigned statTotal;
        static float statBeatsPerMinute;


    protected:
        LogKey mKey;
        float mKeysPerSecond;
        unsigned mTotal;


    private:
        // Calculation related
        std::array<unsigned, 60> mBuffer;
        std::array<unsigned, 9> mPrevKpsBuffer;
        static unsigned mBufferPointer;
        static unsigned mPrevKpsBufferPointer;
    
        bool mNeedToRelease;
};