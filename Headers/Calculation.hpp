#pragma once

#include "KeyPressingManager.hpp"
#include "Settings.hpp"

#include <math.h>
#include <array>

class Calculation
{
    public:
                                    Calculation();

        void                        handleInput(KeyPressingManager& container, std::vector<sf::Keyboard::Key>& keys);
        void                        update();

        std::size_t                 getKeyPerSecond();
        std::size_t                 getBeatsPerMinute();


    private:
        std::size_t                 mFrameTick;
        std::size_t                 mClickedKeysInCurrentFrame;
        std::array<size_t, 60>      mKeysPerTick;
        std::size_t                 mKeyPerSecond;

        const size_t                mNumberOfKPSToSave;
        std::vector<size_t>         mPreviousKeyPerSeconds;
};