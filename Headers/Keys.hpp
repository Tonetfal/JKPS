#pragma once
// For more information
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php
// https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Mouse.php 

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

// This enum needs to make Button class work with both mouse
// buttons and keyboard keys
enum Keys
{
    Unknown = -1, A = 0, B, C,
    D, E, F, G,
    H, I, J, K,
    L, M, N, O,
    P, Q, R, S,
    T, U, V, W,
    X, Y, Z, Num0,
    Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8,
    Num9, Escape, LControl, LShift,
    LAlt, LSystem, RControl, RShift,
    RAlt, RSystem, Menu, LBracket,
    RBracket, Semicolon, Comma, Period,
    Quote, Slash, Backslash, Tilde,
    Equal, Hyphen, Space, Enter,
    Backspace, Tab, PageUp, PageDown,
    End, Home, Insert, Delete,
    Add, Subtract, Multiply, Divide,
    Left, Right, Up, Down,
    Numpad0, Numpad1, Numpad2, Numpad3,
    Numpad4, Numpad5, Numpad6, Numpad7,
    Numpad8, Numpad9, F1, F2,
    F3, F4, F5, F6,
    F7, F8, F9, F10,
    F11, F12, F13, F14,
    F15, Pause, Dash = Hyphen,
    BackSpace = Backspace, BackSlash = Backslash, 
    SemiColon = Semicolon, Return = Enter,
    MLeft = 101, MRight, MMiddle, MXButton1,
    MXButton2
};

sf::Keyboard::Key convertKeysToKey(Keys key);
sf::Mouse::Button convertKeysToButton(Keys key);