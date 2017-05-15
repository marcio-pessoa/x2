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
    echoln(F("G01\tRapid positioning"));
    echoln(F("G01\tLinear interpolation"));
    echoln(F("G02\tCircular interpolation, clockwise"));
    echoln(F("G03\tCircular interpolation, counterclockwise"));
    // echoln(F("G06\tDemonstratio mode"));
    echoln(F("G28\tHome axes"));
    echoln(F("G90\tAbsolute programming"));
    echoln(F("G91\tIncremental programming"));
    // echoln(F("G132\tCalibrate axes"));
  }
  if (letter == 'M' or letter == 0) {
    // echoln(F("M15\tSystem info"));
    // echoln(F("M17\tAttach motors"));
    // echoln(F("M18\tDetach motors; same as M84"));
    // echoln(F("M70\tLaser status"));
    // echoln(F("M03\tLaser on"));
    // echoln(F("M05\tLaser off"));
    // echoln(F("M80\tPower on"));
    // echoln(F("M81\tPower off"));
    // echoln(F("M86\tAxes information"));
    // echoln(F("M87\tIs all done?"));
    // echoln(F("M88\tDistance measure"));
    // echoln(F("M89\tMemory information"));
    // echoln(F("M90\tFan information"));
    // echoln(F("M91\tTemperature information"));
    // echoln(F("M92\tSystem information"));
    // echoln(F("M99\tReset system"));
    // echoln(F("M100\tThis help message"));
    // echoln(F("M111\tDebug mode"));
    // echoln(F("M124\tStop all axes"));
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

bool CommandM80a() {
  if (debug_mode) {
    echo("Power status");
    echoln(String("\n") +
           "  " + 
           power.nameRead() + ": " + (power.status() ? "On" : "Off") + "\n" +
           "  DC current: " + (digitalRead(power_sensor_pin) ? "Yes" : "No"));
  }
  return !digitalRead(power_sensor_pin);
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
bool CommandM80() {
  power.set(HIGH);
  for (byte i=0; i<10; i++) {
    if (!CommandM80a()) {  // Power status
      return false;
    }
    delay(100);
  }
  return true;
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
bool CommandM81() {
  power.set(LOW);
  return power.status();
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
void CommandG90() {
  x_axis.absolute(true);
  y_axis.absolute(true);
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
void CommandG91() {
  x_axis.absolute(false);
  y_axis.absolute(false);
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
void CommandM99() {
  echoln("Reseting...\n");
  x_stepper.release();
  y_stepper.release();
  laser.set(LOW);
  power.set(LOW);
  x2.reset();
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
bool CommandM71() {
  if (digitalRead(power_sensor_pin)) {
    laser.on();
  }
  return !power.status() and !digitalRead(laser_pin);
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
bool CommandM72() {
  laser.off();
  return power.status() and digitalRead(laser_pin);
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
void CommandM70() {
  echoln("LASER: " + String(laser.status() ? "on" : "off"));
}

/* CommandG0
 * 
 * Description
 *   Rapid positioning.
 * 
 *   CommandG0(100, -80, -10)
 * 
 * Parameters
 *   x: x axis position.
 *   y: y axis position.
 *   z: z axis position.
 * 
 * Returns
 *   false: OK
 *   true: Position limit has exceeded
 */
bool CommandG0(float x, float y, float z) {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  if (x != FLIMIT) {
    done = false;
    x_axis.positionWrite(x);
  }
  if (y != FLIMIT) {
    done = false;
    y_axis.positionWrite(y);
  }
  if (z != FLIMIT) {
    done = false;
    if (z < 0) {
      CommandM71();
    }
    if (z > 0) {
      CommandM72();
    }
  }
}

/* CommandG1
 * 
 * Description
 *   Linear interpolation.
 *   Moves axes from B point to C point using hypotenuse line.
 * 
 *   CommandG1(100, -80, -0.3)
 * 
 *   Math explanation
 *     Quando viajamos entre o ponto B e C (no caso das linhas com ângulos), a
 *     velocidade do cursor b e c não podem ser iguais, do contrário teremos
 *     como resultado sempre um triângulo equilátero.
 * 
 *        B
 *         |\
 *         | \
 *         |  \
 *     CA c|   \a H
 *         |    \
 *         |     \
 *         |      \
 *        A---------C
 *             b
 *             CO
 * 
 *     Para formar um triângulo retângulo e dessa forma obter uma linha de
 *     hipotenusa perfeita, é necessário compensar a velocidade dos cursores
 *     b e c para que ambos cheguem ao destino simultaneamente.
 *     Dessa forma, o cateto adjacente (CA - nossa referência) será sempre o de
 *     maior comprimento e por essa razão o cursor c levará mais tempo para
 *     completar o trajeto.
 *     O objetivo consiste em compensar a velocidade do cursor b do cateto
 *     oposto (CO - objetivo de cáculo) para que ele chegue ao ponto de destino
 *     mais lentamente.
 *     Dessa forma, podemos concluir que quanto maior for a acurácia da medida
 *     de tempo maior será a precisão do momento de chegada dos cursores b e c,
 *     consequentemente a linha da hipotenusa (H) será desenhada com maior
 *     fidelidade.
 * 
 *   Computing
 *     Levando em consideração um triangulo onde CA=4cm, CO=3cm, e a base de
 *     tempo T=2ms, qual é o fator de tempo que deve ser aplicado ao cursor b
 *     para que chegue ao destino simultaneamente com o cursor c?
 * 
 *   Proportion (cross-multiplication)
 *     CA * T -> CA * T
 *     CO * x -> CA * T
 * 
 *   So
 *     x = (CA * T) / CO
 * 
 *   Example
 *     x = (4 * 2) / 3
 *     x = 8 / 3
 *     x = 2.66666666666666666666ms
 * 
 * Parameters
 *   x: x axis position.
 *   y: y axis position.
 *   z: z axis (tool) position.
 * 
 * Returns
 *   bool: 0 - No error.
 *         1 - Position limit has exceeded.
 */
bool CommandG1(float x, float y, float z) {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  //
  if (abs(y - y_axis.positionRead()) > abs(x - x_axis.positionRead())) {
    x_axis.delayWrite(round((float)(abs(y - y_axis.positionRead()) *
                                        y_axis.delayRead()) /
                                    abs(x - x_axis.positionRead())));
  }
  if (abs(x - x_axis.positionRead()) > abs(y - y_axis.positionRead())) {
    y_axis.delayWrite(round((float)(abs(x - x_axis.positionRead()) *
                                        x_axis.delayRead()) / 
                                    abs(y - y_axis.positionRead())));
  }
  if (debug_mode) {
    CommandM86();  // Axes information
  }
  // 
  if (x != FLIMIT) {
    done = false;
    x_axis.positionWrite(x);
  }
  if (y != FLIMIT) {
    done = false;
    y_axis.positionWrite(y);
  }
  if (z != FLIMIT) {
    done = false;
    if (z < 0) {
      CommandM71();  // Laser on
    }
    if (z >= 0) {
      CommandM72();  // Laser off
    }
  }
}

/* CommandG2
 * 
 * Description
 *   Circular interpolation, clockwise.
 * 
 *   CommandG2(x, y, i, j)
 * 
 *   The start position is the current position.
 * 
 * Parameters
 *   int x: The last x position of the tool
 *   int y: The last y position of the tool
 *   int i: The horizontal position of the center
 *   int j: The vertical position of the center
 * 
 * Returns
 *   false: OK
 *   true: Invalid input
 */
bool CommandG2(int x, int y, int i, int j) {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  return false;
}

/* CommandG3
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
bool CommandG3(int x, int y) {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  x_axis.delayWrite(x);
  y_axis.delayWrite(y);
  return false;
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
bool CommandG28() {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  done = false;
  CommandG90();  // Absolute programming
  x_axis.delayWrite(2);
  y_axis.delayWrite(2);
  x_axis.positionWrite(x_axis.parkRead());
  y_axis.positionWrite(y_axis.parkRead());
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
bool CommandM124() {
  demonstration_period.set(0);
  x_axis.positionWrite(x_axis.positionRead());
  y_axis.positionWrite(y_axis.positionRead());
  return false;
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
bool CommandG6(int seconds) {
  if (!digitalRead(power_sensor_pin)) {
    return true;
  }
  if (seconds > 0) {
    demonstration_period.set(seconds * 1000);
    return false;
  }
  return true;
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
bool CommandM86() {
  echoln(String("Axes position: ") +
                "(" + String(x_axis.positionRead()) + ", " +
                      String(y_axis.positionRead()) + ")");
  echoln(String("Axes delay: ") +
                "(" + String(x_axis.delayRead()) + ", " +
                      String(y_axis.delayRead()) + ")");
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
bool CommandM88() {
  CommandM72();  // Laser off
  echoln(String(x_axis.positionRead()) + "," +
                y_axis.positionRead() + "," +
                HC_SR04.read());
  return false;
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
bool CommandM91() {
  echoln(temperature.nameRead() + " (" +
         temperature.status_name() + "): " +
         temperature.valueRead() +
         temperature.unitRead());
  if (debug_mode)
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
bool CommandM90() {
  echoln(fan.nameRead() + " (" +
         fan.status_name() + "): " +
         (int)fan.valueRead() +
         fan.unitRead() + " (" +
         fan_control.readRPM() +
         " RPM)");
  if (debug_mode) {
    echoln("  Warning: " + String(fan.max_warningRead()) + fan.unitRead() +
           "\n" +
           "  Critical: " + String(fan.max_criticalRead()) + fan.unitRead());
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
  if (debug_mode) {
    echoln("  SRAM:\t" + String(total) + " B\n" +
           "  Used:\t" + used + " B\n" +
           "  Free:\t" + free + " B\n" +
           "  Warning: " + memory.max_warningRead() + memory.unitRead() + "\n" +
           "  Critical: " + memory.max_criticalRead() + memory.unitRead());
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
void CommandM15() {
  CommandM92();  // 
  CommandM89();  // 
  CommandM80a();  // 
  CommandM91();  // 
  CommandM90();  // 
  CommandM86();  // 
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

/* CommandM0
 * 
 * Description
 *   Stop all axes.
 * 
 *   CommandM0()
 * 
 * Parameters
 *   none
 * 
 * Returns
 *   void
 */
bool CommandM0() {
  demonstration_period.set(0);
  x_axis.positionWrite(x_axis.positionRead());
  y_axis.positionWrite(y_axis.positionRead());
  return false;
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
  debug_mode = !debug_mode;
  echoln("DEBUG: " + String(debug_mode ? F("on") : F("off")));
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
  CommandM0();  // Compulsory stop
  x_stepper.release();
  y_stepper.release();
}
