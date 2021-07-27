# JKPS
JKPS is a simple program that shows keystates, keys per second, total keys and current beats per minute. The program is intended to be used in rythm games such as osu!, stepmania etc.

# Preview
![](Media/Preview-White-orange.gif)

It supports up to 10 keys. Buttons, fonts, colors, background, animations and other things can be changed in the config.

# Usage
Modifications can be done in edit mode, which can be opened/closed by pressing Ctrl + Q.
If you want to add/remove a key press +/-, but if you want to change a key, click with left mouse button or with the same key, then the key will be selected, and it will change its color, then press the key that will replace the old one.
These actions look like:

![](Media/modification-demo.gif)

When you are done with editing exit from edit mode by pressing Ctrl + Q.

Other settings can be changed in the JKPS.cfg file, that will be generated in the same directory after you run the program. If there is already one, nothing will be generated, but if there is any error, error log will be generated.

When you change anything in the config you must put one space between colon and the rest of the data. If this data are some numbers or keys, they must be separated with a comma without any extra space between. If it is a path to a texture or font, it must be written without any quotes, and must include the extension, such as png, jpg etc.

To know what keys and buttons can be written in the config check this [link](https://gist.github.com/JekiTheMonkey/c40b346907ca0e3e8127f4fb0b5af1d5).

Entire explanation of the config you can find [here](https://gist.github.com/JekiTheMonkey/06c7b7dc0401729c2574a04442b19225).

# Hotkeys

Click with right mouse button on the key to edit it

Ctrl + W - Close the program

Ctrl + X - Reset all the statistics

Ctrl + A - Open graphical menu

Ctrl + K - Open an extra window with KPS

Ctrl (hold) - Show opposite button values

# Extra window with KPS

This window can be used with greenscreen in order to show only the text.

![](Media/Preview-kps-window.gif)

# Notes
- Capture the program as a game in OBS, not as a window, until you want that the graphics will be updated
- To make the program work with green screen just change the "Background color" to 0,177,64.
- BPM is not calculated immediately, it will be displayed correctly only after a second and 9 frames. To get the right BPM you must stream at least BPM / 15 notes.

# [Themes](https://gist.github.com/JekiTheMonkey/727f57dcdecb76480b982f0fe479c5c1)

![](Media/Preview-Medieval.gif)

![](Media/Preview-Modern-purple.gif)

![](Media/Preview-Dark-minimalistic.gif)

![](Media/Preview-White-orange.gif)

![](Media/Preview-Dark-violet.gif)

![](Media/Preview-Dark-classic.gif)

# Links

[Download](https://github.com/JekiTheMonkey/JKPS/releases/)

[Themes](https://gist.github.com/JekiTheMonkey/727f57dcdecb76480b982f0fe479c5c1)

# License
Licensed under the [MIT License](LICENSE).
