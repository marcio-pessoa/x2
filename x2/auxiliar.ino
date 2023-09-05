/* auxiliar.ino, x2 Mark I - Two Axes Platform, Arduino auxiliar sketch file
 *
 * Author: Márcio Pessoa <marcio.pessoa@gmail.com>
 * Contributors: none
 */

/* isAllDone
 *
 * Description
 *   Checks if all axes is done (not moving).
 *
 *   isAllDone()
 *
 * Parameters
 *   none
 *
 * Returns
 *   bool: 0 - No axis moving.
 *         1 - There are on or more axes moving.
 */
bool isAllDone() {
  if (x_axis.isDone() and
      y_axis.isDone()) {
    return true;
  } else {
    return false;
  }
}

/* spinCounter
 *
 * Description
 *   Fan spin counter.
 *
 *   spinCounter()
 *
 * Parameters
 *   none
 *
 * Returns
 *   void
 */
void spinCounter() {
  fan_control.counter();
}
