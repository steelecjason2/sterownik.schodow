

int getRandomNoDuplicate(int lower, int upper)  {
  static int offset = 0;
  int randNumber = random(lower, upper);
  if (prevValue == randNumber)
  {
    offset++;
    if (offset >= upper - lower) offset = 1;
    randNumber += offset;
    if (randNumber >= upper) randNumber = lower + randNumber - upper;
  }
  prevValue = randNumber;
  return randNumber;
}

void partyWaveDown() {
  
  bool off = true;
   for (int diode = 0; diode < ledCount; diode++) 
    if (outState[diode] > 0) off = false;
   if (off) {
      for (int diode = 0; diode < ledCount; diode++) {   
      double sinus = (1+sin(11.25 * partyWaveLength*(diode)*PI/180))*brightness/2;
      outState[diode] = (int) sinus;
    }
   }
   
  for (int diode = 0; diode < ledCount; diode++) {   
     int Step = map(partyStep, 1, 2048, 8, 384); 
     Step = constrain(Step, 8, 384); 
     int nState = outState[diode] - Step*mark;
      if (nState <= 0) {
        nState = 0;
        mark = -1;
      } else if (nState > brightness) {
        nState = brightness;
        mark = 1;
      }
      outState[diode] = nState;  
  }
  if (partyloopWave%100 == 0) {
    Serial.print("partyloopWave: ");
    Serial.println(partyloopWave);
  }
  partyloopWave++;
  
 if (partyloopWave > partyloopWaveMax) {
      partyloopWave = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      mode = modePartyWaveUp;
  }

}  

void partyWaveUp() {

 bool off = true;
   for (int diode = 0; diode < ledCount; diode++) 
    if (outState[diode] > 0) off = false;
   if (off) {
      for (int diode = 0; diode < ledCount; diode++) {   
      double sinus = (1+sin(11.25 * partyWaveLength*(diode)*PI/180))*brightness/2;
      outState[diode] = (int) sinus;
    }
   }


  for (int diode = ledCount-1; diode >= 0; diode--) {   
      int Step = map(partyStep, 1, 2048, 8, 384);
      Step = constrain(Step, 8, 384);   
      int nState = outState[diode] + Step*mark;
      if (nState <= 0) {
        nState = 0;
        mark = 1;
      }
      else if (nState > brightness) {
        nState = brightness;
        mark = -1;
      }
      outState[diode] = nState;   
  }  
   if (partyloopWave%100 == 0) {
    Serial.print("partyloopWave: ");
    Serial.println(partyloopWave);
   }
  partyloopWave++;
  
 if (partyloopWave > partyloopWaveMax) {
      partyloopWave = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartyLarsonScannerR;
      else
      mode = modeOff;
  }

}

void partyLarsonScannerR() {                        //experimental code, don't know why it works
   bool changed = false;
  bool off = true;
  int newState;
   for (int diode = 0; diode < ledCount; diode++)
    if (outState[diode] > 0) off = false;
   if (off) {
      outState[ledCount-1] = brightness;
      changed = true;
   }
  for (int diode = ledCount-2; diode>0; diode--) {   
    if (outState[diode+1] > 0){
        if (outState[diode+2] > 0) {
      newState = outState[diode+2];
      newState = newState - partyStep;
      if (newState < 0) newState = 0;
      outState[diode+2] = newState;
      }
      if (outState[diode+1] > 0) {
      newState = outState[diode+1];
      newState = newState - partyStep;
      if (newState < 0) newState = 0;
      outState[diode+1] = newState;
      }
      if (outState[diode] < brightness) {
      newState = outState[diode];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      }
      if (diode>=1){
      newState = outState[diode-1];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode-1] = newState;
      }
      if (diode>=2){
      newState = outState[diode-2];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode-2] = newState;
      }
      if (diode>=3){
      newState = outState[diode-3];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode-3] = newState;
      }
      changed = true;
      break;
    }   
  }
//highly experimental code
   if (partyloopScanner > partyloopScannerMax) {
      partyloopScanner = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartyNukeIn;
      else 
      mode = modeOff;
 
  }
  if (!changed)  {
     partyloopScanner++;  
     mode = modePartyLarsonScannerL;
  }   
}

void partyLarsonScannerL() {                            //experimental code, don't know why it works
   bool changed = false;
   int nState;
  for (int diode = 1; diode < ledCount-1; diode++) {   
    if (outState[diode-1] > 0){
      nState = outState[diode-2];
      nState = nState - partyStep;
      if (nState < 0) nState = 0;
      outState[diode-2] = nState;
      nState = outState[diode-1];
      nState = nState - partyStep;
      if (nState < 0) nState = 0;
      outState[diode-1] = nState;
      nState = outState[diode];
      nState = nState + partyStep;
      if (nState > brightness) nState = brightness;
      outState[diode] = nState;
      if (diode < ledCount-1) {
      nState = outState[diode+1];
      nState = nState + partyStep;
      if (nState > brightness) nState = brightness;
      outState[diode+1] = nState;
      }
      if (diode < ledCount-2) {
      nState = outState[diode+2];
      nState = nState + partyStep;
      if (nState > brightness) nState = brightness;
      outState[diode+2] = nState;
      }
       if (diode < ledCount-3) {
      nState = outState[diode+3];
      nState = nState + partyStep;
      if (nState > brightness) nState = brightness;
      outState[diode+3] = nState;
      }
      changed = true;
      break;
    }   
  }
  if (!changed) mode = modePartyLarsonScannerR;  
}

