#include "../Headers/StringHelper.hpp"
#include "../Headers/LogKey.hpp"

#include <cassert>


unsigned readAmountOfParms(const std::string &str)
{
    unsigned amt = 0;
    for (unsigned idx = 0; idx < str.size(); ++idx)
    {
        if (str[idx] == ',' || idx + 1 == str.size())
            ++amt;
    }

    return amt;
}

std::string readValue(const std::string &str, unsigned n)
{
    unsigned idx, nVal = 0;
    // Skip all previous values
    for (idx = 0; idx < str.size() && nVal != n; ++idx)
    {
        if (str[idx] == ',')
            ++nVal;
    }
    
    assert(nVal == n);
    
    std::string retVal;
    // Write everything in new str until newline or comma
    for (unsigned i = 0; idx < str.size() && str[idx] != ','; ++idx, ++i)
    {
        retVal += str[idx];
    }

    return retVal;
}

static void modifyNumOnIdx(std::string &str, unsigned idx, bool add, char num = ' ')
{
    std::string substr = str.substr(idx + !add);
    if (add)
    {
        str.resize(str.size() + 1);
        str[idx] = num;
        for (unsigned i = 0; i < substr.size(); ++i)
            str[idx + i + 1] = substr[i];        
    }
    else
    {
        for (unsigned i = 0; i < substr.size(); ++i)
            str[idx + i] = substr[i];        
        str.resize(str.size() - 1);
    }
}

void addChOnIdx(std::string &str, unsigned idx, char ch)
{
    modifyNumOnIdx(str, idx, true, ch);
}

void rmChOnIdx(std::string &str, unsigned idx)
{
    modifyNumOnIdx(str, idx, false);
}

std::string eraseDigitsOverHundredths(const std::string &floatStr)
{
    std::size_t pointIdx;
    const bool isFloat = (pointIdx = floatStr.find('.')) != std::string::npos;
    return isFloat ? floatStr.substr(0, pointIdx + 2) : floatStr;
}


// For more information
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Mouse.php 
// sf::Keyboard::A is used as unknown key in order to make the user change the regardless
// the fact that he didn't pass the idiot proof
// Same thing with mouse, I use sf::Mouse::Left as unknown button


