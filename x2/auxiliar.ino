/* auxiliar.ino, x2 Mark I - Two Axes Platform, Arduino auxiliar sketch file
 * 
 * Author: Márcio Pessoa <marcio@pessoa.eti.br>
 * Contributors: none
 */

bool isAllDone() {
  if (x_axis.isDone() and
      y_axis.isDone()) {
    return true;
  }
  else {
    return false;
  }
}

void spinCounter() {
  fan_control.counter();
}

void x_stepperCalibrate(bool debug = true) {
  int x = 0;
  x_axis.delayWrite(2);
  if (debug) Serial.print(".");
  // Go to limit
  for (x=0; x<=1800; x++) {
    x_stepper.step(1, BACKWARD, INTERLEAVE);
    delay(x_axis.delayRead());
  }
  if (debug) Serial.print(".");
  // Come back to center
  for (x=0; x<=840; x++) {
    x_stepper.step(1, FORWARD, INTERLEAVE);
    delay(x_axis.delayRead());
  }
  if (debug) Serial.print(".");
  x_stepper.release();
  x_axis.positionReset();
}

void y_stepperCalibrate(bool debug = true) {
  int y = 0;
  y_axis.delayWrite(2);
  if (debug) Serial.print(".");
  // Go to limit
  for (y=0; y<=1500; y++) {
    y_stepper.step(1, FORWARD, INTERLEAVE);
    delay(y_axis.delayRead());
  }
  if (debug) Serial.print(".");
  // Come back to center
  for (y=0; y<=640; y++) {
    y_stepper.step(1, BACKWARD, INTERLEAVE);
    delay(y_axis.delayRead());
  }
  if (debug) Serial.print(".");
  y_stepper.release();
  y_axis.positionReset();
}

void xy_stepperCalibrate(bool debug = true) {
  int axis_delay = 2;
  if (debug) Serial.print(".");
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
  if (debug) Serial.print(".");
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
  if (debug) Serial.print(".");
  x_stepper.release();
  x_axis.positionReset();
  y_stepper.release();
  y_axis.positionReset();
}
