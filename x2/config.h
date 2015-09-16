/* config.h - x2 - 2 axis platform - Arduino project config file
 * 
 * Author: Márcio Pessoa <marcio@pessoa.eti.br>
 * Contributors: none
 */

// x2 dedicated hardware user interface
const byte X2_INTERFACE_A0 = 1;
const byte X2_INTERFACE_D0 = 15;
const byte X2_INTERFACE_D1 = 9;

// LED
const byte led_ok_pin = 13;
const byte led_warning_pin = 17;
const byte led_critical_pin = 16;

// Power
const byte power_relay_pin = 18;
const byte power_sensor_pin = 19;

// Fan
const byte fan_control_pin = 10;
const byte fan_sensor_pin = 0;

// Temperature
const byte lm35_pin = 0;

// Ultrasonic distance sensor
const byte ultrasonic_trigger_pin = X2_INTERFACE_D0;
const byte ultrasonic_echo_pin = X2_INTERFACE_D1;

// Laser
const byte laser_pin = X2_INTERFACE_D0;

// Random number generator Seed pin
const byte random_Seed_pin = lm35_pin;

// Timers
const byte health_check_timer = 2;  // seconds
const byte sensors_timer = 2;  // seconds
const unsigned long standby_timer = 5;  // minutes

// Serial speed: 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200
const unsigned long serial_speed = 57600;