std::string keyToStr(sf::Keyboard::Key key, bool saveToCfg)
{
    switch (key)
    {
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::B: return "B";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::G: return "G";
        case sf::Keyboard::H: return "H";
        case sf::Keyboard::I: return "I";
        case sf::Keyboard::J: return "J";
        case sf::Keyboard::K: return "K";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::M: return "M";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::O: return "O";
        case sf::Keyboard::P: return "P";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::U: return "U";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::Y: return "Y";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::Num0: return (saveToCfg ? "Num0" : "0");
        case sf::Keyboard::Num1: return (saveToCfg ? "Num1" : "1");
        case sf::Keyboard::Num2: return (saveToCfg ? "Num2" : "2");
        case sf::Keyboard::Num3: return (saveToCfg ? "Num3" : "3");
        case sf::Keyboard::Num4: return (saveToCfg ? "Num4" : "4");
        case sf::Keyboard::Num5: return (saveToCfg ? "Num5" : "5");
        case sf::Keyboard::Num6: return (saveToCfg ? "Num6" : "6");
        case sf::Keyboard::Num7: return (saveToCfg ? "Num7" : "7");
        case sf::Keyboard::Num8: return (saveToCfg ? "Num8" : "8");
        case sf::Keyboard::Num9: return (saveToCfg ? "Num9" : "9");
        case sf::Keyboard::Escape: return "Escape";
        case sf::Keyboard::LControl: return "LControl";
        case sf::Keyboard::LShift: return "LShift";
        case sf::Keyboard::LAlt: return "LAlt";
        case sf::Keyboard::LSystem: return "LSystem";
        case sf::Keyboard::RControl: return "RControl";
        case sf::Keyboard::RShift: return "RShift";
        case sf::Keyboard::RAlt: return "RAlt";
        case sf::Keyboard::RSystem: return "RSystem";
        case sf::Keyboard::Menu: return "Menu";
        case sf::Keyboard::LBracket: return (saveToCfg ? "LBracket" : "[");
        case sf::Keyboard::RBracket: return (saveToCfg ? "RBracket" : "]");
        case sf::Keyboard::Semicolon: return (saveToCfg ? "Semicolon" : ";");
        case sf::Keyboard::Comma: return (saveToCfg ? "Comma" : ",");
        case sf::Keyboard::Period: return (saveToCfg ? "Period" : ".");
        case sf::Keyboard::Quote: return (saveToCfg ? "Quote" : "\"");
        case sf::Keyboard::Slash: return (saveToCfg ? "Slash" : "/");
        case sf::Keyboard::Backslash: return (saveToCfg ? "Backslash" : "\\");
        case sf::Keyboard::Tilde: return (saveToCfg ? "Tilde" : "~");
        case sf::Keyboard::Equal: return (saveToCfg ? "Equal" : "=");
        case sf::Keyboard::Hyphen: return (saveToCfg ? "Hyphen" : "-");
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Enter: return "Enter";
        case sf::Keyboard::Backspace: return "Backspace";
        case sf::Keyboard::Tab: return "Tab";
        case sf::Keyboard::PageUp: return (saveToCfg ? "PageUp" : "PgUp");
        case sf::Keyboard::PageDown: return (saveToCfg ? "PageDown" : "PgDn");
        case sf::Keyboard::End: return "End";
        case sf::Keyboard::Home: return "Home";
        case sf::Keyboard::Insert: return (saveToCfg ? "Insert" : "Ins");
        case sf::Keyboard::Delete: return (saveToCfg ? "Delete" : "Del");
        case sf::Keyboard::Add: return (saveToCfg ? "Add" : "+");
        case sf::Keyboard::Subtract: return (saveToCfg ? "Subtract" : "-");
        case sf::Keyboard::Multiply: return (saveToCfg ? "Multiply" : "*");
        case sf::Keyboard::Divide: return (saveToCfg ? "Divide" : "/");
        case sf::Keyboard::Left: return "Left";
        case sf::Keyboard::Right: return "Right";
        case sf::Keyboard::Up: return "Up";
        case sf::Keyboard::Down: return "Down";
        case sf::Keyboard::Numpad0: return (saveToCfg ? "Numpad0" : "Num0");
        case sf::Keyboard::Numpad1: return (saveToCfg ? "Numpad1" : "Num1");
        case sf::Keyboard::Numpad2: return (saveToCfg ? "Numpad2" : "Num2");
        case sf::Keyboard::Numpad3: return (saveToCfg ? "Numpad3" : "Num3");
        case sf::Keyboard::Numpad4: return (saveToCfg ? "Numpad4" : "Num4");
        case sf::Keyboard::Numpad5: return (saveToCfg ? "Numpad5" : "Num5");
        case sf::Keyboard::Numpad6: return (saveToCfg ? "Numpad6" : "Num6");
        case sf::Keyboard::Numpad7: return (saveToCfg ? "Numpad7" : "Num7");
        case sf::Keyboard::Numpad8: return (saveToCfg ? "Numpad8" : "Num8");
        case sf::Keyboard::Numpad9: return (saveToCfg ? "Numpad9" : "Num9");
        case sf::Keyboard::F1: return "F1";
        case sf::Keyboard::F2: return "F2";
        case sf::Keyboard::F3: return "F3";
        case sf::Keyboard::F4: return "F4";
        case sf::Keyboard::F5: return "F5";
        case sf::Keyboard::F6: return "F6";
        case sf::Keyboard::F7: return "F7";
        case sf::Keyboard::F8: return "F8";
        case sf::Keyboard::F9: return "F9";
        case sf::Keyboard::F10: return "F10";
        case sf::Keyboard::F11: return "F11";
        case sf::Keyboard::F12: return "F12";
        case sf::Keyboard::F13: return "F13";
        case sf::Keyboard::F14: return "F14";
        case sf::Keyboard::F15: return "F15";
        case sf::Keyboard::Pause: return "Pause";
        default: return "Unknown";
    }
}

