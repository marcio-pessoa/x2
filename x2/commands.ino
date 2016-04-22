/* commands.ino, x2 Mark I - Two Axes Platform, Arduino commands sketch file
 * 
 * Author: Márcio Pessoa <marcio@pessoa.eti.br>
 * Contributors: none
 */

void commands() {
  /*
   * Power Management
   */
  // Power Management
  help_main.sectionAdd("Check user manual.");
  //~ help_main.sectionAdd("Power management");
  // power
  CLI.addCommand("power", command_power);
  //~ help_main.commandAdd("power", "power {on,off} system");
  // reset
  CLI.addCommand("reset", command_reset);
  //~ help_main.commandAdd("reset", F("reset system"));
  // laser
  CLI.addCommand("laser", command_laser);
  //~ help_main.commandAdd("laser", F("laser LED"));
  /*
   * Positioning
   */
  //~ help_main.sectionAdd("Positioning");
  // a
  CLI.addCommand("a", command_goto_absolute);
  //~ help_main.commandAdd("g", "go to absolute position");
  // r
  CLI.addCommand("r", command_goto_relative);
  //~ help_main.commandAdd("r", "go to relative position");
  // d
  CLI.addCommand("d", command_delay);
  //~ help_main.commandAdd("d", "set axis delay");
  // stop
  CLI.addCommand("stop", command_stop);
  //~ help_main.commandAdd("stop", "stop moving");
  // park
  CLI.addCommand("park", command_park);
  //~ help_main.commandAdd("park", "go to park position");
  // demo
  CLI.addCommand("demo", command_demo);
  //~ help_main.commandAdd("demo", "demonstration mode");
  // gage
  CLI.addCommand("gage", command_gage);
  //~ help_main.commandAdd("gage", "calibrate axes");
  /*
   * Information
   */
  //~ help_main.sectionAdd(F("Information"));
  // info
  CLI.addCommand("info", command_info);
  //~ help_main.commandAdd("info", F("system information"));
  // axes
  CLI.addCommand("axes", command_axes);
  //~ help_main.commandAdd("axes", F("axes position"));
  // isdone
  CLI.addCommand("isdone", command_isdone);
  //~ help_main.commandAdd("isdone", "all are moves done?");
  // read
  CLI.addCommand("read", command_ultrasonic);
  //~ help_main.commandAdd("read", F("measure distance"));
  // mem
  CLI.addCommand("mem", command_mem);
  //~ help_main.commandAdd("mem", F("memory information"));
  // fan
  CLI.addCommand("fan", command_fan);
  //~ help_main.commandAdd("fan", F("fan speed"));
  // temp
  CLI.addCommand("temp", command_temperature);
  //~ help_main.commandAdd("temp", F("temperature"));
  // version
  CLI.addCommand("ver", command_version);
  //~ help_main.commandAdd("ver", F("system version"));
  // debug
  CLI.addCommand("debug", command_debug);
  //~ help_main.commandAdd("debug", F("debug mode"));
  /*
   * Help
   */
  //~ help_main.sectionAdd(F(""));
  // help
  CLI.addCommand("help", command_help);
  //~ help_main.commandAdd("help", F("this help message"));
  /*
   * Default
   */
  CLI.addDefaultHandler(command_unknown);
}

void command_power() {
  bool ok = false;
  bool change = false;
  char *arg = CLI.next();
  // Turn On
  if (strcmp(arg, "on") == false) {
    ok = true;
    change = power.set(temperature.status() == CRITICAL ? LOW : HIGH);
  }
  // Turn Off
  else if (strcmp(arg, "off") == false) {
    ok = true;
    laser.set(LOW);
    change = power.set(LOW);
  }
  // Show status when required or when invalid argument is inputed
  else {
    Serial.print("Power status");
    if (debug) {
      Serial.println(String("\n") +
                     "  " + power.nameRead() + ": " + (power.status() ? "On" : "Off") + "\n" +
                     "  DC current: " + (digitalRead(power_sensor_pin) ? "Yes" : "No"));
    }
    else {
      Serial.println(String(": ") + (digitalRead(power_sensor_pin) ? "On" : "Off"));
    }
  }
  // 
  if (ok) {
    Serial.println(power.nameRead() + " " + (change ? "turned" : "stays") + ": " + (power.status() ? "On" : "Off"));
  }
}

void command_reset() {
  Serial.println("Reseting...\n");
  x_stepper.release();
  y_stepper.release();
  laser.set(LOW);
  power.set(LOW);
  x2.reset();
}

void command_laser() {
  char *arg = CLI.next();
  // Turn On
  if (strcmp(arg, "on") == false) {
    laser.on();
  }
  else if (strcmp(arg, "off") == false) {
    laser.off();
  }
  else {
    Serial.print("Laser: ");
    if (laser.status()) {
      Serial.println("On");
    }
    else {
      Serial.println("Off");
    }
  }
}

