/* handlers.ino - x2 - 2 axis platform - Arduino handlers sketch file
 * 
 * Author: Márcio Pessoa <marcio@pessoa.eti.br>
 * Contributors: none
 */

void notification_handler() {
  ok_led.blink();
  warning_led.blink();
  critical_led.blink();
}

bool axes_handler() {
  // Don't move axes if system is in CRITICAL state
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  // Check axes values
  x_axis.run();
  y_axis.run();
  // Move x axis
  switch (x_axis.positionReadRelative()) {
    case 1: {
      standby.reset();
      x_stepper.step(1, FORWARD, INTERLEAVE);
      break;
    }
    case -1: {
      standby.reset();
      x_stepper.step(1, BACKWARD, INTERLEAVE);
      break;
    }
    case 0:
    default: {
      break;
    }
  }
  // Move y axis
  switch (y_axis.positionReadRelative()) {
    case 1: {
      standby.reset();
      y_stepper.step(1, FORWARD, INTERLEAVE);
      break;
    }
    case -1: {
      standby.reset();
      y_stepper.step(1, BACKWARD, INTERLEAVE);
      break;
    }
    case 0:
    default: {
      break;
    }
  }
}

void sensors_handler() {
  if (sensors_status.check()) {
    // Check temperature
    temperature.check(lm35.read());
    // Check fan
    fan.check(fan_control.readSpeed());
  }
}

void standby_handler() {
  if (standby.check()) {
    if (!standby_status) {
      standby_status = true;
      Serial.print("Entering in standby... ");
      laser.off();
      command_park();
    }
    if (isAllDone() and !standby_done) {
      standby_done = true;
      power.set(LOW);
      Serial.println("Done.");
    }
  }
}

void demonstration_handler() {
  if (demonstration) {
    if (x_axis.isDone()) {
      x_axis.positionWrite(random(x_axis.hardMinimumRead(), x_axis.hardMaxiumRead()));
      //~ x_axis.delayWrite(random(2 ,10));
    }
    if (y_axis.isDone()) {
      y_axis.positionWrite(random(y_axis.hardMinimumRead(), y_axis.hardMaxiumRead()));
      //~ y_axis.delayWrite(random(2 ,10));
    }
  }
}

void health_check_handler() {
  if (health_check.check()) {
    // Set fan speed
    fan_control.writeSpeed(map(constrain(temperature.valueRead(), 20, 40),
                           20, 40,
                           0, 100));
    // Check temperature
    if (temperature.status() == CRITICAL) {
      command_temperature();
      power.set(LOW);
    }
    // Check fan
    if (fan.status() == CRITICAL and debug) {
      command_fan();
    }
  }
  // Join alarm status
  byte general_status = UNKNOWN;
  if (temperature.status() == OK and
      fan.status() == OK) {
    general_status = OK;
  }
  else if (temperature.status() == WARNING or
           fan.status() == WARNING) {
    general_status = WARNING;
  }
  else if (temperature.status() == CRITICAL or
           fan.status() == CRITICAL) {
    general_status = CRITICAL;
  }
  else {
    general_status = UNKNOWN;
  }
  // Notify status via LEDs
  switch (general_status) {
    case OK: {
      ok_led.enable();
      warning_led.disable();
      critical_led.disable();
      break;
    }
    case WARNING: {
      ok_led.disable();
      warning_led.enable();
      critical_led.disable();
      break;
    }
    case CRITICAL: {
      ok_led.disable();
      warning_led.disable();
      critical_led.enable();
      break;
    }
    case UNKNOWN: 
    default: {
      ok_led.enable();
      warning_led.enable();
      critical_led.enable();
      ok_led.periodWrite(200);
      warning_led.periodWrite(200);
      critical_led.periodWrite(200);
      notification_handler();
      ok_led.periodWrite(1000);
      warning_led.periodWrite(500);
      critical_led.periodWrite(250);
      break;
    }
  }
  // Check condition to power off devices
  if (temperature.status() == CRITICAL or
      digitalRead(power_sensor_pin) == LOW) {
    laser.set(LOW);
    x_stepper.release();
    y_stepper.release();
  }
}