sf::Keyboard::Key strToKey(const std::string &str)
{
    if (str == "A")
        return sf::Keyboard::Key::A;
    if (str == "B")
        return sf::Keyboard::Key::B;
    if (str == "C")
        return sf::Keyboard::Key::C;
    if (str == "D")
        return sf::Keyboard::Key::D;
    if (str == "E")
        return sf::Keyboard::Key::E;
    if (str == "F")
        return sf::Keyboard::Key::F;
    if (str == "G")
        return sf::Keyboard::Key::G;
    if (str == "H")
        return sf::Keyboard::Key::H;
    if (str == "I")
        return sf::Keyboard::Key::I;
    if (str == "J")
        return sf::Keyboard::Key::J;
    if (str == "K")
        return sf::Keyboard::Key::K;
    if (str == "L")
        return sf::Keyboard::Key::L;
    if (str == "M")
        return sf::Keyboard::Key::M;
    if (str == "N")
        return sf::Keyboard::Key::N;
    if (str == "O")
        return sf::Keyboard::Key::O;
    if (str == "P")
        return sf::Keyboard::Key::P;
    if (str == "Q")
        return sf::Keyboard::Key::Q;
    if (str == "R")
        return sf::Keyboard::Key::R;
    if (str == "S")
        return sf::Keyboard::Key::S;
    if (str == "T")
        return sf::Keyboard::Key::T;
    if (str == "U")
        return sf::Keyboard::Key::U;
    if (str == "V")
        return sf::Keyboard::Key::V;
    if (str == "W")
        return sf::Keyboard::Key::W;
    if (str == "X")
        return sf::Keyboard::Key::X;
    if (str == "Y")
        return sf::Keyboard::Key::Y;
    if (str == "Z")
        return sf::Keyboard::Key::Z;
    if (str == "Num0")
        return sf::Keyboard::Key::Num0;
    if (str == "Num1")
        return sf::Keyboard::Key::Num1;
    if (str == "Num2")
        return sf::Keyboard::Key::Num2;
    if (str == "Num3")
        return sf::Keyboard::Key::Num3;
    if (str == "Num4")
        return sf::Keyboard::Key::Num4;
    if (str == "Num5")
        return sf::Keyboard::Key::Num5;
    if (str == "Num6")
        return sf::Keyboard::Key::Num6;
    if (str == "Num7")
        return sf::Keyboard::Key::Num7;
    if (str == "Num8")
        return sf::Keyboard::Key::Num8;
    if (str == "Num9")
        return sf::Keyboard::Key::Num9;
    if (str == "Escape")
        return sf::Keyboard::Key::Escape;
    if (str == "LControl")
        return sf::Keyboard::Key::LControl;
    if (str == "LShift")
        return sf::Keyboard::Key::LShift;
    if (str == "LAlt")
        return sf::Keyboard::Key::LAlt;
    if (str == "LSystem")
        return sf::Keyboard::Key::LSystem;
    if (str == "RControl")
        return sf::Keyboard::Key::RControl;
    if (str == "RShift")
        return sf::Keyboard::Key::RShift;
    if (str == "RAlt")
        return sf::Keyboard::Key::RAlt;
    if (str == "RSystem")
        return sf::Keyboard::Key::RSystem;
    if (str == "Menu")
        return sf::Keyboard::Key::Menu;
    if (str == "LBracket")
        return sf::Keyboard::Key::LBracket;
    if (str == "RBracket")
        return sf::Keyboard::Key::RBracket;
    if (str == "Semicolon")
        return sf::Keyboard::Key::Semicolon;
    if (str == "Comma")
        return sf::Keyboard::Key::Comma;
    if (str == "Period")
        return sf::Keyboard::Key::Period;
    if (str == "Quote")
        return sf::Keyboard::Key::Quote;
    if (str == "Slash")
        return sf::Keyboard::Key::Slash;
    if (str == "Backslash")
        return sf::Keyboard::Key::Backslash;
    if (str == "Tilde")
        return sf::Keyboard::Key::Tilde;
    if (str == "Equal")
        return sf::Keyboard::Key::Equal;
    if (str == "Hyphen")
        return sf::Keyboard::Key::Hyphen;
    if (str == "Space")
        return sf::Keyboard::Key::Space;
    if (str == "Enter")
        return sf::Keyboard::Key::Enter;
    if (str == "Backspace")
        return sf::Keyboard::Key::Backspace;
    if (str == "Tab")
        return sf::Keyboard::Key::Tab;
    if (str == "PageUp")
        return sf::Keyboard::Key::PageUp;
    if (str == "PageDown")
        return sf::Keyboard::Key::PageDown;
    if (str == "End")
        return sf::Keyboard::Key::End;
    if (str == "Home")
        return sf::Keyboard::Key::Home;
    if (str == "Insert")
        return sf::Keyboard::Key::Insert;
    if (str == "Delete")
        return sf::Keyboard::Key::Delete;
    if (str == "Add")
        return sf::Keyboard::Key::Add;
    if (str == "Subtract")
        return sf::Keyboard::Key::Subtract;
    if (str == "Multiply")
        return sf::Keyboard::Key::Multiply;
    if (str == "Divide")
        return sf::Keyboard::Key::Divide;
    if (str == "Left" || str == "LeftArrow")
        return sf::Keyboard::Key::Left;
    if (str == "Right" || str == "RightArrow")
        return sf::Keyboard::Key::Right;
    if (str == "Up" || str == "UpArrow")
        return sf::Keyboard::Key::Up;
    if (str == "Down" || str == "DownArrow")
        return sf::Keyboard::Key::Down;
    if (str == "Numpad0")
        return sf::Keyboard::Key::Numpad0;
    if (str == "Numpad1")
        return sf::Keyboard::Key::Numpad1;
    if (str == "Numpad2")
        return sf::Keyboard::Key::Numpad2;
    if (str == "Numpad3")
        return sf::Keyboard::Key::Numpad3;
    if (str == "Numpad4")
        return sf::Keyboard::Key::Numpad4;
    if (str == "Numpad5")
        return sf::Keyboard::Key::Numpad5;
    if (str == "Numpad6")
        return sf::Keyboard::Key::Numpad6;
    if (str == "Numpad7")
        return sf::Keyboard::Key::Numpad7;
    if (str == "Numpad8")
        return sf::Keyboard::Key::Numpad8;
    if (str == "Numpad9")
        return sf::Keyboard::Key::Numpad9;
    if (str == "F1")
        return sf::Keyboard::Key::F1;
    if (str == "F2")
        return sf::Keyboard::Key::F2;
    if (str == "F3")
        return sf::Keyboard::Key::F3;
    if (str == "F4")
        return sf::Keyboard::Key::F4;
    if (str == "F5")
        return sf::Keyboard::Key::F5;
    if (str == "F6")
        return sf::Keyboard::Key::F6;
    if (str == "F7")
        return sf::Keyboard::Key::F7;
    if (str == "F8")
        return sf::Keyboard::Key::F8;
    if (str == "F9")
        return sf::Keyboard::Key::F9;
    if (str == "F10")
        return sf::Keyboard::Key::F10;
    if (str == "F11")
        return sf::Keyboard::Key::F11;
    if (str == "F12")
        return sf::Keyboard::Key::F12;
    if (str == "F13")
        return sf::Keyboard::Key::F13;
    if (str == "F14")
        return sf::Keyboard::Key::F14;
    if (str == "F15")
        return sf::Keyboard::Key::F15;
    if (str == "Pause")
        return sf::Keyboard::Key::Pause;

    return sf::Keyboard::Key::Unknown;
}

