#include <FreqMeasure.h>        //for measuring RPM
#include <ServoTimer2.h>        //Uses #2 timer for servo PWM instead of #1 (whic is used by FreqMeasure)
#include <SPI.h>
#include <SD.h>                 //for wiriting measurements to SD card
#include <Wire.h>               //for 1-wire buss comms with RTC
#include "RTClib.h"             //for RTC

#define RPMPin 8 // RPM counting
#define escPin1 3 // ESC control
#define thrustPin A0 // load cell
#define voltagePin A1    //ESC voltage
#define currentPin A2    //ESC current
#define Imax 15          //max current, triggers safety shutdown NOT YET IMPLIMENTED
#define speedStep 25     //ms interval to adjust servo speed
#define motorPause 1000  //time for motor to adjust speed
#define avgPoints 4      //number of non-RPM measurements to average  
#define RPMPts  40       //number of RPM readings to make and then average
#define runPause 60000      //seconds to wait between consecuitive runs if asked

RTC_DS1307 RTC; // define the RTC object

//initalize vars here to keep master list
String input, esc, prop, motor, date, timing, motorDirection;
float L1, L2, ratio = 1;
double sum = 0, RPM, current = 0, thrust = 0, voltage = 0;
int count = 0, mcount = 0, motorSpeed, thrustOffset, currentOffset, nruns, currentRun;
boolean metaFinished = false, runTest = false;
const int chipSelect = 10;
unsigned long waitUntil = motorPause;
char dateStr[10];
char timeStr[10];
char measStr[36];

ServoTimer2 esc1;    //servo output
File logfile;        //log file

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);

  while(1);
}

//setup for serial, RTC, 1-wire, FreqMeasure
void setup() {
  Serial.begin(115200);    //start serial

  //SD card initialization
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");

  //create new file on SD card
  char filename[] = "THRUST00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      break; // leave the loop!
    }
  }

  //throw error if problem with logfileM
  if (! logfile) {
    error("couldnt create file");
  }

  Serial.print("Logging to: ");
  Serial.println(filename);

  input = "";              //clear serial input var
  metaFinished = false;    // set metadata flag to false

    FreqMeasure.begin();     //start FreqMeasure

  esc1.attach(escPin1);    //ESC connect and neutral
  esc1.write(1500);

  Wire.begin();            //1-wire for RTC
  if (!RTC.begin()) {
    Serial.println("RTC failed");
  }
  currentRun = 1;
}

void loop() {
  DateTime now; //RTC object
  now = RTC.now();

  //gather test metadata
  while (!metaFinished){
getMetaData:
    getMetaData();

    //re-print metadata for confirm
    Serial.println(F("Current test setup:"));
    Serial.println(F(" "));
    Serial.println(F("###################"));
    printMetaData();
    Serial.println(F("###################"));
    Serial.println(F(" "));

    //confirm metadata, if wrong, do it again
    Serial.println(F("Is the above data correct (Y/N)?"));
    if (readSerial().charAt(0) != 'Y') {
      Serial.println(F("Sorry, please try again!"));
      goto getMetaData;
    }
    else {
      Serial.print(F("Writing test parameters to SD card..."));
      logfile.print("ESC: ");
      logfile.println(esc);
      logfile.print("Prop: ");
      logfile.println(prop);
      logfile.print("Motor: ");
      logfile.println(motor);
      logfile.print("DateTime: ");
      logfile.println(date);
      logfile.print("Timing: ");
      logfile.println(timing);
      logfile.print("Direction: ");
      logfile.println(motorDirection);
      logfile.print("L1: ");
      logfile.println(L1);
      logfile.print("L2: ");
      logfile.println(L2);
      logfile.print("GearRatio: ");
      logfile.print(ratio);

      logfile.flush();
      Serial.println(F("FINISHED"));
      metaFinished = true;
    }
  }


  //enter "TEST" to start test

  //thrust loop
  Serial.println(F("Type 'START' to run the test"));
  if (readSerial() == "START") {
    thrustOffset = analogRead(thrustPin);
    //currentOffset = analogRead(currentPin);

    logfile.println(" ");
    logfile.println("time,servo_ms,rpm,thrust_kg,voltage_v,current_A");
    logfile.flush();
    runTest = true;
    if (motorDirection == "R"){
      motorSpeed = 1450;
    }
    else if (motorDirection == "F") {
      motorSpeed = 1550;
    }
  }
if (runTest == false) {
  motorSpeed = 1500;
  esc1.write(motorSpeed);
}
  
  while (runTest) {
      esc1.write(motorSpeed);
      //code snippit from https://www.pjrc.com/teensy/td_libs_FreqMeasure.html
      if (FreqMeasure.available()) {
        // average several reading together
        sum = sum + FreqMeasure.read();
        count = count + 1;
        if (count > RPMPts) {
          FreqMeasure.end();
          RPM = (F_CPU / (sum / count)) * 60;
          takeMeas();
          sum = 0;
          count = 0;
          FreqMeasure.begin();
        }
      }
    }
    Serial.println(" ");
    Serial.println(F("TEST FINISHED - DATA SUCESSFULLY RECORDED"));
    Serial.println(F("Type 'START' torepeat test with same parameters"));
    Serial.println(F("OR"));
    Serial.println(F("Reset Arduino to perform test with new parameters"));
    Serial.println(" ");

    //finish file write

    //reset Arduino

}










