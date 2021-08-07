#pragma once

#include "Button.hpp"

#include <vector>
#include <memory>


struct ButtonPositioner
{
    ButtonPositioner(std::vector<std::unique_ptr<Button>> *buttons);

    void operator()();

    std::vector<std::unique_ptr<Button>> *mButtons;
};