std::string btnToStr(sf::Mouse::Button button)
{
    switch(button)
    {
        case sf::Mouse::Left: return "M Left";
        case sf::Mouse::Right: return "M Right";
        case sf::Mouse::Middle: return "M Middle";
        case sf::Mouse::XButton1: return "M X1";
        case sf::Mouse::XButton2: return "M X2";
        default: return "M Left";
    }
}

sf::Mouse::Button strToBtn(const std::string &str)
{
    if (str == "M Left")
        return sf::Mouse::Left;
    if (str == "M Right")
        return sf::Mouse::Right;
    if (str == "M Middle")
        return sf::Mouse::Middle;
    if (str == "M XButton1")
        return sf::Mouse::XButton1;
    if (str == "M XButton2")
        return sf::Mouse::XButton2;

    return sf::Mouse::Left;
}

bool isKey(const std::string &str)
{
    sf::Keyboard::Key key = strToKey(str);
    return key != sf::Keyboard::Unknown;
}

bool isButton(const std::string &str)
{
    sf::Mouse::Button btn = strToBtn(str);
    // strToBtn returns sf::Mouse::Left if it couldn't find right value
    return btn == sf::Mouse::Left ? str == "M Left" : true;
}

std::string logKeyToStr(const LogKey &logKey)
{
    if (logKey.keyboardKey)
        return keyToStr(*logKey.keyboardKey);
    else
        return btnToStr(*logKey.mouseButton);
}

