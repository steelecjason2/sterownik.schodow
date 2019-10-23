


void readFromSerial() {
  while(Serial.available()) {
    char c = (char)Serial.read();         // read char from RoboRemo app
 if(c == '\n') {
      cmd[cmdIndex] = 0;
      exeCmd();                           // execute the command
      cmdIndex = 0;                       // reset the cmdIndex
    } else {      
      cmd[cmdIndex] = c;
      if(cmdIndex < 49) cmdIndex++;
    }
    }
}

boolean cmdStartsWith(char *st) {
  for(int i = 0; ; i++) {
    if(st[i] == 0) return true;
    if(cmd[i] == 0) return false;
    if(cmd[i] != st[i]) return false;;
  }
  return false;
}

void lightSensorPrint(int button) {
  
        int input = analogRead(lightSensor);
        Serial.print("lightValue: ");
        Serial.println(input);
        if (stateLight) Serial.println("sensors off");
        else Serial.println("sensors on");
        if (button == downSensor) Serial.println("bottom sensor");
        if (button == upSensor) Serial.println("top sensor");
        if (button == downButton) Serial.println("bottom button");
        if (button == upButton) Serial.println("top button");
        
}

void exeCmd() {
    
if (cmdStartsWith("synchro0")) {
      Serial.println("--------------- main config ---------------");
      Serial.print("brightness ");
      Serial.println(brightness); 
      Serial.print("brighteningStepForAll ");
      Serial.println(brighteningStepForAll);
      Serial.print("dimmingStepForAll ");
      Serial.println(dimmingStepForAll);
      Serial.print("brighteningStep ");
      Serial.println(brighteningStep); 
      Serial.print("dimmingStep ");
      Serial.println(dimmingStep);
      Serial.print("brighteningAltStep ");
      Serial.println(brighteningAltStep); 
      Serial.print("dimmingAltStep ");
      Serial.println(dimmingAltStep); 
      Serial.print("standby ");
      Serial.println(standby); 
      Serial.print("waitDuration ");
      Serial.println(waitDuration); 
      Serial.print("alternative ");
      Serial.println(alternative); 
      Serial.print("standbyActive ");
      Serial.println(standbyActive);
      Serial.print("Mode ");
      Serial.println(mode);
      Serial.println("-------------------------------------------------");
      mode = modeOff;
      partyTest = false;
    }

if (cmdStartsWith("synchro1")) {
      Serial.println("--------- light sensor config ----------");
      Serial.print("lightThreshold ");
      Serial.println(lightThreshold);   
      Serial.print("darkThreshold ");
      Serial.println(darkThreshold); 
      Serial.print("light sensor state: ");
      if (stateLight == false) Serial.println("dark"); 
      else Serial.println("light"); 
      Serial.println("-------------------------------------------------");
    }

if (cmdStartsWith("synchro2")) {
      Serial.println("-------------- party config ---------------");
      Serial.print("brightness ");
      Serial.println(brightness);
      Serial.print("partyStep ");
      Serial.println(partyStep);
      Serial.print("partyloopRandMax ");
      Serial.println(partyloopRandMax); 
      Serial.print("partyloopBDMax ");
      Serial.println(partyloopBDMax);   
      Serial.print("partyloopScannerMax ");
      Serial.println(partyloopScannerMax);
      Serial.print("partyloopNukeMax ");
      Serial.println(partyloopNukeMax);
      Serial.print("partyloopWaveMax ");
      Serial.println(partyloopWaveMax);
      Serial.print("partyWaveLength ");
      Serial.println(partyWaveLength);
      Serial.print("partyloopSparkleMax ");
      Serial.println(partyloopSparkleMax);
      Serial.print("partySparkleFreq ");
      Serial.println(partySparkleFreq);
      Serial.println("-------------------------------------------------");
      mode = modeOff;
      standbyActive = false;
    }    
  
if (cmdStartsWith("save")) {                                      //save parameters to eeprom on save command
   cfg.brightness = brightness;
   cfg.dimmingStep = dimmingStep;
   cfg.dimmingAltStep = dimmingAltStep;
   cfg.dimmingStepForAll = dimmingStepForAll;
   cfg.brighteningStep = brighteningStep;
   cfg.brighteningAltStep = brighteningAltStep;
   cfg.brighteningStepForAll = brighteningStepForAll;
   cfg.partyStep = partyStep;
   cfg.partyloopRandMax = partyloopRandMax;
   cfg.partyloopBDMax = partyloopBDMax;
   cfg.partyloopScannerMax = partyloopScannerMax;
   cfg.partyloopWaveMax = partyloopWaveMax;
   cfg.partyWaveLength = partyWaveLength;
   cfg.partyloopNukeMax = partyloopNukeMax;
   cfg.partyloopSparkleMax = partyloopSparkleMax;
   cfg.partySparkleFreq = partySparkleFreq;
   cfg.standby = standby;
   cfg.alternative = alternative;
   cfg.standbyActive = standbyActive;
   cfg.lightThreshold = lightThreshold;
   cfg.darkThreshold = darkThreshold;
   cfg.waitDuration = waitDuration;
   saveConfig();
  }

  if (cmdStartsWith("brightness ")) {
     int exbrightness = atoi(cmd+11);
     brightness = constrain(exbrightness, 0, 4095);
     Serial.print("brightness: ");
     Serial.println(brightness);
  }
   if (cmdStartsWith("downButton ")) {
     int buttonClicks = atoi(cmd+11);
     if (buttonClicks == 1) {
      modeSet(0);
     }
     if (buttonClicks == 2) {
      standbyActive = !standbyActive;
      modeSetStandby();

     }
     if (buttonClicks == 3) {
      alternative=!alternative;
      mode = modeConfirm;
      Serial.print("mode: ");
      if (alternative) Serial.println("B");
      else Serial.println("A");
     }
     if (buttonClicks == 5) {
      standbyActive = false;
      partyTest = false;
      modeSetParty();
     
     }
     if (buttonClicks < 0) {
      modeSetAll();
     }   
   }
   
   if (cmdStartsWith("upButton ")) {
     int buttonClicks = atoi(cmd+9);
      if (buttonClicks == 1) {
      modeSet(1);
     }
     if (buttonClicks == 2) {
      standbyActive=!standbyActive;
      modeSetStandby();
     }
     if (buttonClicks == 3) {
      alternative=!alternative;
      mode = modeConfirm;
      Serial.print("mode: ");
      if (alternative) Serial.println("B");
      else Serial.println("A");
     }
     if (buttonClicks == 5) {
      standbyActive = false;
      partyTest = false;
      modeSetParty();
     
     }
     if (buttonClicks < 0) {
      modeSetAll();
     }   
   }
  if (cmdStartsWith("standby ")) {
    int exstandby = atoi(cmd+8);
    standby = constrain(exstandby, 0, 1024);
    Serial.print("standby: ");
    Serial.println(standby);
  }
  if (cmdStartsWith("waitDuration ")) {
    int exwaitDuration = atoi(cmd+13);
    waitDuration = constrain(exwaitDuration, 0, 30000);
    Serial.print("waitDuration: ");
    Serial.println(waitDuration);
  }
  if (cmdStartsWith("brighteningStep ")) {
    int exBrighteningStep = atoi(cmd+16);
    brighteningStep = constrain(exBrighteningStep, 1, 128);
    Serial.print("brighteningStep: ");
    Serial.println(brighteningStep);
  }
  if (cmdStartsWith("brighteningAltStep ")) { 
    int exBrighteningAltStep = atoi(cmd+19);
    brighteningAltStep = constrain(exBrighteningAltStep, 1, 2048);
    Serial.print("brighteningAltStep: ");
    Serial.println(brighteningAltStep);
  }
  if (cmdStartsWith("dimmingStep ")) {
    int exDimmingStep = atoi(cmd+12);
    dimmingStep = constrain(exDimmingStep, 1, 128);
    Serial.print("dimmingStep: ");
    Serial.println(dimmingStep);
  }
  if (cmdStartsWith("dimmingAltStep ")) {
    int exDimmingAltStep = atoi(cmd+15);
    dimmingAltStep = constrain(exDimmingAltStep, 1, 2048);
    Serial.print("dimmingAltStep: ");
    Serial.println(dimmingAltStep);
  }
  if (cmdStartsWith("brighteningStepForAll ")) {
    int exbrighteningStepForAll = atoi(cmd+22);
    brighteningStepForAll = constrain(exbrighteningStepForAll, 1, 512);
    Serial.print("brighteningStepForAll: ");
    Serial.println(brighteningStepForAll);
  }
  if (cmdStartsWith("dimmingStepForAll ")) {
    int exdimmingStepForAll = atoi(cmd+18);
    dimmingStepForAll = constrain(exdimmingStepForAll, 1, 512);
    Serial.print("dimmingStepForAll: ");
    Serial.println(dimmingStepForAll);
  }
  if (cmdStartsWith("partyStep ")) {
    int expartyStep = atoi(cmd+10);
    partyStep = constrain(expartyStep, 1, 2048);
    Serial.print("partyStep: ");
    Serial.println(partyStep);
  }
  if (cmdStartsWith("partyloopRandMax ")) {
    int expartyloopRandMax = atoi(cmd+17);
    partyloopRandMax = constrain(expartyloopRandMax, 1, 64);
    Serial.print("partyloopRandMax: ");
    Serial.println(partyloopRandMax);
  }
  if (cmdStartsWith("partyloopBDMax ")) {
    int expartyloopBDMax = atoi(cmd+15);
    partyloopBDMax = constrain(expartyloopBDMax, 1, 64);
    Serial.print("partyloopBDMax: ");
    Serial.println(partyloopBDMax);
  }
  if (cmdStartsWith("partyloopScannerMax ")) {
    int expartyloopScannerMax = atoi(cmd+20);
    partyloopScannerMax = constrain(expartyloopScannerMax, 1, 64);
    Serial.print("partyloopScannerMax: ");
    Serial.println(partyloopScannerMax);
  }
  if (cmdStartsWith("partyloopWaveMax ")) {
    int expartyloopWaveMax = atoi(cmd+17);
    partyloopWaveMax = constrain(expartyloopWaveMax, 1, 8192);
    Serial.print("partyloopWaveMax: ");
    Serial.println(partyloopWaveMax);
  }
  if (cmdStartsWith("partyWaveLength ")) {
    int expartyWaveLength = atoi(cmd+16);
    if (expartyWaveLength%2 != 0) expartyWaveLength--;
    partyWaveLength = constrain(expartyWaveLength, 2, 8);
    Serial.print("partyWaveLength: ");
    Serial.println(partyWaveLength);
    }
    
  if (cmdStartsWith("partyloopNukeMax ")) {
    int expartyloopNukeMax = atoi(cmd+17);
    partyloopNukeMax = constrain(expartyloopNukeMax, 1, 64);
    Serial.print("partyloopNukeMax: ");
    Serial.println(partyloopNukeMax);
  }
  if (cmdStartsWith("partyloopSparkleMax ")) {
    int expartyloopSparkleMax = atoi(cmd+20);
    partyloopSparkleMax = constrain(expartyloopSparkleMax, 1, 8192);
    Serial.print("partyloopSparkleMax: ");
    Serial.println(partyloopSparkleMax);
  }
   if (cmdStartsWith("partySparkleFreq ")) {
    int expartySparkleFreq = atoi(cmd+17);
    partySparkleFreq = constrain(expartySparkleFreq, 2, 256);
    Serial.print("partySparkleFreq: ");
    Serial.println(partySparkleFreq);
  }
  if (cmdStartsWith("lightThreshold ")) {
    int exlightThreshold = atoi(cmd+15);
    lightThreshold = constrain(exlightThreshold, 1, 1023);
    if (lightThreshold >= darkThreshold) {
    Serial.print("lightThreshold: ");
    Serial.println(lightThreshold);
    }
    else {
    Serial.print("lightThreshold ");
    Serial.println(darkThreshold+1);
    lightThreshold = darkThreshold + 1;
    }
  }
  if (cmdStartsWith("darkThreshold ")) {
    int exdarkThreshold = atoi(cmd+14);
    darkThreshold = constrain(exdarkThreshold, 1, 768);
    if (lightThreshold >= darkThreshold) {
    Serial.print("darkThreshold: ");
    Serial.println(darkThreshold);
  }
  else {
    Serial.print("darkThreshold ");
    Serial.println(lightThreshold-1);
    darkThreshold = lightThreshold-1;
    }
  }
  if (cmdStartsWith("LarsonScanner")) {
    Serial.println("LarsonScanner on");
    Serial.print("partyloopScannerMax: ");
    Serial.println(partyloopScannerMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartyLarsonScannerR;
    
  }
  if (cmdStartsWith("wave")) {
    Serial.println("wave on");
    Serial.print("partyloopWaveMax: ");
    Serial.println(partyloopWaveMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartyWaveDown;
  }
   if (cmdStartsWith("nuke")) {
    Serial.println("nuke on");
    Serial.print("partyloopNuke: ");
    Serial.println(partyloopNukeMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartyNukeIn;
  }
  if (cmdStartsWith("ping")) {
    Serial.println("ping on");
    Serial.print("partyloopBDMax: ");
    Serial.println(partyloopBDMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartyBFB;
  }
  if (cmdStartsWith("random")) {
    Serial.println("random on");
    Serial.print("partyloopRandMax: ");
    Serial.println(partyloopRandMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartyRandOn;
  }
  if (cmdStartsWith("sparkle")) {
    Serial.println("sparkle on");
    Serial.print("partySparkleFreq: ");
    Serial.println(partySparkleFreq);
     Serial.print("partyloopSparkleMax: ");
    Serial.println(partyloopSparkleMax);
    partyTest = true;
    standbyActive = false;
    off();
    mode = modePartySparkle;
  }
}
