#pragma once
 
#include "Keys.hpp"

#include <iostream>


std::string convertKeyToString(Keys key, bool saveToCfg);
Keys convertStringToKey(const std::string &str);