char enumKeyToStr(sf::Keyboard::Key key)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) 
    ||  sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
    {
        switch (key)
        {
            case sf::Keyboard::A: return 'A';
            case sf::Keyboard::B: return 'B';
            case sf::Keyboard::C: return 'C';
            case sf::Keyboard::D: return 'D';
            case sf::Keyboard::E: return 'E';
            case sf::Keyboard::F: return 'F';
            case sf::Keyboard::G: return 'G';
            case sf::Keyboard::H: return 'H';
            case sf::Keyboard::I: return 'I';
            case sf::Keyboard::J: return 'J';
            case sf::Keyboard::K: return 'K';
            case sf::Keyboard::L: return 'L';
            case sf::Keyboard::M: return 'M';
            case sf::Keyboard::N: return 'N';
            case sf::Keyboard::O: return 'O';
            case sf::Keyboard::P: return 'P';
            case sf::Keyboard::Q: return 'Q';
            case sf::Keyboard::R: return 'R';
            case sf::Keyboard::S: return 'S';
            case sf::Keyboard::T: return 'T';
            case sf::Keyboard::U: return 'U';
            case sf::Keyboard::V: return 'V';
            case sf::Keyboard::W: return 'W';
            case sf::Keyboard::X: return 'X';
            case sf::Keyboard::Y: return 'Y';
            case sf::Keyboard::Z: return 'Z';
            case sf::Keyboard::Numpad0:
            case sf::Keyboard::Num0: return ')';
            case sf::Keyboard::Numpad1:
            case sf::Keyboard::Num1: return '!';
            case sf::Keyboard::Numpad2:
            case sf::Keyboard::Num2: return '@';
            case sf::Keyboard::Numpad3:
            case sf::Keyboard::Num3: return '#';
            case sf::Keyboard::Numpad4:
            case sf::Keyboard::Num4: return '$';
            case sf::Keyboard::Numpad5:
            case sf::Keyboard::Num5: return '%';
            case sf::Keyboard::Numpad6:
            case sf::Keyboard::Num6: return '^';
            case sf::Keyboard::Numpad7:
            case sf::Keyboard::Num7: return '&';
            case sf::Keyboard::Numpad8:
            case sf::Keyboard::Num8: return '*';
            case sf::Keyboard::Numpad9:
            case sf::Keyboard::Num9: return '(';
            case sf::Keyboard::LBracket: return '{';
            case sf::Keyboard::RBracket: return '}';
            case sf::Keyboard::Semicolon: return ':';
            case sf::Keyboard::Comma: return '<';
            case sf::Keyboard::Period: return '>';
            case sf::Keyboard::Quote: return '"';
            case sf::Keyboard::Slash: return '?';
            case sf::Keyboard::Tilde: return '~';
            case sf::Keyboard::Equal: return '+';
            case sf::Keyboard::Hyphen: return '_';
            case sf::Keyboard::Space: return ' ';
            case sf::Keyboard::Add: return '+';
            case sf::Keyboard::Subtract: return '-';
            case sf::Keyboard::Multiply: return '*';
            case sf::Keyboard::Divide: return '/';
            default: return 'a';
        }
    }
    else
    {
        switch (key)
        {
            case sf::Keyboard::A: return 'a';
            case sf::Keyboard::B: return 'b';
            case sf::Keyboard::C: return 'c';
            case sf::Keyboard::D: return 'd';
            case sf::Keyboard::E: return 'e';
            case sf::Keyboard::F: return 'f';
            case sf::Keyboard::G: return 'g';
            case sf::Keyboard::H: return 'h';
            case sf::Keyboard::I: return 'i';
            case sf::Keyboard::J: return 'j';
            case sf::Keyboard::K: return 'k';
            case sf::Keyboard::L: return 'l';
            case sf::Keyboard::M: return 'm';
            case sf::Keyboard::N: return 'n';
            case sf::Keyboard::O: return 'o';
            case sf::Keyboard::P: return 'p';
            case sf::Keyboard::Q: return 'q';
            case sf::Keyboard::R: return 'r';
            case sf::Keyboard::S: return 's';
            case sf::Keyboard::T: return 't';
            case sf::Keyboard::U: return 'u';
            case sf::Keyboard::V: return 'v';
            case sf::Keyboard::W: return 'w';
            case sf::Keyboard::X: return 'x';
            case sf::Keyboard::Y: return 'y';
            case sf::Keyboard::Z: return 'z';
            case sf::Keyboard::Numpad0:
            case sf::Keyboard::Num0: return '0';
            case sf::Keyboard::Numpad1:
            case sf::Keyboard::Num1: return '1';
            case sf::Keyboard::Numpad2:
            case sf::Keyboard::Num2: return '2';
            case sf::Keyboard::Numpad3:
            case sf::Keyboard::Num3: return '3';
            case sf::Keyboard::Numpad4:
            case sf::Keyboard::Num4: return '4';
            case sf::Keyboard::Numpad5:
            case sf::Keyboard::Num5: return '5';
            case sf::Keyboard::Numpad6:
            case sf::Keyboard::Num6: return '6';
            case sf::Keyboard::Numpad7:
            case sf::Keyboard::Num7: return '7';
            case sf::Keyboard::Numpad8:
            case sf::Keyboard::Num8: return '8';
            case sf::Keyboard::Numpad9:
            case sf::Keyboard::Num9: return '9';
            case sf::Keyboard::LBracket: return '[';
            case sf::Keyboard::RBracket: return ']';
            case sf::Keyboard::Semicolon: return ';';
            case sf::Keyboard::Comma: return ',';
            case sf::Keyboard::Period: return '.';
            case sf::Keyboard::Quote: return '\'';
            case sf::Keyboard::Slash: return '/';
            case sf::Keyboard::Tilde: return '`';
            case sf::Keyboard::Equal: return '=';
            case sf::Keyboard::Hyphen: return '-';
            case sf::Keyboard::Space: return ' ';
            case sf::Keyboard::Add: return '+';
            case sf::Keyboard::Subtract: return '-';
            case sf::Keyboard::Multiply: return '*';
            case sf::Keyboard::Divide: return '/';
            default: return 'a';
        }
    }
}
