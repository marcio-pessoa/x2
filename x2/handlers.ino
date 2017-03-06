/* handlers.ino, x2 Mark I - Two Axes Platform, Arduino handlers sketch file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@sciemon.com>
 * Contributors: none
 */

bool AxesHandler() {
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
      x_stepper.step(1, BACKWARD, INTERLEAVE);
      break;
    }
    case -1: {
      standby.reset();
      x_stepper.step(1, FORWARD, INTERLEAVE);
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
      y_stepper.step(1, BACKWARD, INTERLEAVE);
      break;
    }
    case -1: {
      standby.reset();
      y_stepper.step(1, FORWARD, INTERLEAVE);
      break;
    }
    case 0:
    default: {
      break;
    }
  }
  // 
  if (isAllDone() and done == false) {
    done = true;
    status(false);
    x_axis.delayWrite(2);
    y_axis.delayWrite(2);
  }
}

void DemonstrationHandler() {
  if(!demonstration_period.check()) {
    if (x_axis.isDone()) {
      x_axis.positionWrite(random(x_axis.hardMinimumRead(), x_axis.hardMaxiumRead()));
    }
    if (y_axis.isDone()) {
      y_axis.positionWrite(random(y_axis.hardMinimumRead(), y_axis.hardMaxiumRead()));
    }
  }
}

void HealthCheckHandler() {
  if (health_check.check()) {
    // Set fan speed
    fan_control.writeSpeed(map(constrain(temperature.valueRead(), 20, 50),  // temperature
                               20, 50,    // From (minimum and maximum)
                               0, 100));  // To (minimum and maximum)
    // Power off on high temperature
    if (temperature.status() == CRITICAL) {
      if (debug) CommandM91();
      CommandM18();  // Detach motors
      CommandM72();  // Laser off
      CommandM82();  // Power off
    }
    // Report fan on high speed
    if (fan.status() == CRITICAL) {
      if (debug) CommandM90();
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
        break;
      }
    }
  }
}

void NotificationHandler() {
  ok_led.blink();
  warning_led.blink();
  critical_led.blink();
}

void PowerHandler() {
  if (standby.check()) {
    if (!standby_status) {
      standby_status = true;
      // Serial.print("Entering in standby... ");
      CommandG90();  // Absolute programming
      CommandM72();  // Laser off
      CommandG28();  // Home axes
    }
    if (isAllDone() and !standby_done) {
      standby_done = true;
      standby_status = false;
      CommandM82();  // Power off
      // Serial.println("Done.");
    }
  }
  // If power fail
  if (digitalRead(power_sensor_pin) == LOW) {
    CommandM18();  // Disable all motors
    CommandM72();  // Laser off
  }
}

void SensorsHandler() {
  if (sensors_status.check()) {
    // Check temperature
    temperature.check(lm35.read());
    // Check fan
    fan.check(fan_control.readSpeed());
  }
}
