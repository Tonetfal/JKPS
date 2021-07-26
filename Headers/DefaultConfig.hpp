#pragma once

#include <iostream>

static std::string DefaultConfigString = 
R"(# Download link:
# https://github.com/JekiTheMonkey/JKPS/releases 
# My Discord: Jeki the Monkey from München#3932
# If any settings will not be found default parameter value will be used
#
# Read this page to know how to edit keys and mouse buttons from config
# https://gist.github.com/JekiTheMonkey/c40b346907ca0e3e8127f4fb0b5af1d5 

[Keys]
Keys: Z,X

[Mouse]
Buttons: No

[Statistics text]
Statistics text distance: 5
Space between buttons and statistics: 15
Statistics text font: Default
Statistics text color: 255,255,255
Statistics text character size: 14
Statistics text bold: false
Statistics text italic: false
Show statistics text: true
Show max KPS on separate line: true
Show BPM: true

[Buttons text]
Buttons text font: Default
Buttons text color: 255,255,255
Buttons text character size: 15
Buttons text width: 0
Buttons text height: 0
Buttons text horizontal bounds: 4
Buttons text vertical bounds: 4
Buttons text bold: false
Buttons text italic: false
Only show set keys: false
Show key counters: true

[Button graphics]
Button distance: 6
Button texture: Default
Button texture size: 60,60
Button texture color: 30,30,30

[Animation graphics]
Animation style: 1
Animation texture: Default
Animation velocity: 5
# Style 1
Animation scale on click: 1
Animation color: 255,180,0
# Style 2
Animation offset: 3

[Background]
Background texture: Default
Background color: 170,170,170
Scale background texture if it does not fit: true

[Edit mode]
Edit mode alert color: 255,180,0
Highlighted text button color: 210,30,210

[Main window]
Window title bar: false
Window bonus size top: 6
Window bonus size bottom: 6
Window bonus size left: 6
Window bonus size right: 130

[KPS window]
Enable from start: false
Window size: 300,300
KPS Background color: 0,177,64
Text font: Default
Text size: 130
Number size: 100
KPS extra window distance from top: 20
KPS extra window distance between text: 50

[Theme developer]
Value to multiply on click: 1
)";