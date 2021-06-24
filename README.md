# KPS
KPS is a simple program that shows keystates, keys per second, total keys and current beats per minute. The program is intended to be used in rythm games such as osu!, stepmania etc.

# Preview
![](modification demo.gif)

![](https://i.imgur.com/Oj5TnQZ.gif)

It supports up to 10 keys. Buttons, font, color, background, animation and other things can be changed in the config.

# Download
TODO

# Usage
To modify keys in program press Ctrl + Q, then click the button to change with the **left mouse button** or with the **same key** to select it, then press the key that will replace the old one. To deselect a button click with left mouse button on it. To exit from edit mode press Ctrl + Q.

To exit from program press Ctrl + W or close it from the tray.

To edit anything else use KPS.cfg. Any settings value must have a space after semicolon, and if there are several values, such as keys or some digits, they must be separated with comma and do not have any space between.

To know what keys can be written in the config check [SFML documentation](https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php). Arrow keys can be written like so - UpArrow, DownArrow etc.

To edit font, button, animation or background texture write the path (without quotes) to the asset. 

# Notes
BPM is not accurate, it will be calcuted correctly only after a second and 7 frames

# License
TODO
