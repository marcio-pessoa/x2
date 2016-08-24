/* gcode.ino, x2 Mark I - Two Axes Platform, G-code parser sketch file
 * 
 * Author: Márcio Pessoa <marcio.pessoa@sciemon.com>
 * Contributors: none
 */

#define BUFFER_SIZE 48

char buffer[BUFFER_SIZE];
int buffer_pointer = 0;

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
  switch (letter) {
    case 'G':
      switch (number) {
        case 0:
          CommandM100(letter);
          break;
        case 1:
        case 2:
          retval = CommandG1(GCodeNumber('X', 0), GCodeNumber('Y', 0));
          break;
        case 3:
          retval = CommandG3(GCodeNumber('X', 0), GCodeNumber('Y', 0));
          break;
        case 6:
          retval = CommandG6(GCodeNumber('T', 0));
          break;
        case 28:
          retval = CommandG28();
          break;
        case 90:
          CommandG90();
          break;
        case 91:
          CommandG91();
          break;
        case 132:
          retval = CommandG132();
          break;
        default:
          Command0();
          break;
      }
      break;
    case 'M':
      switch(number) {
        case 0:
          CommandM100(letter);
          break;
        case 124:
          retval = CommandM124();
          break;
        case 17:
          retval = CommandM17();
          break;
        case 18:
        case 84:
          CommandM18();
          break;
        case 70:
          CommandM70();
          break;
        case 71:
          retval = CommandM71();
          break;
        case 72:
          retval = CommandM72();
          break;
        case 80:
          CommandM80();
          break;
        case 81:
          retval = CommandM81();
          break;
        case 82:
          retval = CommandM82();
          break;
        case 15:
          CommandM15();
          break;
        case 86:
          retval = CommandM86();
          break;
        case 87:
          retval = CommandM87();
          break;
        case 88:
          retval = CommandM88();
          break;
        case 89:
          retval = CommandM89();
          break;
        case 90:
          retval = CommandM90();
          break;
        case 91:
          retval = CommandM91();
          break;
        case 92:
          CommandM92();
          break;
        case 99:
          CommandM99();
          break;
        case 100:
          CommandM100();
          break;
        case 111:
          CommandM111();
          break;
        default:
          Command0();
          break;
      }
      break;
    default:
      if (buffer_pointer > 2) {
        Command0();
      }
      break;
  }
  if (buffer_pointer > 2) {
    status(retval);
  }
}
