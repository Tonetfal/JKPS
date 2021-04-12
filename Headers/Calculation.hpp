#pragma once

#include "KeyPressingManager.hpp"

#include <SFML/System/Time.hpp>

#include <array>
#include <vector>

class Calculation
{
    public:
                                    Calculation();

        void                        handleInput(KeyPressingManager& container, std::vector<sf::Keyboard::Key>& keys);
        void                        update();

        std::size_t                 getKeyPerSecond();
        std::size_t                 getBeatsPerMinute();


    private:
        int                         mFrameTick;
        int                         mClickedKeysInCurrentFrame;
        std::array<int, 60>         mKeysPerTick;
        std::size_t                 mKeyPerSecond;

        const size_t                mNumberOfKPSToSave;
        std::vector<size_t>         mPreviousKeyPerSeconds;
};