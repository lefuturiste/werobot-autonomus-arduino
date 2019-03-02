// LATEST VERSION
// by werobot

#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"
#include "MeAuriga.h"
#include "LegoRC.h"

MeGyro Gyro(0, 0x69);
MeRGBLed Ring(0, 12);

// left = M1
MeEncoderOnBoard LeftEncoder(SLOT1);
// right = M2
MeEncoderOnBoard RightEncoder(SLOT2);

int pinIrLed = A11; //pin A11 utilisé en sortie digitale (port 10 Auriga)
LegoRC lego(pinIrLed, 50);

// to read: LightSensor.read();
MeLightSensor LightSensor(12);

int leftSpeed = 0;
int rightSpeed = 0;
int leftAskedSpeed = 0;
int rightAskedSpeed = 0;

String command = "";
String commandName = "";
int commandParam1 = 0;
int commandParam2 = 0;
int commandParam3 = 0;
int commandParam4 = 0;

void stopAll() {
  leftAskedSpeed = 0;
  rightAskedSpeed = 0;
  LeftEncoder.setMotorPwm(0);
  RightEncoder.setMotorPwm(0);
}


void apply() {
//  Serial.print("left:");
//  Serial.print(leftAskedSpeed);
//  Serial.print(" right: ");
//  Serial.println(rightAskedSpeed);
  RightEncoder.setMotorPwm(rightAskedSpeed); 
  LeftEncoder.setMotorPwm(leftAskedSpeed); 
}

void left(int param) {
  leftAskedSpeed = param;
}

void right(int param) {
  rightAskedSpeed = param;
}

void color(int red, int green, int blue, int leds = 0) {
  Ring.setColor(leds, red, green, blue);
  Ring.show();
}

bool isCover() {
  return (LightSensor.read() < 81);
}

void triggerElectron() {
  lego.sendCommand(1, 4, 9); // 1 --> ChanneL switch 2 ; 4 --> Single output mode - rouge ; 9 --> PWM backward step 7 (avance chariot)
}

void setup() {
  Serial.begin(9600); 
  Gyro.begin();
  Ring.setpin(44);
  delay(2);
  color(0, 0, 0);
//  delay(500);
}

void loop() {
  handleSerial();
  delay(1);
}

void handleSerial() {
  
    command = Serial.readStringUntil('\n');
    if (command != ""){
//      Serial.println("--");
//      Serial.println(command);

      // Parse command name
      commandName = command.substring(0, command.indexOf("#"));

      // Parse command arguments (1, 2, 3)
      String raw1 = command.substring(command.indexOf("#") + 1, command.length());      
      if (raw1 != command) {       
        commandParam1 = raw1.substring(0, raw1.indexOf("#")).toInt();
        
        String raw2 = raw1.substring(raw1.indexOf("#") + 1, raw1.length());
//        Serial.println(raw2);

        if (raw2 != raw1) {
          if (raw2.indexOf("#") != -1) {
            commandParam2 = raw2.substring(0, raw2.indexOf("#")).toInt();

            String raw3 = raw2.substring(raw2.indexOf("#") + 1, raw2.length());
            if (raw3 != raw2) {
              if (raw3.indexOf("#") != -1) {
                commandParam3 = raw3.substring(0, raw3.indexOf("#")).toInt();
                commandParam4 = raw3.substring(raw3.indexOf("#") + 1, raw3.length()).toInt();              
              } else {
                commandParam3 = raw3.substring(0, raw3.length()).toInt();
              }
            }
          } else {
            commandParam2 = raw2.substring(0, raw2.length()).toInt();
          }
        }
      }
      
//       Serial.println(commandName);
//       Serial.println(commandParam1);
//       Serial.println(commandParam2);
//       Serial.println(commandParam3);
//       Serial.println(commandParam4);

      if (commandName == "STOPALL") {
        stopAll();
      } else if (commandName == "LEFT") {
          left(commandParam1);        
      } else if (commandName == "RIGHT") {
          right(commandParam1);
      } else if (commandName == "TWICE") {
          left(commandParam1);
          right(commandParam2);
          apply();
      } else if (commandName == "APPLY") {
          apply();
      } else if (commandName == "COLOR") {
          color(commandParam1, commandParam2, commandParam3, commandParam4);
      } else if (commandName == "LIGHT") {
          Serial.print("L: ");
          Serial.println(LightSensor.read());
      } else if (commandName == "ISCOVER") {
          Serial.print("L: ");
          Serial.println(isCover() ? "true" : "false");
      } else if (commandName == "RGYRO") {
          Gyro.begin();
          Gyro.fast_update();
      } else if (commandName == "ANGLEZ") {
          Gyro.fast_update();
          Serial.print("G: ");
          Serial.println(Gyro.getAngleZ());
      } else if (commandName == "ELEC") {
          lego.sendCommand(1, 4, 9);
          Serial.println("L: OK");
      } else if (commandName == "PING") {
          Serial.println("L: pong!");
      } else {
          Serial.println("E: invalid commandName");
      }
      
//      Serial.println("--");
      delay(2);
      commandParam1 = 0;
      commandParam2 = 0;
      commandParam3 = 0;
      commandParam4 = 0;
    }
}

