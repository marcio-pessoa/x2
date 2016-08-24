/* commands.ino, x2 Mark I - Two Axes Platform, Arduino commands sketch file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@sciemon.com>
 * Contributors: none
 */

/* CommandM100
 * 
 * Description
 *   Shows help messages.
 * 
 *   CommandM100()
 * 
 * Parameters
 *   letter: The command initial letter. It's used to display a specific help
 *           about a letter.
 *           If letter equals to zero, all help messages will be shown.
 * 
 * Returns
 *   void
 */
void CommandM100(char letter = 0) {
  if (letter == 'G' or letter == 0) {
    echoln(F("G01\Move axes"));
    echoln(F("G03\tDelay between moves"));
    echoln(F("G06\tDemonstratio mode"));
    echoln(F("G28\tHome axes"));
    echoln(F("G90\tAbsolute programming"));
    echoln(F("G91\tIncremental programming"));
    echoln(F("G132\tCalibrate axes"));
  }
  if (letter == 'M' or letter == 0) {
    echoln(F("M15\tSystem info"));  // Test: Pending
    echoln(F("M17\tAttach motors"));  // Test: Pending
    echoln(F("M18\tDetach motors; same as M84"));  // Test: Pending
    echoln(F("M70\tLaser status"));  // Test: Pending
    echoln(F("M71\tLaser on"));
    echoln(F("M72\tLaser off"));
    echoln(F("M80\tPower status"));
    echoln(F("M81\tPower on"));
    echoln(F("M82\tPower off"));
    echoln(F("M86\tAxes information"));  // Test: Pending
    echoln(F("M87\tIs all done?"));  // Test: Pending
    echoln(F("M88\tDistance measure"));  // Test: Pending
    echoln(F("M89\tMemory information"));  // Test: Pending
    echoln(F("M90\tFan information"));  // Test: Pending
    echoln(F("M91\tTemperature information"));  // Test: Pending
    echoln(F("M92\tSystem information"));  // Test: Pending
    echoln(F("M99\tReset system"));  // Test: Pending
    echoln(F("M100\tThis help message"));
    echoln(F("M111\tDebug mode"));  // Test: Pending
    echoln(F("M124\tStop all axes"));
  }
}

/* 
 * 
 * Description
 *   .
 * 
 *   ()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
bool CommandM87() {
  echoln("Move: " + String(isAllDone() ? "ok" : "pending"));
  return false;
}

bool CommandM80() {
  echo("Power status");
  if (debug) {
    echoln(String("\n") +
           "  " + 
           power.nameRead() + ": " + (power.status() ? "On" : "Off") + "\n" +
           "  DC current: " + (digitalRead(power_sensor_pin) ? "Yes" : "No"));
  }
  else {
    Serial.println(String(": ") + (digitalRead(power_sensor_pin) ? "On" : "Off"));
  }
  return !digitalRead(power_sensor_pin);
}

bool CommandM81() {
  power.set(HIGH);
  return !power.status();
}

bool CommandM82() {
  power.set(LOW);
  return power.status();
}

void CommandG90() {
  x_axis.absolute(true);
  y_axis.absolute(true);
}

void CommandG91() {
  x_axis.absolute(false);
  y_axis.absolute(false);
}

void CommandM99() {
  echoln("Reseting...\n");
  x_stepper.release();
  y_stepper.release();
  laser.set(LOW);
  power.set(LOW);
  x2.reset();
}

bool CommandM71() {
  if (digitalRead(power_sensor_pin)) {
    laser.on();
  }
  return !power.status() and !digitalRead(laser_pin);
}

bool CommandM72() {
  laser.off();
  return power.status() and digitalRead(laser_pin);
}

void CommandM70() {
  echoln("LASER: " + String(laser.status() ? "on" : "off"));
}

/* CommandG1
 * 
 * Description
 *   Moves axes to position.
 * 
 *   CommandG1(100, -80)
 * 
 * Parameters
 *   x: x axis position.
 *   y: y axis position.
 * 
 * Returns
 *   bool: 0 - No error.
 *         1 - Position limit has exceeded.
 */
bool CommandG1(int x, int y) {
  x_axis.positionWrite(x);
  y_axis.positionWrite(y);
  return false;
}

bool CommandG3(int x, int y) {
  x_axis.delayWrite(x);
  y_axis.delayWrite(y);
  return false;
}

bool CommandG28() {
  x_axis.delayWrite(2);
  y_axis.delayWrite(2);
  x_axis.positionWrite(x_axis.parkRead());
  y_axis.positionWrite(y_axis.parkRead());
  x_stepper.release();
  y_stepper.release();
  return false;
}

bool CommandM124() {
  demonstration_period.set(0);
  x_axis.positionWrite(x_axis.positionRead());
  y_axis.positionWrite(y_axis.positionRead());
  return false;
}

bool CommandG6(int seconds) {
  if (seconds > 0) {
    demonstration_period.set(seconds * 1000);
    return false;
  }
  return true;
}

bool CommandG132() {
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

bool CommandM86() {
  echoln(String("Axes position: ") +
                "(" + String(x_axis.positionRead()) + ", " +
                      String(y_axis.positionRead()) + ")");
  echoln(String("Axes delay: ") +
                "(" + String(x_axis.delayRead()) + ", " +
                      String(y_axis.delayRead()) + ")");
}

bool CommandM88() {
  laser.off();
  echoln(distance + ": " + HC_SR04.read() + " cm");
  return false;
}

bool CommandM91() {
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

bool CommandM90() {
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

bool CommandM89() {
  int total = 2 * 1024;
  int free = freeMemory();
  int used = total - free;
  int percent = (float)used * 100 / total;
  // 
  Alarm memory(75, 85);
  memory.nameWrite("MEMORY");
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

void CommandM15() {
  CommandM92();
  CommandM89();
  CommandM80();
  CommandM91();
  CommandM90();
  CommandM86();
}

void CommandM92() {
  echoln(x2.version());
  if (debug or (millis() < 100)) {
    echoln(x2.owner());
    echoln(x2.compiled());
    echoln(x2.license());
    echoln(x2.website());
    echoln(x2.contact());
  }
}

/* CommandM111
 * 
 * Description
 *   Changes debug mode on or off.
 * 
 *   CommandM111()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
void CommandM111() {
  debug = !debug;
  echoln("DEBUG: " + String(debug ? F("on") : F("off")));
}

void Command0() {
  echoln(F("Unknown command")); 
}

bool CommandM17() {
  return false;
}

/* CommandM18
 * 
 * Description
 *   Detaches stepper motors.
 * 
 *   CommandM18()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
void CommandM18() {
  x_stepper.release();
  y_stepper.release();
}
