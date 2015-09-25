/* x2.ino - x2 - 2 axis platform - Arduino main sketch file
 * 
 * This sketch was developed and tested on: Arduino Uno
 * To work on other Arduino models, some adaptations may be necessary.
 * 
 * Author: Márcio Pessoa <marcio@pessoa.eti.br>
 * Contributors: none
 */

#include <Arduino.h>         // Arduino - Main library
#include <Project.h>         // Sciemon - Basic project definitions
#include <Help.h>            // Sciemon - Help messages builder
#include <Blinker.h>         // Sciemon - Blink leds nicely
#include <Timer.h>           // Sciemon - Timer library with nice features
#include <Alarm.h>           // Sciemon - Manage alarms
#include <Switch.h>          // Sciemon - Switch manipulation
#include <Fan.h>             // Sciemon - Fan speed control
#include <Temperature.h>     // Sciemon - Temperature Sensors
#include <Axis.h>            // Sciemon - Motor axis
#include <Ultrasonic.h>      // Sciemon - Ultrasonic distance sensor
#include <Infrared.h>        // Sciemon - Infrared distance sensor
#include "config.h"          // Sciemon - Configuration
#include <AFMotor.h>         // Adafruit - Motor Shield
#include "SCommand.h"        // SerialCommand - Command line interface
#include <MemoryFree.h>      // 

// Project definitions
Project x2("x2",  // Platform
           "I",  // Mark
           "0.10b",  // Version
           "2015-09-24",  // Version date
           "1",  // Serial number
           "Copyright (c) 2012-2015 Marcio Pessoa",  // Owner
           "undefined. There is NO WARRANTY.",  // License
           "http://pessoa.eti.br/",  // Website
           "Marcio Pessoa <marcio@pessoa.eti.br>");  // Contact

// OK LED (Status LED)
Blinker ok_led(led_ok_pin);

// Warning LED
Blinker warning_led(led_warning_pin);

// Critical LED
Blinker critical_led(led_critical_pin);

// Power relay
Switch power(power_relay_pin, LOW);

// Fan
Fan fan_control(fan_control_pin, fan_sensor_pin);
Alarm fan(85, 95);

// Teperature sensor
Temperature lm35;
Alarm temperature(60, 70, 10, 5);

// Check timer
Timer health_check(health_check_timer * 1000);

// Sensors timer
Timer sensors_status(sensors_timer * 1000);

// Laser
Switch laser(laser_pin);

// Ultrasonic distance sensor
Ultrasonic HC_SR04;
String distance = "Distance";

// Axis
Axis x_axis("x",   // Name
            2,     // Electronic identifier
            -800,  // Minimum hard position
            800,   // Maximum hard position
            -800,  // Minimum soft position
            800,   // Maximum soft position
            0,     // Park position
            2);    // Move delay
Axis y_axis("y",   // Name
            1,     // Electronic identifier
            -500,  // Minimum hard position
            400,   // Maximum hard position
            -500,  // Minimum soft position
            400,   // Maximum soft position
            0,     // Park position
            2);    // Move delay

// Step motors
AF_Stepper x_stepper(200, x_axis.pinRead());
AF_Stepper y_stepper(200, y_axis.pinRead());

// Power save options
Timer standby((unsigned long)standby_timer * 60 * 1000, COUNTDOWN);
bool standby_status = false;
bool standby_done = false;

// CLI - Command Line Interface
SerialCommand CLI;
Help help_main;

void setup() {
  // Serial interface
  Serial.begin(serial_speed);
  // Start up message
  Serial.println(x2.version());
  Serial.println(x2.owner());
  Serial.println(x2.license());
  Serial.println(x2.website());
  Serial.println(x2.contact());
  // Power relay
  power.nameWrite("Power relay");
  // Power supply DC detection
  pinMode(power_sensor_pin, INPUT);
  // Laser
  laser.nameWrite("Laser");
  // Temperature
  lm35.attach(lm35_pin);
  temperature.nameWrite("Temperature");
  // Fan
  fan_control.writeSpeed(100);
  attachInterrupt(fan_sensor_pin, counter, RISING);
  fan.force_check(OK);
  fan.nameWrite("Fan");
  fan.unitWrite("%");
  // Ultrasonic distance measuring sensor
  HC_SR04.attach(ultrasonic_trigger_pin, ultrasonic_echo_pin);
  // Random number generator seed
  pinMode(random_Seed_pin, INPUT);
  randomSeed(analogRead(random_Seed_pin));
  // Setup callbacks for commands
  commands();
}

void loop() {
  SensorsHandler();
  HealthCheckHandler();
  NotificationHandler();
  AxesHandler();
  DemonstrationHandler();
  PowerHandler();
  CLI.readSerial();
}