void partyNukeIn() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      double sinus = sin(11*(diode+1)*PI/180)*partyStep/2;
     
      int Step = (int) sinus+3 ;
      newState = newState + Step;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      changed = true;
    }
  }
  if (partyloopNuke > partyloopNukeMax) {           //highly experimental code
      partyloopNuke = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartySparkle;
      else
      mode = modeOff;
  }
   if (!changed)  {
     partyloopNuke++;  
     mode = modePartyNukeOut;
  } 
   
}
 
void partyNukeOut() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] > 0) {
      int newState = outState[diode];
      double sinus = sin(11*(diode+1)*PI/180)*partyStep/4;
    
      int Step = (int) sinus +3;
      newState = newState - Step;
      if (newState < 0) newState = 0;
      outState[diode] = newState;
      changed = true;
    }
  }

if (!changed) mode = modePartyNukeIn;
}

void partySparkle() {
 
 for (int diode = 0; diode < ledCount; diode++){
 if (random(partySparkleFreq) == 0) outState[diode] = map(random(30)+70, 0, 100, 0, brightness);

    if (outState[diode] > 0){
      int newState = outState[diode];
      newState = newState - brightness/8;
      if (newState < 0) newState = 0;
      outState[diode] = newState;
      }
 }
   
   if (partyloopSparkle%100 == 0) {
    Serial.print("partyloopSparkle: ");
    Serial.println(partyloopSparkle);
   }

   partyloopSparkle++;
  int delaymode = map(brightness, 1024, 4095, 36, 28);  
  delay(delaymode);
 if (partyloopSparkle > partyloopSparkleMax) {
      partyloopSparkle = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartyRandOn;
      else
      mode = modeOff;
  }
 }

void partyRandOn() {
  bool changed = false;
  bool on = false;
  for (int diode = 0; diode < ledCount; diode++){
    if (outState[diode] == 0) changed=true;
  }
  if (changed){
    do {
      int x = getRandomNoDuplicate(0, ledCount); 
      if (outState[x] == 0)  {
      outState[x] = brightness;
      on = true;
      }
      else on = false;
    }  
    while (!on);
  }

  if (partyloopRand > partyloopRandMax) {           //highly experimental code
      partyloopRand = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartyBFB;
      else
      mode = modeOff;
  }
  if (!changed)  {
     partyloopRand++;  
     mode = modePartyRandOff;
  } 
  int delaymode = map(partyStep, 1, 2048, 512, 128);  
  delay(delaymode);
}

void partyRandOff() {
  bool changed = false;
  bool off = false;
  for (int diode = 0; diode < ledCount; diode++)
    if (outState[diode] == brightness) changed=true;
  if (changed){
    do {
      int x = getRandomNoDuplicate(0, ledCount); 
      if (outState[x] == brightness)  {
      outState[x] = 0;
      off = true;
      }
      else off = false;
    }  
    while (!off);
  }

  if (!changed) mode = modePartyRandOn;
 int delaymode = map(partyStep, 1, 2048, 512, 128);  
  delay(delaymode);
}

void partyBFB() {
  bool changed = false;

  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      
      changed = true;
      break;
    }    
  }

//highly experimental code
 if (partyloopBD > partyloopBDMax) {
      partyloopBD = 1;
      for (int diode = 0; diode < ledCount; diode++)
      if (outState[diode] > 0) outState[diode] = 0;
      if (!partyTest)
      mode = modePartyWaveDown;
      else
      mode = modeOff;
  }
  if (!changed)  {
     partyloopBD++;  
     mode = modePartyDFB;
  }
}

void partyDFB() {
   bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] > 0) {
      int newState = outState[diode];
      newState = newState - partyStep;
      if (newState < 0) newState = 0;
      outState[diode] = newState;
      changed = true;
      break;
    }
   }
    if (!changed) mode = modePartyBFT;   
}

void partyBFT() {
   bool changed = false;
  for (int diode = ledCount-1; diode >= 0; diode--) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      newState = newState + partyStep;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      changed = true;
      break;
    }   
  }
  if (!changed) mode = modePartyDFT;
}

void partyDFT() {
   bool changed = false;
  for (int diode = ledCount-1; diode >= 0; diode--) {   
    if (outState[diode] > 0) {
      int newState = outState[diode];
      newState = newState - partyStep;
      if (newState < 0) newState = 0;
      outState[diode] = newState;
      changed = true;
      break;
    }
  }
  if (!changed)  mode = modePartyBFB;
}

void modeSetParty() {
 
  if (mode == modePartyBFB || mode == modePartyDFB || mode == modePartyBFT || mode == modePartyDFT || mode == modePartyNukeIn || mode == modePartyNukeOut || mode == modePartySparkle || 
     mode == modePartyRandOn || mode == modePartyRandOff || mode == modePartyLarsonScannerR || mode == modePartyLarsonScannerL || mode == modePartyWaveUp || mode == modePartyWaveDown )  
      {
         mode = modeDimmingAll;
         Serial.println("Party Mode Off");  
      }
   else  {
    mode = modePartyLarsonScannerR;  
    Serial.println("Party Mode On");
   }
}