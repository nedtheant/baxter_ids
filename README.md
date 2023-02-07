# baxter_ids
Code to run a reader and setup software for an id card reader for keeping track of attendance at events.

# HOW TO USE
asdkjhasd

# HOW TO BUILT HARDWARE
PINS:
RF:
RST 9 
SDA 10
MOSI 11
MISO 12
SCK 13

Green Led = 8
Red Led = 7

# LED SIGNALS
- 2 green flash when turning on.
- 2 red for failed scan
- green flash when successful scan (followed by 3 red when not recognised uid (triggered from software.))
- red flsh when duplicate scan
- 10 reds when eeprom runs out.