CNCLib / CNCStepper
======

Build your own CNC machine/laser with *CNCLib* and *CNCStepper*.<br />
Both projects are on github.

https://github.com/aiten/CNCLib <br />
https://github.com/aiten/CNCStepper


CNCStepper
======

Use the windows CNCLib application to control your CNC machine.

see folder: *Sketch*

### Hardware

-   8 bit Arduino: Uno, Nano, Duemilanove, Mega 2560, Pro

-   32 bit Arduino: Due, zero, M0

-   esp32 super mini, e.g. https://www.espboards.dev/esp32/esp32-c6-super-mini/ 

-   Arduino Shields: CNCShield, Ramps 1.4, Ramps FD

-   Drivers: A4988, DRV8825, SMC 800, L298 and TB6560

-   LCD 12864 (using u8glib) and SD

-   can be used with servos (see sketch IRobot)

### Software

-   Stepper motor library with acceleration support

-   GCode Interpreter basic and extended (Arduino dependent)

-   Axis supported: 4 or 6, depending on the hardware

-   LCD Support

-   SD Support

-   HPGL Interpreter sample


### Installation procedure

-    clone or download repository
-    use Arduino IDE >= 2.0.0
-    set "Sketchbook location" to <basedir>/Sketch (Arduino IDE Menu: File->Preferences) - this is necessary to find the libraries
-    install the library u8g2 (Menu: Sketch->Include library->Manage libraries)
-    for arduino due and/or zero install the board (Menu: Tools->Board: xxx->Boards Manager...)
-    open sketch - MiniCNC, MaxiCNC, ...
-    configure machine (see: configuration.h)
-    select Arduino (Uno, Due, ...) and USB port
-    download sketch
-    configure machine (EEprom) using CNCLib (https://github.com/aiten/CNCLib)
