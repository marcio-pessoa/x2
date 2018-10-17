/* config.h, x2 Mark I - Two Axes Platform, Arduino project config file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@gmail.com>
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

// Ultrasonic distance measuring sensor
const byte ultrasonic_trigger_pin = X2_INTERFACE_D0;
const byte ultrasonic_echo_pin = X2_INTERFACE_D1;

// Infrared distance measuring sensor
const byte idms_pin = X2_INTERFACE_A0;

// Laser
const byte laser_pin = X2_INTERFACE_D0;

// Random number generator Seed pin
const byte random_Seed_pin = lm35_pin;

// Timers
const byte health_check_timer = 2;  // seconds
const byte sensors_timer = 2;  // seconds
const byte standby_timer = 1;  // minute

// System status (initial state)
byte general_status = UNKNOWN;

// Debug mode
bool debug_mode = false;

// Serial speed: 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200
const unsigned long serial_speed = 115200;

// Move done
bool done = true;

// 
const float FLIMIT = 340282350000000000000000000000000000000;
