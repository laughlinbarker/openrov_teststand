void getMetaData() {
  //F() macro stores string in PRGMEM, saving SRAM, for which we only have 1024 Bytes
  DateTime now;
  now = RTC.now();

  Serial.print(F("ESC Model: "));
  esc = readSerial();
  Serial.println(esc);

  Serial.print(F("Prop Model: "));
  prop = readSerial();
  Serial.println(prop);

  Serial.print(F("Motor Model: "));
  motor = readSerial();
  Serial.println(motor);

  Serial.print(F("Date: "));
  Serial.print(now.year(),DEC);
  Serial.print("/");
  Serial.print(now.month(),DEC);
  Serial.print("/");
  Serial.println(now.day(),DEC);

  Serial.print(F("Time: "));
  Serial.print(now.hour(),DEC);
  Serial.print(":");
  Serial.print(now.minute(),DEC);
  Serial.print(":");
  Serial.println(now.second(),DEC);

  Serial.print(F("ESC Timing: "));
  timing = readSerial();
  Serial.println(timing);

motorDirection:
  Serial.print(F("Motor Direction (F/R): "));
  motorDirection = readSerial();
  Serial.println(motorDirection);
  if (motorDirection == "R") {
    Serial.println("REVERSE!");
  }
  else if (motorDirection == "F") {
    Serial.println("FORWARD!");
  }
  else {
    goto motorDirection;
  }

  Serial.print(F("Length of arm above piviot [cm]: "));
  L1 = StrToFloat(readSerial());
  Serial.println(L1);

  Serial.print(F("Length of arm below piviot [cm]: "));
  L2 = StrToFloat(readSerial());
  Serial.println(L2);
  
  Serial.print(F("Gear ratio (1) :"));
  ratio = StrToFloat(readSerial());
  Serial.println(ratio);

  Serial.print(F("Number of consecuitive runs: "));
  String runs = readSerial();
  nruns = runs.toInt();
  Serial.println(nruns);

}


String readSerial() {
  input = "";
  while(1) {
    if (Serial.available() > 0) {
      char inByte = (char)Serial.read();
      input += inByte;
      if (inByte == '\n') {
        input.setCharAt(input.length()-1, '\0');
        return input;
      }
      // check for EOL (only need to catch \n, don't bother with carrige return)

    }
  }
}


//StrToFloat written by job_revi on Adafruit forums:
//http://forums.adafruit.com/viewtopic.php?f=8&t=22083
float StrToFloat(String str){
  char carray[str.length() + 1]; //determine size of the array
  str.toCharArray(carray, sizeof(carray)); //put str into an array
  return atof(carray);
}

void printMetaData() {
  Serial.print(F("ESC Model: "));
  Serial.println(esc);

  Serial.print(F("Prop Model: "));
  Serial.println(prop);

  Serial.print(F("Motor Model: "));
  Serial.println(motor);

  Serial.print(F("ESC Timing: "));
  Serial.println(timing);

  Serial.print(F("Motor Direction (F/R): "));
  Serial.println(motorDirection);

  Serial.print(F("Length of arm above piviot [cm]: "));
  Serial.println(L1);

  Serial.print(F("Length of arm below piviot [cm]: "));
  Serial.println(L2);
  
  Serial.print(F("Gear Ratio: "));
  Serial.println(ratio);
}

void takeMeas() {
  current = current + analogRead(currentPin);
  voltage = voltage + analogRead(voltagePin);
  thrust = thrust + analogRead(thrustPin);
  mcount = mcount + 1;

  if (mcount == avgPoints) {
    current = current/avgPoints; //average
    current = ((current - 102 )* 0.004887) / 0.133; // 0.133A/v - 102 is manually calculated offset of 0.5v
    voltage = voltage/avgPoints; //average
    voltage = voltage * 0.004887 * 3.3376;
    thrust = thrust/avgPoints; //average
    thrust = ((thrust - thrustOffset) * 0.004887) / 0.8813; // 0.8813 kg/v 
    thrust = thrust * L1 / L2 ;        //L1 is length above piviot, L2 is length below piviot
    Serial.print(F("PWM Speed: "));  
    Serial.println(motorSpeed);
    Serial.print(F("RPM: "));
    Serial.println(RPM);
    Serial.print(F("Current [A]: "));
    Serial.println(current);
    Serial.print(F("Thrust [kg]: "));
    Serial.println(thrust);
    Serial.print(F("Voltage [v]: "));
    Serial.println(voltage);
    Serial.println(" ");
    writeMeasToSD();

    mcount = 0;
    current = 0;
    thrust = 0;
    voltage = 0;

    speedBump();

  }

}

//increase/decrease speed by speedStep (macro defined)
//will change to speedBump(int direction) where direction is +/- 1 to indicate direction
void speedBump() {
  if (motorSpeed > 1500) {
    motorSpeed = motorSpeed + speedStep;
  }
  else if (motorSpeed < 1500) {
    motorSpeed = motorSpeed - speedStep;
  }

  if (motorSpeed >= 2200) {
    testWait();
  }

  else if (motorSpeed <= 800) {
    testWait();
  }


  //add some pause to allow motor to respond to speed command
  waitUntil = millis();
  while ((unsigned long)(millis() - waitUntil) <= motorPause) {
    esc1.write(motorSpeed);
  }
}

void writeMeasToSD() {  
  DateTime now;
  now = RTC.now();
  //  Serial.print(now.hour());
  //  Serial.print(":");
  //  Serial.print(now.minute());
  //  Serial.print(":");
  //  Serial.print(now.second());
  //  Serial.print(",");
  //  Serial.print(motorSpeed);
  //  Serial.print(",");
  //  Serial.print(RPM);
  //  Serial.print(",");
  //  Serial.print(thrust);
  //  Serial.print(",");
  //  Serial.print(voltage);
  //  Serial.print(",");
  //  Serial.println(current);

  logfile.print(now.hour());
  logfile.print(":");
  logfile.print(now.minute());
  logfile.print(":");
  logfile.print(now.second());
  logfile.print(",");
  logfile.print(motorSpeed);
  logfile.print(",");
  logfile.print(RPM);
  logfile.print(",");
  logfile.print(thrust);
  logfile.print(",");
  logfile.print(voltage);
  logfile.print(",");
  logfile.println(current);

  logfile.flush();
}

void testWait() {
  if (currentRun != nruns) {
    logfile.println(" ");
    logfile.println("time,servo_ms,rpm,thrust_kg,voltage_v,current_A");
    logfile.flush();
    
    currentRun++;
    motorSpeed = 1500;
    esc1.write(motorSpeed);
    delay(runPause);
    if (motorDirection == "R"){
      motorSpeed = 1450;
    }
    else if (motorDirection == "F") {
      motorSpeed = 1550;
    }
  }
  else {
    motorSpeed = 1500;
    esc1.write(motorSpeed);
    runTest = false;
  }
}



