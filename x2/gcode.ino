/* gcode.ino, x2 Mark I - Two Axes Platform, G-code parser sketch file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@sciemon.com>
 * Contributors: none
 */

#define BUFFER_SIZE 48

char buffer[BUFFER_SIZE];
int buffer_pointer;

void status(bool i) {
  Serial.println(i == false ? F("ok") : F("nok"));
}

bool echo(String message) {
  Serial.print(String(F("echo:")) + message);
}

bool echoln(String message) {
  echo(message + "\n");
}

void GcodeReady() {
  buffer_pointer = 0;
}

void GcodeCheck() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (buffer_pointer < BUFFER_SIZE-1) {
      buffer[buffer_pointer++] = c;
    }
    if (c == '\n') {
      buffer[buffer_pointer] = 0;
      GCodeParse();
      GcodeReady();
    }
  }
}

float GCodeNumber(char code, float val) {
  char *ptr = buffer;
  while(ptr && *ptr && ptr < buffer + buffer_pointer) {
    if(*ptr == code) {
      return atof(ptr + 1);
    }
    ptr = strchr(ptr, ' ') + 1;
  }
  return val;
}

void GCodeParse() {
  bool retval = false;
  char letter = buffer[0];
  byte number = GCodeNumber(letter, -1);
  if (letter == 'G') {
    switch (number) {
      case 0:
        command_M100(letter);
        break;
      case 1:
        retval = command_goto_absolute(GCodeNumber('X', 0), GCodeNumber('Y', 0));
        break;
      case 2:
        retval = command_goto_relative(GCodeNumber('X', 0), GCodeNumber('Y', 0));
        break;
      case 3:
        retval = command_delay(GCodeNumber('X', 0), GCodeNumber('Y', 0));
        break;
      case 28:
        retval = command_park();
        break;
      case 132:
        retval = command_gage();
        break;
      case 6:
        retval = command_demo(GCodeNumber('T', 0));
        break;
      default:
        break;
    }
  }
  else if (letter == 'M') {
    switch(number) {
      case 0:
        command_M100(letter);
        break;
      case 124:
        retval = command_stop();
        break;
      case 17:
        retval = command_attach();
        break;
      case 18:
        command_detach();
        break;
      case 80:
        retval = command_power_on();
        break;
      case 81:
        retval = command_power_off();
        break;
      case 82:
        retval = command_power_status();
        break;
      case 99:
        command_reset();
        break;
      case 84:
        retval = command_laser_on();
        break;
      case 85:
        retval = command_laser_off();
        break;
      case 15:
        command_info();
        break;
      case 86:
        retval = command_axes();
        break;
      case 87:
        retval = command_isdone();
        break;
      case 88:
        retval = command_ultrasonic();
        break;
      case 89:
        retval = command_mem();
        break;
      case 90:
        retval = command_fan();
        break;
      case 91:
        retval = command_temperature();
        break;
      case 92:
        retval = command_version();
        break;
      case 100:
        command_M100();
        break;
      case 111:
        command_M111();
        break;
      default:
        break;
    }
  }
  if (buffer_pointer > 0) {
    status(retval);
  }
}
