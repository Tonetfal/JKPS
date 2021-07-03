#pragma once

#include "SceneNode.hpp"

#include <array>
#include <vector>

// KPS calculation
// There is a buffer iterator that is in an infinite loop, once becoming std::array.size()
// it will be reset to 0. When user presses a button Calculator::add()
// function is called, and mPressesInCurrentUpdate is increased. That variable
// will be added in current element of the buffer, and then to the current KPS.
// After the update function iterator will be incremented.
// When the function starts it checks the current buffer element, and if it is 
// more than 0, the current KPS will be decremented by that value.

// BPM calculation
// Basically there is a KPS buffer that holds mKPSBufferSize KPSs.
// When a request for BPM arrives an arithmetic sum is performed, and then
// multiplied by 60 (second) / 4 (1/4 time signature).
// The BPM will be calculated correctly only after a second and mKPSBufferSize frames.
// Also it will fall behing by mKPSBufferSize frames.


class Calculator : public SceneNode
{
    public:
        virtual void updateCurrent(sf::Time dt);

        static int getKeyPerSecond();
        static int getMaxKeyPerSecond();
        static int getTotalKeys();
        static int getBeatsPerMinute();

        static void add();
        static void clear();

        unsigned getCategory() const;

    private:
        static int mKeysPerSecond;
        static int mMaxKeyPerSecond;
        static int mTotalKeys;
        static int mBufferIt;
        static int mPressesInCurrentUpdate;
        // static std::array<int, 30> mBuffer;

        static const int mKPSBufferSize;
        static std::vector<int> mKPSBuffer;
};