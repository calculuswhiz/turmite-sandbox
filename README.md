# turmite-sandbox
A fairly versatile turmite generator

Uses opengl to simulate turmites.

Screenshot (from experiment2.ini:

![Golden spiral](https://raw.githubusercontent.com/calculuswhiz/turmite-sandbox/master/spiral.png)

To use:
- `make`
- Then `./main [ini file]`. Choose an .ini file as input.
- Watch your turmites go!
- To spawn a new one in, click at a spot to copy an existing turmite.
- If you have more than one turmite in your ini file, you can select it using the mouse wheel (see terminal output for the name).
- To speed up/slow down, press `+`/`-` respectively.
- To quit, press `q`.
- To pause, press `p`.
- I've provided a couple ini files for you in `behavior_files/`.

The ini file is structured as follows:

```
#Comment

# This signals the start of an automaton. (More than one is ok per file.)
[Automaton]

#Name
[Name] "name goes here surrounded by junk characters"

#Color
[Color] red green blue
# Values provided on a scale of 0-1 (floats)

# Initialization:
[Start]   xpixel   ypixel  statenumber   orientation
# xpixel - the starting x pixel value
# ypixel - starting y pixel value
# statenumber - the index of the starting state (see below)
# orientation - N S E or W (short for North South East West). Abbreviation only.

#Push state onto state vector:
[State]   offnext onnext offturn onturn offpaint onpaint
# offnext - next state if current cell is off
# onnext - next state if current cell is on
# offturn - L R N U (short for Left Right None U-turn). Abbreviation only. Where to turn if cell is off
# onturn - Where to turn is cell is on
# offpaint - 1 or 0. Turn cell on (1) or off (0) if cell is off.
# onpaint - 1 or 0. Turn cell on or off if cell is on.
# Issue [State] again to push another state

#Finally, finish constructing the turmite. To construct another, simply copy an [Automaton][/Automaton] block.
[/Automaton]
```

