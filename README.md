# JKPS
JKPS is a simple program that shows keystates, keys per second, total keys and current beats per minute. The program is intended to be used in rhythm games such as osu!, stepmania etc.

# Preview
![](Media/Preview-White-orange.gif)

It supports up to 10 keyboard keys and 5 mouse buttons.

# Usage
To add or remove a keyboard key press Ctrl + "+" or "-". 

To add or remove a mouse button press Ctrl + "<" or ">". 

To change a key click with right mouse button on the graphical button to open key selector, and then click the required key on the keyboard. If you want to change the text on the button, then write it in the second box.

These actions look like:

![](Media/modification-demo.gif)

To change how the program looks press Ctrl + A to open graphical menu. If you want to change texture or font, then you have to provide it in the same folder where the program is, and then write the whole asset name (extension included) in the graphical menu, and then press Enter or Refresh button.

The program will generate a config named JKPS.cfg, that holds every parameter of the program.

Entire explanation of the config you can find [here](https://gist.github.com/JekiTheMonkey/06c7b7dc0401729c2574a04442b19225).

# Hotkeys

Click with right mouse button on the key to edit it

Ctrl + "+/-" - Add/remove keyboard keys

Ctrl + "</>" - Add/remove mouse buttons

Ctrl + W - Close the program

Ctrl + X - Reset all the statistics

Ctrl + A - Open graphical menu

Ctrl + K - Open an extra window with KPS

Alt (hold) - Show opposite button values

# Graphical menu

![](Media/menu-demo.gif)

# Extra window with KPS

![](Media/Preview-kps-window.gif)

# Notes
- Capture the program as a game in OBS, not as a window.
- To make the program work with green screen just write in the "Background texture" parameter "GreenscreenBG.png" and press Enter or Refresh button.
- The palette in menu will not adjust own color to the parameter one.
- If you change anything in the config file, exit the program first.
- BPM is not calculated immediately, it will be displayed correctly only after a second and 9 frames. To get the right BPM you must stream at least "BPM / 15" notes.

# [Themes](https://gist.github.com/JekiTheMonkey/727f57dcdecb76480b982f0fe479c5c1)

![](Media/Preview-Red-violet.gif)

![](Media/Preview-Dark-minimalistic-2-nano.gif)

![](Media/Preview-Dark-minimalistic-2.gif)

![](Media/Preview-Medieval.gif)

![](Media/Preview-Modern-purple.gif)

![](Media/Preview-Dark-minimalistic-nano.gif)

![](Media/Preview-Dark-minimalistic.gif)

![](Media/Preview-White-orange.gif)

![](Media/Preview-Dark-violet.gif)

![](Media/Preview-Dark-classic.gif)

# Links

[Download](https://github.com/JekiTheMonkey/JKPS/releases/)

[Themes](https://gist.github.com/JekiTheMonkey/727f57dcdecb76480b982f0fe479c5c1)

# License
Licensed under the [MIT License](LICENSE).
