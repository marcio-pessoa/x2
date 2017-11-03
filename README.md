README, x2 Mark I - Two Axes Platform, Readme file

1. Introduction

2. Mechanics

3. Electronics

Arduino Uno pin designation table
╔════╦═════╦═══════════╦═══════════╦══════════════════════════════════════════╗
║ id ║ pin ║   type    ║ direction ║               description                ║
╠════╬═════╬═══════════╬═══════════╬══════════════════════════════════════════╣
║  0 ║   0 ║ digital   ║ input     ║ Serial communication                     ║
║  1 ║   1 ║ digital   ║ output    ║ Serial communication                     ║
║  2 ║   0 ║ interrupt ║ input     ║ Fan speed sensor                         ║
║  3 ║   3 ║ digital   ║ output    ║ Stepper 1 (activation/speed control)     ║
║  4 ║   4 ║ digital   ║ output    ║ Stepper drive                            ║
║  5 ║   5 ║ digital   ║ output    ║ Stepper 2 (activation/speed control)     ║
║  6 ║   6 ║ digital   ║ output    ║ Stepper 2 (activation/speed control)     ║
║  7 ║   7 ║ digital   ║ output    ║ Stepper drive                            ║
║  8 ║   8 ║ digital   ║ output    ║ Stepper drive                            ║
║  9 ║   9 ║ PWM       ║ output    ║ X2_INTERFACE_D1 (PWM)                    ║
║ 10 ║  10 ║ PWM       ║ output    ║ Fan speed control                        ║
║ 11 ║  11 ║ digital   ║ output    ║ Stepper 1 (activation/speed control)     ║
║ 12 ║  12 ║ digital   ║ output    ║ Stepper drive                            ║
║ 13 ║  13 ║ digital   ║ output    ║ OK LED (status LED)                      ║
║ 14 ║   0 ║ analog    ║ input     ║ Temperature sensor (step motor driver)   ║
║ 15 ║   1 ║ A/D       ║ I/O       ║ X2_INTERFACE_D0 and X2_INTERFACE_A0      ║
║ 16 ║   2 ║ digital   ║ output    ║ Critical LED                             ║
║ 17 ║   3 ║ digital   ║ input     ║ Warning LED                              ║
║ 18 ║   4 ║ digital   ║ output    ║ Power relay                              ║
║ 19 ║   5 ║ digital   ║ input     ║ Power supply DC detection                ║
╚════╩═════╩═══════════╩═══════════╩══════════════════════════════════════════╝

x2 interface pin designation table for Deep viewer
╔════╦═════════════════╦══════════════════════════════════════════════════════╗
║ id ║      pin        ║                  Deep viewer pins                    ║
╠════╬═════════════════╬══════════════════════════════════════════════════════╣
║  1 ║ 1               ║ +5 Vcc                                               ║
║  2 ║ X2_INTERFACE_D1 ║ Ultrasonic sensor echo pin                           ║
║  3 ║ X2_INTERFACE_D0 ║ Ultrasonic sensor trigger pin / IDMS pin             ║
║  4 ║ 4               ║ GND                                                  ║
╚════╩═════════════════╩══════════════════════════════════════════════════════╝

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
                                Source: http://busyducks.com/ascii-art-arduinos

4. Software

4.1. Odd Scanner

Use oddscanner program to generate a G-code file:

  oddscanner 100 80 > example.gcode
  
Where:

  oddscanner: program file
  100: width
  80: height
  example.gcode: output G-code file
  
After file generation, use xC to run G-code file:

  xc cli --id x2 -p example.gcode -o example.csv
  
A CSV file will be generated. So this file can be converted to a picture.









































/*
 * 
 * Description
 * 
 *   Source: https://www.marginallyclever.com/2014/03/how-to-improve-the-2-axis-cnc-gcode-interpreter-to-understand-arcs/
 * 
 * If you’ve read the previous posts about building a CNC machine that understands GCODE, you’ll know that we only went as far as to draw straight lines. In this article we’re going to look at how to draw arcs.
 *
 * To keep the ideas here bite-sized the arcs I will draw are sections of circles at a constant Z height. That means no oval-based arcs and no shapes like bunny hops or ocean waves. After we cover the basic form I touch on some ideas for drawing more complex shapes.
 *
 * Syntax
 *
 * The robot needs to know where to start, where to end, where is the center of the circle, and which way around the circle to travel. Here is some sample GCODE.
 * 
 * [code]G02 X5 Y0 I1 J0[/code]
 * 
 * The starting point is the last position of the tool in the machine, which the machine should already know. The end position is (5,0). You could also think of it as (X,Y) just like a G00 or G01 line command. The center of the circle is (1,0), or (I,J). The G02 means “go clockwise”. The other option is G03 for counter clockwise.
 * 
 * Approximation
 * 
 * Now the we’ve established the syntax for telling the robot what to do. But how does the machine actually walk the steps around the curve? The simplest way is to break each arc into lots of short straight lines. If they’re short enough no one will be able to tell the difference. I’m going to define a value called CM_PER_SEGMENT that holds the length of these short lines. The length of the arc divided by CM_PER_SEGMENT will give me the number of straight lines.
 * 
 * Note: I could say “Just keep making short lines until you hit the end position” in a while() loop. That introduces two problems. One, I can’t estimate in advance how long it will take to complete and Two, what if your short line misses the end position by going just a little too far? It could turn into a never-ending loop. …or is that a never ending arc?
 * 
 * Arc length with atan3()
 * 
 * We’ll need a way to calculate the length of the arcs. If know the angle of the start point and the end point then we can use equations for the length of an arc. So how to we get those start and end angles?
 * 
 * Most computer languages today have a robust math library that includes atan(x,y).
 * 
 * // returns angle of dy/dx as a value from 0...2PI
 */
