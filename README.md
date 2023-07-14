 # baxter_ids
Code to run a reader and setup software for an id card reader for keeping track of attendance at events.


# HOW TO USE
### Scanning people in
1. Turn on switch at rear of reader and press the button ontop to turn it on. 
2. The LED should flash green and then be a steady blue.
3. Put the barcode of your ID card on the acrylic at the front of the reader. 
4. The LED will go green for a good scan, and red for a bad one (just rescan usually).
5. You can turn the scanner off and attendances will still be saved.
6. Max attendences stored is ~256. If this happens the LED will flash RED for a while after a scan.
### Getting data off the device
1. Plug the reader into your PC USB port. You will have to find which port this is.
2. You can look up how to do this ([Or use this link](https://au.mathworks.com/help/supportpkg/arduinoio/ug/find-arduino-port-on-windows-mac-and-linux.html)), on windows it'll be something like "COM3", on mac it'll be something like "dev/tty-USB1".
3. In `/software/main.py`, update `PORT = "YOUR PORT HERE"` near the top of the file with whatever you found in step 2.
4. Save this file.
5. Run this file, you're gonna need to install [python3](https://www.python.org/downloads/) and [pyserial](https://pypi.org/project/pyserial/).
6. Follow the instructions in the file. (get will save an .csv file to the `software` directory with the current date as the name. Be careful as you can overwrite attendances if you do multiple on the same day.)
7. Make sure you clear attendances before the next use.

# LED SIGNALS
- 2 green flashes when turning on.
- 2 red flashes for failed scan.
- Green flash when successful scan (including duplicate scan).
- 10 reds when eeprom runs out (Too many ids scanned).

# HOW TO BUILD THE THING (Or buy off me for $1000)
mmm just buy it off me...