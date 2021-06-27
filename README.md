# KPS
KPS is a simple program that shows keystates, keys per second, total keys and current beats per minute. The program is intended to be used in rythm games such as osu!, stepmania etc.

# Preview
![](Preset-1.gif)

It supports up to 10 keys. Buttons, font, color, background, animation and other things can be changed in the config.

# Usage
Modifications can be done in edit mode, which can be open/closed by pressing Ctrl + Q.
If you want to add/remove a key press +/-, but if you want to change a key, click with left mouse button or with the same key, then the key will be selected, and it will change its own color, then press the key that will replace the old one.
These actions look like:

![](modification-demo.gif)


Other settings can be changed in the KPS.cfg file, that will be generated in the same directory after you run the program. If there is already one, nothing will be generated, but if there is any error, error log will be generated.
When you change anything in the config you must put one space between semicolon and the rest of the data. If this data are some numbers or keys, they must be separated with a comma without any extra space between. If it is a path to a texture or font, it must be written without any quotes.
To exit the program just click Ctrl + W or close it from the tray.

To know what keys and buttons can be written in the config check SFML documentation - [Keyboard keys](https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php), [Mouse buttons](https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Mouse.php). Arrow keys can be written like so - UpArrow, DownArrow etc.

# Notes
- BPM is not calculated immediately, it will be displayed correctly only after a second and 9 frames.
- If you want to have non-square buttons, you have to set the path to the texture with your button, but also you have to change the animation texture, because it is still squared, - just put the same texture, but white one.

# Themes
![](Preset-1.gif)

![](Preset-2.gif)

![](Preset-3.gif)

# License
Licensed under the [MIT License](LICENSE).