float atan3(float dy, float dx) {
  float a = atan2(dy, dx);
  if (a<0) {
    a = (PI * 2.0) + a;
  }
  return a;
}

 /* If dx = start x – center x and dy = start y – center y then we’ll have the angle for the start position. We can repeat this for the end position and then all that matters is which way around the circle we’re going.
 * 
 *   float radius = sqrt((sy-cy)*(sy-cy) + (sx-cx)*(sx-cx));
 *   float angle1 = atan3(sy-cy,sx-cx);
 *   float angle2 = atan3(y-cy,x-cx);
 *   float sweep = angle2-angle1;
 * 
 *   if(dir==ARC_CW && sweep<0) angle2+=2*PI;   else if(dir==ARC_CCW && sweep>0) angle1+=2*PI;
 * 
 *   sweep=angle2-angle1;
 * 
 *   // get length of arc
 *   // float circumference=PI*2.0*radius;
 *   // float len=sweep*circumference/(PI*2.0);
 *   // simplifies to
 *   float len = abs(sweep) * radius;
 * Interpolation
 * 
 * We have our start angle, our ending angle, and thanks to the length of the arc and CM_PER_SEGMENT we know how many segments we’re going to need. We’ll need a way to walk along the arc and find each of those points at the ends of the segments. We know
 * 
 * [code]num_segements = len / CM_PER_SEGMENT[/code]
 * 
 * so we can make a loop,
 * 
 * [code]for(i=0;i<num_segments;++i) {[/code]
 * 
 * and each time through the loop we know we’re at
 * 
 * [code]fraction = ( i / num_segements )[/code]
 * 
 * Using a simple form of interpolation we can get the new position.
 * 
 * [code]new position = ( end position – start position ) * fraction + start position[/code]
 * 
 * Interpolation is a great tool to keep in your back pocket. You can use it to walk between any two values, which makes it awesome for animations. There are many more sophisticated kinds of interpolation, but they are outside the scope of this article.
 * 
 * Final thoughts
 * 
 * In theory arcs could be made to curve in any direction. G17 says “use the XY plane” – arc in the default style across the table top. G18 says “use the ZX plane” – arc like a windshield wiper or “wax on, wax off”. G19 says “use the ZY plane” – arc like casting a fishing rod or chopping with an axe. As far as I know there is no arbitrary plane command and there probably never will be. Modern software can convert all the arcs into line segments before sending the GCODE so the robot never really needs to know G02,G03,G17,G18, or G19.
 * 
 * This code was taken almost line-for-line from the open source code for the Makelangelo and the GcodeCNCdemo. If you build something cool with any of this code, please let me know. I love to reblog that kind of stuff!
 */
// This method assumes the limits have already been checked.
// This method assumes the start and end radius match (both points are same distance from center)
// This method assumes arcs are not >180 degrees (PI radians)
// This method assumes all movement is at a constant Z height
// cx/cy - center of circle
// x/y - end position
// dir - ARC_CW or ARC_CCW to control direction of arc
// (posx,posy,posz) is the starting position
// line() is our existing command to draw a straight line using Bresenham's algorithm.
void arc(float cx, float cy, float x, float y, float dir) {
  float posx = 0;
  float posy = 0;
  float posz = 0;

  // get radius
  float dx = posx - cx;
  float dy = posy - cy;
  float radius = sqrt(dx * dx + dy * dy);

  // find the sweep of the arc
  float angle1 = atan3(dy, dx);
  float angle2 = atan3(y - cy, x - cx);
  float sweep = angle2 - angle1;

  if (dir>0 && sweep<0) {
    angle2 += 2 * PI;
  }
  else {
    if (dir0) {
      angle1+=2*PI;
    }
  }

  sweep = angle2 - angle1;

  // get length of arc
  // float circumference=PI*2.0*radius;
  // float len=sweep*circumference/(PI*2.0);
  // simplifies to
  float len = abs(sweep) * radius;

  int i, num_segments = floor(len / CM_PER_SEGMENT);

  // declare variables outside of loops because compilers can be really dumb and inefficient some times.
  float nx, ny, nz, angle3, fraction;

  for(i=0; i<num_segments; ++i) {
    // interpolate around the arc
    fraction = ((float)i) / ((float)num_segments);
    angle3 = (sweep * fraction) + angle1;

    // find the intermediate position
    nx = cx + cos(angle3) * radius;
    ny = cy + sin(angle3) * radius;
    // make a line to that intermediate position
    line(nx, ny, posz);
  }

  // one last line hit the end
  line(x, y, posz);
}