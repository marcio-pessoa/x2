# x2 Mark I - Two Axes Platform

## Videos

Watch this projects videos on [YouTube Playlist].

[Change log](CHANGELOG.md)

[Bill of materials](BOM.md)

[Kanban](KANBAN.md)

## Mechanics

## Electronics

### Arduino

Technical specifications:

- Model: [Arduino Uno] R3

#### Pin designation table

 id | pin |   type    | direction |               description
----|-----|-----------|-----------|--------------------------------------------
  0 |   0 | digital   | input     | Serial communication
  1 |   1 | digital   | output    | Serial communication
  2 |   0 | interrupt | input     | Fan speed sensor
  3 |   3 | digital   | output    | Stepper 1 (activation/speed control)
  4 |   4 | digital   | output    | Stepper drive
  5 |   5 | digital   | output    | Stepper 2 (activation/speed control)
  6 |   6 | digital   | output    | Stepper 2 (activation/speed control)
  7 |   7 | digital   | output    | Stepper drive
  8 |   8 | digital   | output    | Stepper drive
  9 |   9 | PWM       | output    | X2_INTERFACE_D1 (PWM)
 10 |  10 | PWM       | output    | Fan speed control
 11 |  11 | digital   | output    | Stepper 1 (activation/speed control)
 12 |  12 | digital   | output    | Stepper drive
 13 |  13 | digital   | output    | OK LED (status LED)
 14 |   0 | analog    | input     | Temperature sensor (step motor driver)
 15 |   1 | A/D       | I/O       | X2_INTERFACE_D0 and X2_INTERFACE_A0
 16 |   2 | digital   | output    | Critical LED
 17 |   3 | digital   | input     | Warning LED
 18 |   4 | digital   | output    | Power relay
 19 |   5 | digital   | input     | Power supply DC detection

#### x2 interface pin designation table for Deep viewer

 id |      pin        |                  Deep viewer pins
----|-----------------|--------------------------------------------------------
  1 | 1               | +5 Vcc
  2 | X2_INTERFACE_D1 | Ultrasonic sensor echo pin
  3 | X2_INTERFACE_D0 | Ultrasonic sensor trigger pin / IDMS pin
  4 | 4               | GND

                                 +-----+
    +----[PWR]-------------------| USB |--+
    |                            +-----+  |
    |         GND/RST2  [ ][ ]            |
    |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |   C5
    |          5V/MISO2 [ ][ ]  A4/SDA[ ] |   C4
    |                             AREF[ ] |
    |                              GND[x] |
    | [ ]N/C                    SCK/13[x] |   B5
    | [ ]v.ref                 MISO/12[x] |   .
    | [ ]RST                   MOSI/11[x]~|   .
    | [ ]3V3    +---+               10[x]~|   .
    | [x]5v     | A |                9[x]~|   .
    | [x]GND   -| R |-               8[x] |   B0
    | [x]GND   -| D |-                    |
    | [x]Vin   -| U |-               7[x] |   D7
    |          -| I |-               6[x]~|   .
    | [x]A0    -| N |-               5[x]~|   .
    | [x]A1    -| O |-               4[x] |   .
    | [x]A2     +---+           INT1/3[x]~|   .
    | [x]A3                     INT0/2[x] |   .
    | [x]A4/SDA  RST SCK MISO     TX>1[x] |   .
    | [x]A5/SCL  [ ] [ ] [ ]      RX<0[x] |   D0
    |            [ ] [ ] [ ]              |
    |  UNO_R3    GND MOSI 5V  ____________/
    \_______________________/

Source: <http://busyducks.com/ascii-art-arduinos>

## Software

### Odd Scanner

Change to `oddscanner` work directory:

``` bash
cd oddscanner
```

Use `oddscanner` to generate a G-code file:

``` bash
./oddscanner 100 80 > example.gcode
```

Where:

- `oddscanner`: program file
- `100`: width
- `80`: height
- `example.gcode`: output G-code file

After file generation, use xC to run G-code file:

``` bash
xc cli --id x2 -p example.gcode -o example.csv
```

A CSV file will be generated, this file can be converted to a picture with `csv2png`:

``` bash
./csv2png -i example.csv -o example.png -d
```

---
[Arduino Uno]: https://www.arduino.cc/en/Main/arduinoBoardUno/#techspecs
[YouTube Playlist]: https://www.youtube.com/watch?v=V8XMp_TPpIc&list=PLs1JZHyVhlZUKraWlGDl2sSLEq4o7M2Q1
