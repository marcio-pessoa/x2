/* commands.ino, x2 Mark I - Two Axes Platform, Arduino commands sketch file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@sciemon.com>
 * Contributors: none
 */

/* command_M100
 * 
 * Description
 *   Shows help messages.
 * 
 *   command_M100()
 * 
 * Parameters
 *   letter: The command initial letter. It's used to display a specific help
 *           about a letter.
 *           If letter equals to zero, all help messages will be shown.
 * 
 * Returns
 *   void
 */
void command_M100(char letter = 0) {
  if (letter == 'G' or letter == 0) {
    echoln(F("G01\tAbolute moves"));  // Test: Pending
    echoln(F("G02\tRelative moves"));  // Test: Pending
    echoln(F("G03\tDelay between moves"));  // Test: Pending
    echoln(F("G06\tDemonstratio mode"));  // Test: Pending
    echoln(F("G28\tHome axes"));  // Test: OK
    echoln(F("G132\tCalibrate axes"));  // Test: Pending
  }
  if (letter == 'M' or letter == 0) {
    echoln(F("M17\tEnable/Power all stepper motors"));  // Test: Pending
    echoln(F("M18\tDisable all stepper motors; same as M84"));  // Test: Pending
    echoln(F("M15\tSystem info"));  // Test: Pending
    echoln(F("M80\tPower on"));  // Test: OK
    echoln(F("M81\tPower off"));  // Test: OK
    echoln(F("M82\tPower status"));  // Test: OK
    echoln(F("M84\tLaser on"));  // Test: Pending
    echoln(F("M84\tLaser off"));  // Test: Pending
    echoln(F("M86\tAxes information"));  // Test: Pending
    echoln(F("M87\tIs all done?"));  // Test: Pending
    echoln(F("M88\tDistance measure"));  // Test: Pending
    echoln(F("M89\tMemory information"));  // Test: Pending
    echoln(F("M90\tFan information"));  // Test: Pending
    echoln(F("M91\tTemperature information"));  // Test: Pending
    echoln(F("M92\tVersion information"));  // Test: Pending
    echoln(F("M99\tReset system"));  // Test: Pending
    echoln(F("M100\tThis help message"));  // Test: Pending
    echoln(F("M111\tDebug mode"));  // Test: Pending
    echoln(F("M124\tStop all axes"));  // Test: Pending
  }
}

bool command_isdone() {
  echoln("MOVE: " + String(isAllDone() ? "ok" : "pending"));
  return false;
}

bool command_power_status() {
  echoln("POWER: " + String(power.status() ? "on" : "off"));
  return false;
}

bool command_power_on() {
  power.set(HIGH);
  return !power.status();
}

bool command_power_off() {
  power.set(LOW);
  return power.status();
}

void command_reset() {
  echoln("Reseting...\n");
  x_stepper.release();
  y_stepper.release();
  laser.set(LOW);
  power.set(LOW);
  x2.reset();
}

bool command_laser_on() {
  if (digitalRead(power_sensor_pin)) {
    laser.on();
  }
  status(!power.status());
}

bool command_laser_off() {
  laser.off();
  status(power.status());
}

/* command_goto_absolute
 * 
 * Description
 *   Moves axes to absolute position.
 * 
 *   command_goto_absolute(100, -80)
 * 
 * Parameters
 *   x: Cartesian position to x axis.
 *   y: Cartesian position to y axis.
 * 
 * Returns
 *   bool: 0 - No error.
 *         1 - Position limit has exceeded.
 */
bool command_goto_absolute(int x, int y) {
  x_axis.positionWrite(x);
  y_axis.positionWrite(y);
  return false;
}

/* command_goto_relative
 * 
 * Description
 *   Moves axes to relative position.
 * 
 *   command_goto_relative(100, -80)
 * 
 * Parameters
 *   x: Cartesian position to x axis.
 *   y: Cartesian position to y axis.
 * 
 * Returns
 *   bool: 0 - No error.
 *         1 - Position limit has exceeded.
 */
bool command_goto_relative(int x, int y) {
  x_axis.positionWrite(x_axis.positionRead() + x);
  y_axis.positionWrite(y_axis.positionRead() + y);
  return false;
}

bool command_delay(int x, int y) {
  x_axis.delayWrite(x);
  y_axis.delayWrite(y);
  return false;
}

bool command_park() {
  x_axis.delayWrite(2);
  y_axis.delayWrite(2);
  x_axis.positionWrite(x_axis.parkRead());
  y_axis.positionWrite(y_axis.parkRead());
  x_stepper.release();
  y_stepper.release();
  return false;
}

