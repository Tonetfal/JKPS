#include "../Headers/StringHelper.hpp"

// For more information
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Mouse.php 
// sf::Keyboard::A is used as unknown key in order to make the user change the regardless
// the fact that he didn't pass the idiot proof
// Same thing with mouse, I use sf::Mouse::Left as unknown button

std::string convertKeyToString(sf::Keyboard::Key key, bool saveToCfg)
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
        case sf::Keyboard::Num0: return "Num0";
        case sf::Keyboard::Num1: return "Num1";
        case sf::Keyboard::Num2: return "Num2";
        case sf::Keyboard::Num3: return "Num3";
        case sf::Keyboard::Num4: return "Num4";
        case sf::Keyboard::Num5: return "Num5";
        case sf::Keyboard::Num6: return "Num6";
        case sf::Keyboard::Num7: return "Num7";
        case sf::Keyboard::Num8: return "Num8";
        case sf::Keyboard::Num9: return "Num9";
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
        case sf::Keyboard::LBracket: return "LBracket";
        case sf::Keyboard::RBracket: return "RBracket";
        case sf::Keyboard::Semicolon: return (saveToCfg ? "Semicolon" : ":");
        case sf::Keyboard::Comma: return (saveToCfg ? "Comma" : ",");
        case sf::Keyboard::Period: return (saveToCfg ? "Period" : ".");
        case sf::Keyboard::Quote: return (saveToCfg ? "Quote" : "\"");
        case sf::Keyboard::Slash: return (saveToCfg ? "Slash" : "/");
        case sf::Keyboard::Backslash: return (saveToCfg ? "Backslash" : "\\");
        case sf::Keyboard::Tilde: return (saveToCfg ? "Tilde" : "~");
        case sf::Keyboard::Hyphen: return (saveToCfg ? "Hyphen" : "-");
        case sf::Keyboard::Space: return "Space";
        case sf::Keyboard::Enter: return "Enter";
        case sf::Keyboard::Backspace: return "Backspace";
        case sf::Keyboard::Tab: return "Tab";
        case sf::Keyboard::PageUp: return "PageUp";
        case sf::Keyboard::PageDown: return "PageDown";
        case sf::Keyboard::End: return "End";
        case sf::Keyboard::Home: return "Home";
        case sf::Keyboard::Insert: return "Insert";
        case sf::Keyboard::Delete: return "Delete";
        case sf::Keyboard::Add: return (saveToCfg ? "Add" : "+");
        case sf::Keyboard::Subtract: return (saveToCfg ? "Subtract" : "-");
        case sf::Keyboard::Multiply: return (saveToCfg ? "Multiply" : "*");
        case sf::Keyboard::Divide: return (saveToCfg ? "Divide" : "/");
        case sf::Keyboard::Left: return "Left";
        case sf::Keyboard::Right: return "Right";
        case sf::Keyboard::Up: return "Up";
        case sf::Keyboard::Down: return "Down";
        case sf::Keyboard::Numpad0: return "Numpad0";
        case sf::Keyboard::Numpad1: return "Numpad1";
        case sf::Keyboard::Numpad2: return "Numpad2";
        case sf::Keyboard::Numpad3: return "Numpad3";
        case sf::Keyboard::Numpad4: return "Numpad4";
        case sf::Keyboard::Numpad5: return "Numpad5";
        case sf::Keyboard::Numpad6: return "Numpad6";
        case sf::Keyboard::Numpad7: return "Numpad7";
        case sf::Keyboard::Numpad8: return "Numpad8";
        case sf::Keyboard::Numpad9: return "Numpad9";
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
        default: return "A";
    }
}

sf::Keyboard::Key convertStringToKey(const std::string &str)
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

    return sf::Keyboard::Key::A;
}

std::string convertButtonToString(sf::Mouse::Button button)
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

sf::Mouse::Button convertStringToButton(const std::string &str)
{
    if (str == "Left")
        return sf::Mouse::Left;
    if (str == "Right")
        return sf::Mouse::Right;
    if (str == "Middle")
        return sf::Mouse::Middle;
    if (str == "XButton1")
        return sf::Mouse::XButton1;
    if (str == "XButton2")
        return sf::Mouse::XButton2;

    return sf::Mouse::Left;
}