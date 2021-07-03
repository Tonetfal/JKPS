#include "../Headers/Mode.hpp"
#include "../Headers/Settings.hpp"

Mode::Mode()
: mAlert(5.f)
{
    mAlert.setOrigin(5.f, 5.f);
    mAlert.setFillColor(Settings::AlertColor);
}

Mode::State Mode::getState()
{
    return mState;
}