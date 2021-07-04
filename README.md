# JKPS
JKPS is a simple program that shows keystates, keys per second, total keys and current beats per minute. The program is intended to be used in rythm games such as osu!, stepmania etc.

# Preview
![](Preset-1.gif)

It supports up to 10 keys. Buttons, font, color, background, animation and other things can be changed in the config.

# Usage
Modifications can be done in edit mode, which can be opened/closed by pressing Ctrl + Q.
If you want to add/remove a key press +/-, but if you want to change a key, click with left mouse button or with the same key, then the key will be selected, and it will change its own color, then press the key that will replace the old one.
These actions look like:

![](modification-demo.gif)


Other settings can be changed in the JKPS.cfg file, that will be generated in the same directory after you run the program. If there is already one, nothing will be generated, but if there is any error, error log will be generated.
When you change anything in the config you must put one space between semicolon and the rest of the data. If this data are some numbers or keys, they must be separated with a comma without any extra space between. If it is a path to a texture or font, it must be written without any quotes.
To exit the program just click Ctrl + W or close it from the tray.

To know what keys and buttons can be written in the config check this [link](https://gist.github.com/JekiTheMonkey/c40b346907ca0e3e8127f4fb0b5af1d5).

# Notes
- Capture the program as a game in OBS, not as a window, until you want that the graphics will be updated
- To make the program work with green screen just change the "Background color" to 0,177,64.
- BPM is not calculated immediately, it will be displayed correctly only after a second and 9 frames. To get the right BPM you must stream at least BPM / 15 notes.

# [Themes](https://gist.github.com/JekiTheMonkey/727f57dcdecb76480b982f0fe479c5c1)
![](Preset-4.gif)

![](Preset-1.gif)

![](Preset-2.gif)

![](Preset-3.gif)

# License
Licensed under the [MIT License](LICENSE).
