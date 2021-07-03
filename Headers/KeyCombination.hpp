#pragma once

#include "Keys.hpp"

#include <vector>


struct KeyCombination
{
	KeyCombination(Keys k1, Keys k2 = Keys::Unknown, Keys k3 = Keys::Unknown);

    static bool isCombinationPressed(const KeyCombination &combination);

    bool operator==(const KeyCombination &combination) const;
    bool operator==(Keys key) const;
    bool operator<(const KeyCombination &combination) const;

	std::vector<Keys> mKeys;
};