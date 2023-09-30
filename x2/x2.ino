/* x2.ino, x2 Mark I - Two Axes Platform, Arduino main sketch file
 *
 * This sketch was developed and tested on: Arduino Uno
 * To work on other Arduino models, some adaptations may be necessary.
 *
 * Author: Márcio Pessoa <marcio.pessoa@gmail.com>
 * Contributors: none
 */

#include <AFMotor.h>      // Adafruit - Motor Shield
#include <Alarm.h>        // Marcio Pessoa - Manage alarms
#include <Arduino.h>      // Arduino - Main library
#include <Axis.h>         // Marcio Pessoa - Motor axis
#include <Blinker.h>      // Marcio Pessoa - Blink leds nicely
#include <Fan.h>          // Marcio Pessoa - Fan speed control
#include <IDMS.h>         // Marcio Pessoa - Infrared distance measuring
#include <Project.h>      // Marcio Pessoa - Basic project definitions
#include <Switch.h>       // Marcio Pessoa - Switch manipulation
#include <Temperature.h>  // Marcio Pessoa - Temperature Sensors
#include <Timer.h>        // Marcio Pessoa - Timer library with nice features
#include <Ultrasonic.h>   // Marcio Pessoa - Ultrasonic distance measuring

#include "config.h"  // Marcio Pessoa - Configuration
// #include <SharpIR.h>      // Unoficial - Infrared distance measuring sensor
#include <MemoryFree.h>  //

// Project definitions
Project x2("x2",                                        // Platform
           "I",                                         // Mark
           "Two Axes Platform",                         // Name
           "0.18b",                                     // Version
           "2017-08-19",                                // Version date
           "1",                                         // Serial number
           "Copyright (c) 2012-2017 Marcio Pessoa",     // Owner
           "undefined. There is NO WARRANTY.",          // License
           "https://github.com/marcio-pessoa/x2",       // Website
           "Marcio Pessoa <marcio.pessoa@gmail.com>");  // Contact

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
Alarm fan(85,   // Warning
          95);  // Critical

// Teperature sensor
Temperature lm35;
Alarm temperature(60,  // Maximum warning
                  70,  // Maximum critical
                  10,  // Minimum warning
                  5);  // Minimum critical

// Check timer
Timer health_check(health_check_timer * 1000);

// Sensors timer
Timer sensors_status(sensors_timer * 1000);

// Demonstration timer
Timer demonstration_period(0, COUNTDOWN);

// Laser
Switch laser(laser_pin);

// Ultrasonic distance measuring sensor
Ultrasonic HC_SR04;

// Infrared distance measuring sensor
IDMS Sharp_GP2Y0A21YK0F;
// SharpIR Sharp_GP2Y0A21YK0F(idms_pin, 1080);  // TODO: Uninstall this lib

// Axis
Axis x_axis("x",   // Name
            2,     // Electronic identifier
            -800,  // Minimum hard position
            800,   // Maximum hard position
            -800,  // Minimum soft position
            800,   // Maximum soft position
            0,     // Park position
            2);    // Move delay
AF_Stepper x_stepper(200, x_axis.pinRead());
Axis y_axis("y",   // Name
            1,     // Electronic identifier
            -500,  // Minimum hard position
            400,   // Maximum hard position
            -500,  // Minimum soft position
            400,   // Maximum soft position
            0,     // Park position
            2);    // Move delay
AF_Stepper y_stepper(200, y_axis.pinRead());

// Power save options
Timer standby((unsigned long)standby_timer * 60 * 1000, COUNTDOWN);
bool standby_status = false;
bool standby_done = false;

void setup() {
  // Serial interface
  Serial.begin(serial_speed);
  // Start up message
  // Serial.println("Starting...");
  CommandM92();  // System information
  // Power relay
  power.nameWrite("Power relay");
  // Power supply DC detection
  pinMode(power_sensor_pin, INPUT);
  // Standby is disabled by default
  // standby.disable();
  // Laser
  laser.nameWrite("Laser");
  // Temperature
  lm35.attach(lm35_pin);
  temperature.nameWrite("Temperature");
  temperature.unitWrite(" *C");
  // Fan
  fan_control.writeSpeed(100);
  // pinMode(fan_sensor_pin, INPUT_PULLUP);
  attachInterrupt(fan_sensor_pin, spinCounter, RISING);
  fan.force_check(OK);
  fan.nameWrite("Fan");
  fan.unitWrite("%");
  // Ultrasonic distance measuring sensor
  HC_SR04.attach(ultrasonic_trigger_pin, ultrasonic_echo_pin);
  // Infrared distance measuring sensor
  Sharp_GP2Y0A21YK0F.attach(idms_pin);
  // Random number generator seed
  pinMode(random_Seed_pin, INPUT);
  randomSeed(analogRead(random_Seed_pin));
  // G-code ready to receive commands
  GcodeReady();
}

void loop() {
  SensorsHandler();
  HealthCheckHandler();
  NotificationHandler();
  AxesHandler();
  DemonstrationHandler();
  PowerHandler();
  GcodeCheck();
}
