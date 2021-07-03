#include "../Headers/StringHelper.hpp"


// Keys::A is used as unknown key in order to make the user change the key
// regardless the fact that he didn't pass the idiot proof
std::string convertKeyToString(Keys key, bool saveToCfg)
{
    switch (key)
    {
        case Keys::A: return "A";
        case Keys::B: return "B";
        case Keys::C: return "C";
        case Keys::D: return "D";
        case Keys::E: return "E";
        case Keys::F: return "F";
        case Keys::G: return "G";
        case Keys::H: return "H";
        case Keys::I: return "I";
        case Keys::J: return "J";
        case Keys::K: return "K";
        case Keys::L: return "L";
        case Keys::M: return "M";
        case Keys::N: return "N";
        case Keys::O: return "O";
        case Keys::P: return "P";
        case Keys::Q: return "Q";
        case Keys::R: return "R";
        case Keys::S: return "S";
        case Keys::T: return "T";
        case Keys::U: return "U";
        case Keys::V: return "V";
        case Keys::W: return "W";
        case Keys::X: return "X";
        case Keys::Y: return "Y";
        case Keys::Z: return "Z";
        case Keys::Num0: return "Num0";
        case Keys::Num1: return "Num1";
        case Keys::Num2: return "Num2";
        case Keys::Num3: return "Num3";
        case Keys::Num4: return "Num4";
        case Keys::Num5: return "Num5";
        case Keys::Num6: return "Num6";
        case Keys::Num7: return "Num7";
        case Keys::Num8: return "Num8";
        case Keys::Num9: return "Num9";
        case Keys::Escape: return "Escape";
        case Keys::LControl: return "LControl";
        case Keys::LShift: return "LShift";
        case Keys::LAlt: return "LAlt";
        case Keys::LSystem: return "LSystem";
        case Keys::RControl: return "RControl";
        case Keys::RShift: return "RShift";
        case Keys::RAlt: return "RAlt";
        case Keys::RSystem: return "RSystem";
        case Keys::Menu: return "Menu";
        case Keys::LBracket: return "LBracket";
        case Keys::RBracket: return "RBracket";
        case Keys::Semicolon: return (saveToCfg ? "Semicolon" : ":");
        case Keys::Comma: return (saveToCfg ? "Comma" : ",");
        case Keys::Period: return (saveToCfg ? "Period" : ".");
        case Keys::Quote: return (saveToCfg ? "Quote" : "\"");
        case Keys::Slash: return (saveToCfg ? "Slash" : "/");
        case Keys::Backslash: return (saveToCfg ? "Backslash" : "\\");
        case Keys::Tilde: return (saveToCfg ? "Tilde" : "~");
        case Keys::Hyphen: return (saveToCfg ? "Hyphen" : "-");
        case Keys::Space: return "Space";
        case Keys::Enter: return "Enter";
        case Keys::Backspace: return "Backspace";
        case Keys::Tab: return "Tab";
        case Keys::PageUp: return "PageUp";
        case Keys::PageDown: return "PageDown";
        case Keys::End: return "End";
        case Keys::Home: return "Home";
        case Keys::Insert: return "Insert";
        case Keys::Delete: return "Delete";
        case Keys::Add: return (saveToCfg ? "Add" : "+");
        case Keys::Subtract: return (saveToCfg ? "Subtract" : "-");
        case Keys::Multiply: return (saveToCfg ? "Multiply" : "*");
        case Keys::Divide: return (saveToCfg ? "Divide" : "/");
        case Keys::Left: return "Left";
        case Keys::Right: return "Right";
        case Keys::Up: return "Up";
        case Keys::Down: return "Down";
        case Keys::Numpad0: return "Numpad0";
        case Keys::Numpad1: return "Numpad1";
        case Keys::Numpad2: return "Numpad2";
        case Keys::Numpad3: return "Numpad3";
        case Keys::Numpad4: return "Numpad4";
        case Keys::Numpad5: return "Numpad5";
        case Keys::Numpad6: return "Numpad6";
        case Keys::Numpad7: return "Numpad7";
        case Keys::Numpad8: return "Numpad8";
        case Keys::Numpad9: return "Numpad9";
        case Keys::F1: return "F1";
        case Keys::F2: return "F2";
        case Keys::F3: return "F3";
        case Keys::F4: return "F4";
        case Keys::F5: return "F5";
        case Keys::F6: return "F6";
        case Keys::F7: return "F7";
        case Keys::F8: return "F8";
        case Keys::F9: return "F9";
        case Keys::F10: return "F10";
        case Keys::F11: return "F11";
        case Keys::F12: return "F12";
        case Keys::F13: return "F13";
        case Keys::F14: return "F14";
        case Keys::F15: return "F15";
        case Keys::Pause: return "Pause";
        case Keys::MLeft: return "M Left";
        case Keys::MRight: return "M Right";
        case Keys::MMiddle: return "M Middle";
        case Keys::MXButton1: return "M X1";
        case Keys::MXButton2: return "M X2";
        default: return "A";
    }
}