bool command_stop() {
  demonstration_period.set(0);
  x_axis.positionWrite(x_axis.positionRead());
  y_axis.positionWrite(y_axis.positionRead());
  return false;
}

bool command_demo(int seconds) {
  if (seconds > 0) {
    demonstration_period.set(seconds * 1000);
  }
}

bool command_gage() {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  echo("Calibrating");
  laser.off();
  int axis_delay = 2;
  Serial.print(".");
  // Go to limit
  int i = 0;
  for (i=0; i<=1800; i++) {
    if (i <= 1800) {
      x_stepper.step(1, BACKWARD, INTERLEAVE);
    }
    if (i <= 1500) {
      y_stepper.step(1, FORWARD, INTERLEAVE);
    }
    delay(axis_delay);
  }
  Serial.print(".");
  // Come back to center
  for (i=0; i<=840; i++) {
    if (i <= 840) {
      x_stepper.step(1, FORWARD, INTERLEAVE);
    }
    if (i <= 640) {
      y_stepper.step(1, BACKWARD, INTERLEAVE);
    }
    delay(axis_delay);
  }
  Serial.print(".");
  x_stepper.release();
  x_axis.positionReset();
  y_stepper.release();
  y_axis.positionReset();
  Serial.println(F(" Done."));
  return false;
}

bool command_axes() {
  echoln(String("Axes position: ") +
              "(" + String(x_axis.positionRead()) + ", " +
                    String(y_axis.positionRead()) + ")");
  echoln(String("Axes delay: ") +
              "(" + String(x_axis.delayRead()) + ", " +
                    String(y_axis.delayRead()) + ")");
}

bool command_ultrasonic() {
  laser.off();
  echoln(distance + ": " + HC_SR04.read() + " cm");
  return false;
}

bool command_temperature() {
  echoln(temperature.nameRead() + " (" +
       temperature.status_name() + "): " +
       temperature.valueRead() +
       temperature.unitRead());
  if (debug)
    echoln("  Warning low: " +
         String(temperature.min_warningRead()) +
         temperature.unitRead() + "\n" +
         "  Critical low: " +
         String(temperature.min_criticalRead()) +
         temperature.unitRead() + "\n" +
         "  Warning high: " +
         String(temperature.max_warningRead()) +
         temperature.unitRead() + "\n" +
         "  Critical high: " +
         String(temperature.max_criticalRead()) +
         temperature.unitRead());
}

bool command_fan() {
  echoln(fan.nameRead() + " (" +
       fan.status_name() + "): " +
       (int)fan.valueRead() +
       fan.unitRead() + " (" +
       fan_control.readRPM() +
       " RPM)");
  if (debug) {
    echoln("  Warning: " + String(fan.max_warningRead()) + fan.unitRead() +
         "\n" +
         "  Critical: " + String(fan.max_criticalRead()) + fan.unitRead());
  }
}

bool command_mem() {
  int total = 2 * 1024;
  int free = freeMemory();
  int used = total - free;
  int percent = (float)used * 100 / total;
  // 
  Alarm memory(75, 85);
  memory.nameWrite("Memory");
  memory.unitWrite("%");
  memory.check(percent);
  // 
  echoln(memory.nameRead() + " (" + memory.status_name() + "): " + 
       percent + memory.unitRead() + " used");
  if (debug) {
    echoln("  SRAM:\t" + String(total) + " B\n" +
         "  Used:\t" + used + " B\n" +
         "  Free:\t" + free + " B\n" +
         "  Warning: " + memory.max_warningRead() + memory.unitRead() + "\n" +
         "  Critical: " + memory.max_criticalRead() + memory.unitRead());
  }
}

void command_info() {
  command_version();
  command_mem();
  command_power_status();
  command_temperature();
  command_fan();
  command_axes();
}

bool command_version() {
  echoln(x2.version());
  if (debug or (millis() < 100)) {
    echoln(x2.owner());
    echoln(x2.compiled());
    echoln(x2.license());
    echoln(x2.website());
    echoln(x2.contact());
  }
  return false;
}

/* command_M111
 * 
 * Description
 *   Changes debug mode on or off.
 * 
 *   command_M111()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
void command_M111() {
  debug = !debug;
  echoln("DEBUG: " + String(debug ? F("on") : F("off")));
}

void command_unknown() {
  echoln(F("Error: unknown command")); 
}

bool command_attach() {
  return false;
}

/* command_detach
 * 
 * Description
 *   Detaches stepper motors.
 * 
 *   command_detach()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
void command_detach() {
  x_stepper.release();
  y_stepper.release();
}
