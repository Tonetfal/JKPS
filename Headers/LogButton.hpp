#pragma once

#include "LogKey.hpp"

#include <memory>
#include <array>


class LogButton
{
    public:
        LogButton(const unsigned idx, LogKey &key);

        void processRealtimeInput();

        static void moveIndex();
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
        const unsigned mBtnIdx;
        bool mState;
        
        // Calculation related
        std::array<unsigned, 60lu> mBuffer;
        std::array<float, 7lu> mPrevKpsBuffer;
        static unsigned mBufferIndex;
        static unsigned mPrevKpsBufferIndex;
    
};