Keys convertStringToKey(const std::string &str)
{
    if (str == "A")
        return Keys::A;
    if (str == "B")
        return Keys::B;
    if (str == "C")
        return Keys::C;
    if (str == "D")
        return Keys::D;
    if (str == "E")
        return Keys::E;
    if (str == "F")
        return Keys::F;
    if (str == "G")
        return Keys::G;
    if (str == "H")
        return Keys::H;
    if (str == "I")
        return Keys::I;
    if (str == "J")
        return Keys::J;
    if (str == "K")
        return Keys::K;
    if (str == "L")
        return Keys::L;
    if (str == "M")
        return Keys::M;
    if (str == "N")
        return Keys::N;
    if (str == "O")
        return Keys::O;
    if (str == "P")
        return Keys::P;
    if (str == "Q")
        return Keys::Q;
    if (str == "R")
        return Keys::R;
    if (str == "S")
        return Keys::S;
    if (str == "T")
        return Keys::T;
    if (str == "U")
        return Keys::U;
    if (str == "V")
        return Keys::V;
    if (str == "W")
        return Keys::W;
    if (str == "X")
        return Keys::X;
    if (str == "Y")
        return Keys::Y;
    if (str == "Z")
        return Keys::Z;
    if (str == "Num0")
        return Keys::Num0;
    if (str == "Num1")
        return Keys::Num1;
    if (str == "Num2")
        return Keys::Num2;
    if (str == "Num3")
        return Keys::Num3;
    if (str == "Num4")
        return Keys::Num4;
    if (str == "Num5")
        return Keys::Num5;
    if (str == "Num6")
        return Keys::Num6;
    if (str == "Num7")
        return Keys::Num7;
    if (str == "Num8")
        return Keys::Num8;
    if (str == "Num9")
        return Keys::Num9;
    if (str == "Escape")
        return Keys::Escape;
    if (str == "LControl")
        return Keys::LControl;
    if (str == "LShift")
        return Keys::LShift;
    if (str == "LAlt")
        return Keys::LAlt;
    if (str == "LSystem")
        return Keys::LSystem;
    if (str == "RControl")
        return Keys::RControl;
    if (str == "RShift")
        return Keys::RShift;
    if (str == "RAlt")
        return Keys::RAlt;
    if (str == "RSystem")
        return Keys::RSystem;
    if (str == "Menu")
        return Keys::Menu;
    if (str == "LBracket")
        return Keys::LBracket;
    if (str == "RBracket")
        return Keys::RBracket;
    if (str == "Semicolon")
        return Keys::Semicolon;
    if (str == "Comma")
        return Keys::Comma;
    if (str == "Period")
        return Keys::Period;
    if (str == "Quote")
        return Keys::Quote;
    if (str == "Slash")
        return Keys::Slash;
    if (str == "Backslash")
        return Keys::Backslash;
    if (str == "Tilde")
        return Keys::Tilde;
    if (str == "Hyphen")
        return Keys::Hyphen;
    if (str == "Space")
        return Keys::Space;
    if (str == "Enter")
        return Keys::Enter;
    if (str == "Backspace")
        return Keys::Backspace;
    if (str == "Tab")
        return Keys::Tab;
    if (str == "PageUp")
        return Keys::PageUp;
    if (str == "PageDown")
        return Keys::PageDown;
    if (str == "End")
        return Keys::End;
    if (str == "Home")
        return Keys::Home;
    if (str == "Insert")
        return Keys::Insert;
    if (str == "Delete")
        return Keys::Delete;
    if (str == "Add")
        return Keys::Add;
    if (str == "Subtract")
        return Keys::Subtract;
    if (str == "Multiply")
        return Keys::Multiply;
    if (str == "Divide")
        return Keys::Divide;
    if (str == "Left")
        return Keys::Left;
    if (str == "Right")
        return Keys::Right;
    if (str == "Up")
        return Keys::Up;
    if (str == "Down")
        return Keys::Down;
    if (str == "Numpad0")
        return Keys::Numpad0;
    if (str == "Numpad1")
        return Keys::Numpad1;
    if (str == "Numpad2")
        return Keys::Numpad2;
    if (str == "Numpad3")
        return Keys::Numpad3;
    if (str == "Numpad4")
        return Keys::Numpad4;
    if (str == "Numpad5")
        return Keys::Numpad5;
    if (str == "Numpad6")
        return Keys::Numpad6;
    if (str == "Numpad7")
        return Keys::Numpad7;
    if (str == "Numpad8")
        return Keys::Numpad8;
    if (str == "Numpad9")
        return Keys::Numpad9;
    if (str == "F1")
        return Keys::F1;
    if (str == "F2")
        return Keys::F2;
    if (str == "F3")
        return Keys::F3;
    if (str == "F4")
        return Keys::F4;
    if (str == "F5")
        return Keys::F5;
    if (str == "F6")
        return Keys::F6;
    if (str == "F7")
        return Keys::F7;
    if (str == "F8")
        return Keys::F8;
    if (str == "F9")
        return Keys::F9;
    if (str == "F10")
        return Keys::F10;
    if (str == "F11")
        return Keys::F11;
    if (str == "F12")
        return Keys::F12;
    if (str == "F13")
        return Keys::F13;
    if (str == "F14")
        return Keys::F14;
    if (str == "F15")
        return Keys::F15;
    if (str == "Pause")
        return Keys::Pause;
    if (str == "M Left")
        return Keys::MLeft;
    if (str == "M Right")
        return Keys::MRight;
    if (str == "M Middle")
        return Keys::MMiddle;
    if (str == "M X1")
        return Keys::MXButton1;
    if (str == "M X2")
        return Keys::MXButton2;

    return Keys::A;
}