void command_goto_absolute() {
  // int x = 0;
  // int y = 0;
  char *arg = CLI.next();
  if (arg != NULL) {
    // x = atoi(arg);
    x_axis.positionWrite(atoi(arg));
  }
  arg = CLI.next();
  if (arg != NULL) {
    // y = atoi(arg);
    y_axis.positionWrite(atoi(arg));
  } 
}

void command_goto_relative() {
  // int x = 0;
  // int y = 0;
  char *arg = CLI.next();
  if (arg != NULL) {
    // x = atoi(arg);
    x_axis.positionWrite(x_axis.positionRead() + atoi(arg));
  }
  arg = CLI.next();
  if (arg != NULL) {
    // y = atoi(arg);
    y_axis.positionWrite(y_axis.positionRead() + atoi(arg));
  } 
}

void command_delay() {
  // int x = 0;
  // int y = 0;
  char *arg = CLI.next();
  if (arg != NULL) {
    // x = atoi(arg);
    x_axis.delayWrite(atoi(arg));
  }
  arg = CLI.next();
  if (arg != NULL) {
    // y = atoi(arg);
    y_axis.delayWrite(atoi(arg));
  } 
}

void command_park() {
  x_axis.delayWrite(2);
  y_axis.delayWrite(2);
  x_axis.positionWrite(x_axis.parkRead());
  y_axis.positionWrite(y_axis.parkRead());
  x_stepper.release();
  y_stepper.release();
}

void command_stop() {
  demonstration_period.set(0);
  x_axis.positionWrite(x_axis.positionRead());
  y_axis.positionWrite(y_axis.positionRead());
}

void command_demo() {
  char *arg = CLI.next();
  unsigned int seconds = atoi(arg);
  if (seconds > 0) {
    demonstration_period.set(seconds * 1000);
  }
}

void command_gage() {
  if (digitalRead(power_sensor_pin)) {
    Serial.print(F("Calibrating"));
    laser.off();
    xy_stepperCalibrate(true);
    Serial.println(F(" Done."));
  }
}

void command_axes() {
  Serial.println(String("Axes position: ") +
                 "(" + String(x_axis.positionRead()) + ", " +
                       String(y_axis.positionRead()) + ")");
  Serial.println(String("Axes delay: ") +
                 "(" + String(x_axis.delayRead()) + ", " +
                       String(y_axis.delayRead()) + ")");
  //~ if (debug) {
    //~ Serial.println(x_axis.status() + "\n" +
                   //~ y_axis.status());
  //~ }
}

void command_ultrasonic() {
  laser.off();
  Serial.println(distance + ": " + 
                 HC_SR04.read() + " cm");
}

void command_temperature() {
  Serial.println(temperature.nameRead() + " (" +
                 temperature.status_name() + "): " +
                 temperature.valueRead() +
                 temperature.unitRead());
  if (debug)
    Serial.println("  Warning low: " +
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

void command_fan() {
  Serial.println(fan.nameRead() + " (" +
                 fan.status_name() + "): " +
                 (int)fan.valueRead() +
                 fan.unitRead() + " (" +
                 fan_control.readRPM() +
                 " RPM)");
  if (debug) {
    Serial.println("  Warning: " + String(fan.max_warningRead()) + fan.unitRead() + "\n" +
                   "  Critical: " + String(fan.max_criticalRead()) + fan.unitRead());
  }
}

void command_mem() {
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
  Serial.println(memory.nameRead() + " (" + memory.status_name() + "): " + 
                 percent + memory.unitRead() + " used");
  if (debug) {
    Serial.println("  SRAM:\t" + String(total) + " B\n" +
                   "  Used:\t" + used + " B\n" +
                   "  Free:\t" + free + " B\n" +
                   "  Warning: " + memory.max_warningRead() + memory.unitRead() + "\n" +
                   "  Critical: " + memory.max_criticalRead() + memory.unitRead());
  }
}

void command_info() {
  command_version();
  command_mem();
  command_power();
  command_temperature();
  command_fan();
  command_axes();
}

void command_isdone() {
  if (isAllDone()) {
    Serial.println("Move (OK): Done");
  }
  else {
    Serial.println("Move (Pending): On going");
  }
}

void command_version() {
  Serial.println(x2.version());
  if (debug or (millis() < 100)) {
    Serial.println(x2.owner());
    Serial.println(x2.license());
    Serial.println(x2.website());
    Serial.println(x2.contact());
  }
}

void command_debug() {
  debug = !debug;
  Serial.println("Debug mode: " + String(debug ? "Yes" : "No"));
}

void command_help() {
  Serial.println(help_main.show());
}

void command_unknown() {
  Serial.println(F("Error: unknown command")); 
}

void command_detach() {
  x_stepper.release();
  y_stepper.release();
}
