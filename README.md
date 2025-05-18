## Final Palm Line - Switch Fightstick Code (Atmega32U4) 

This is a fork of [Switch Fightstick Code (Atmega32U4)](https://github.com/fluffymadness/ATMega32U4-Switch-Fightstick).

The goal of this repo was to take an existing Arduino Pro Micro based Nintendo Switch Arcade Stick and strip it off the regular Arcade Stick Stuff (DPAD Modes, Analog modes, SOCD and such) to create a proper base for building something like the third-party Fanta Palm Controller for Final Bar Line and Groove Coaster.

The led rings are there for fun.

This is intended for Nintendo Switch as for the time of writing, but since the controller is presented as the HORI Pokken Tournament controller, it's very likely that Steam and the PS5 can pick it up without a hitch.

Please refer to the building instructions to compile it and flash it to your ProMicro or ProMicro clone. Should work out of the box with the proper hardware and wiring.

### What's in 

- Complete working project with some nice LED Lighting. 
- Mapped all the required buttons and the two levers for Theatrhythm Final Bar Line and Groove Coaster

### What's missing 

- Some buttons aren't mapped because the ProMicro has only 18 inputs and I wanted the LED lighting. The missing buttons are -, L3, R3, ZL and ZR. 
- No way of using the missing buttons through combos. 
- The LED lighting cannot be modified as of now and the code is a bit messy because I ported it from my initial naive project and it worked a bit different than the project I ended up forking. 
- Every typical FIght Stick functionality (SOCD, DPAD and Analog modes) has been removed in favor of simplicity and for the lack of a proper use case in the intended purpose of this fork.

### Pin Mapping 

Please refer to the Pin Mapping section of the `finalpalm.ino` file to remap your inputs as needed (or rewire them)
```
// Pin Mapping
#define LED_RING1_PIN 0
#define LED_RING2_PIN 1

#define PIN_LUP 6
#define PIN_LDOWN 7
#define PIN_LLEFT 9
#define PIN_LRIGHT 8

#define PIN_RUP 14
#define PIN_RDOWN 15
#define PIN_RLEFT 16
#define PIN_RRIGHT 10

#define PIN_A 18
#define PIN_B 19
#define PIN_X 4
#define PIN_Y 5

#define PIN_L 2
#define PIN_R 20

#define PIN_PLUS 3
#define PIN_HOME 21
```

### Building Instructions

- Download Arduino IDE, 
- Download the Bounce2 Library inside the Arduino IDE
- Download Arduino Lufa from https://github.com/Palatis/Arduino-Lufa and follow the instructions on it's repository
- Build and Flash for your ATMEGA32U4 Board
- Have Fun


### Credits 

Special thanks to shinyquagsire's and progmem's reverseengineering work for the pokken tournament controller. (https://github.com/progmem/Switch-Fightstick)

Dean Camera for the LUFA Library

zlittell, msf-xinput was very helpful for starting.

thomasfredericks for the Bounce2Lib.

fluffymadness for the ATMega32U4-Switch-Fightstick implementation, of course


