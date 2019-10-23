#include <Tlc5940.h>
#include "ClickButton.h"
#include <EEPROM.h>

#define sensorPull LOW                          // HIGH for PIR sensor
#define baudrate 9600                           // HM10 bluetooth module

const int downButtonPin = 6;
const int upButtonPin = 5;
const int downSensorPin = 7;
const int upSensorPin = 8;
const int lightSensor = A1;
const int adjustmentPin = 12;
const int sensorPin0 = A0;

const int buttons = 4;  // Nr. of buttons in the array
  ClickButton button[4] = {
  ClickButton (downButtonPin, LOW),
  ClickButton (upButtonPin, LOW),
  ClickButton (downSensorPin, sensorPull),        
  ClickButton (upSensorPin, sensorPull),         
};

int LEDfunction[buttons]  = { 0, 0, 0, 0 };

const int downButton = 0;
const int upButton = 1;
const int downSensor = 2;
const int upSensor = 3;

const int modeOff = 0;
const int modeBrighteningFromBottom = 1;
const int modeDimmingFromBottom = 2;
const int modeBrighteningFromTop = 3;
const int modeDimmingFromTop = 4;
const int modeBrighteningAll = 5;
const int modeDimmingAll = 6;
const int modeOn = 7;
const int modePartyBFB = 8;
const int modePartyDFB = 9;
const int modePartyBFT = 10;
const int modePartyDFT = 11;
const int modePartyNukeIn = 12;
const int modePartyNukeOut = 13;
const int modePartyRandOn = 14;
const int modePartyRandOff = 15;
const int modeBrighteningAllStandby = 16;
const int modeBrighteningFromBottomAlt = 17;
const int modeDimmingFromBottomAlt = 18;
const int modeBrighteningFromTopAlt = 19;
const int modeDimmingFromTopAlt = 20;
const int modePartyLarsonScannerR = 22;
const int modePartyLarsonScannerL = 23;
const int modePartyWaveUp = 24;
const int modePartyWaveDown = 25;
const int modePartySparkle = 26;
const int modeConfirm = 27;
const int modeStandby = 28;


int ledCount = 16;                //nr. of led strips
int outs[] = {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0}; //outputs for led strips, inverted, first on top, last on bottom
int outState[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

int brightness = 1024;
int delayTime = 7;                // main loop delay
int prevValue = -1;               //for random number generator
int dimmingStep = 12;
int dimmingAltStep = 255;
int dimmingStepForAll = 8;
int brighteningStep = 96;
int brighteningAltStep = 100;
int brighteningStepForAll = 24;
int partyStep = 256;
int partyloopRand = 1;            //must be 1 
int partyloopRandMax = 2;         //number of randomOn and randomOff loops in partymode
int partyloopBD = 1;              //must be 1
int partyloopBDMax = 3;           //number of brightening and dimming loops in partymode
int partyloopScanner = 1;         //must be 1
int partyloopScannerMax = 4;      //number of larsonScanner loops in partymode
int partyWaveLength = 4;            //must be 1
int partyloopWave = 1;            //must be 1
int partyloopWaveMax = 1024;      //number of wave loops in partymode
int partyloopNuke = 1;            //must be 1
int partyloopNukeMax = 2;         //number of nuke loops in partymode
int partySparkleFreq = 8;
int partyloopSparkle = 1;   
int partyloopSparkleMax = 1024;         //number of nuke loops in partymode
int standby = 4;                  //brightness value for standby mode
int mode = modeOff;
int revolutionSpeed = 0;          //counter of party loop
int alternative = 0;              //alternative mode for brightening and dimming mode
int standbyActive = 0;            //for standby mode
char cmd[50];                     // command to store from serial port
int cmdIndex;
int mark = 1;                     //for wave mode, up and down
boolean partyTest = 0;            //for testing party modes, no loop, just one party mode
boolean state = 0;                // for light sensor, 0 is dark state
int lightThreshold = 700;         // for hysteresis of light sensor
int darkThreshold  = 300;         // for hysteresis of light sensor
int sensorState[] = {0,0};        // for single trigger sensors status change
int lastsensorState[] = {0,0};    // for single trigger sensors status change





struct settings                   //configuration with list of settings to save in eeprom memory
{
    int brightnessEeprom;
    int dimmingStepEeprom;
    int dimmingAltStepEeprom;
    int dimmingStepForAllEeprom;
    int brighteningStepEeprom;
    int brighteningAltStepEeprom;
    int brighteningStepForAllEeprom;
    int partyStepEeprom;
    int partyloopRandMaxEeprom;
    int partyloopBDMaxEeprom;
    int partyloopScannerMaxEeprom;
    int partyloopWaveMaxEeprom;
    int partyWaveLengthEeprom;
    int partyloopNukeMaxEeprom;
    int partyloopSparkleMaxEeprom;
    int partySparkleFreqEeprom;
    int standbyEeprom;
    int alternativeEeprom;
    int standbyActiveEeprom;
    int lightThresholdEeprom;
    int darkThresholdEeprom;
    
} cfg;






void setup()
{

  delay(500);                                         // wait for bluetooth module to start
  cmdIndex = 0;
  Serial.begin(baudrate);
  
  randomSeed(analogRead(sensorPin0));                 //analog input 0 pin, as the seed for random numbers generator 
  pinMode(lightSensor, INPUT);                        // light sensor as input A1 pin
  pinMode(adjustmentPin, INPUT);                      // light sensor as input 12 pin



  Tlc.init();

    button[0].debounceTime   = 20;                    // Debounce timer in ms
    button[0].multiclickTime = 600;                   // Time limit for multi clicks
    button[0].longClickTime  = 1100;                  // Time until long clicks register
    button[1].debounceTime   = 20;                    // Debounce timer in ms
    button[1].multiclickTime = 600;                   // Time limit for multi clicks
    button[1].longClickTime  = 1100;                  // Time until long clicks register
    button[2].debounceTime   = 20;                    // Debounce timer in ms
    button[2].multiclickTime = 50;                    // Time limit for multi clicks
    button[2].longClickTime  = 100000;                // Time until long clicks register
    button[3].debounceTime   = 20;                    // Debounce timer in ms
    button[3].multiclickTime = 50;                    // Time limit for multi clicks
    button[3].longClickTime  = 100000;                // Time until long clicks register
   
   EEPROM.get(0, cfg);                                   //read configuration from eeprom, and change some parameters
   brightness = cfg.brightnessEeprom;
   dimmingStep = cfg.dimmingStepEeprom;
   dimmingAltStep = cfg.dimmingAltStepEeprom;
   dimmingStepForAll = cfg.dimmingStepForAllEeprom;
   brighteningStep = cfg.brighteningStepEeprom;
   brighteningAltStep = cfg.brighteningAltStepEeprom;
   brighteningStepForAll = cfg.brighteningStepForAllEeprom;
   partyStep = cfg.partyStepEeprom;
   partyloopRandMax = cfg.partyloopRandMaxEeprom;
   partyloopBDMax = cfg.partyloopBDMaxEeprom;
   partyloopScannerMax = cfg.partyloopScannerMaxEeprom;
   partyWaveLength = cfg.partyWaveLengthEeprom;
   partyloopWaveMax = cfg.partyloopWaveMaxEeprom;
   partyloopNukeMax = cfg.partyloopNukeMaxEeprom;
   partyloopSparkleMax = cfg.partyloopSparkleMaxEeprom;
   partySparkleFreq = cfg.partySparkleFreqEeprom;
   standby = cfg.standbyEeprom;
   alternative = cfg.alternativeEeprom;
   standbyActive = cfg.standbyActiveEeprom;
   lightThreshold = cfg.lightThresholdEeprom;
   darkThreshold = cfg.darkThresholdEeprom;
   if (standbyActive == 1) mode = modeBrighteningAllStandby;


    int input = analogRead(lightSensor);
    if (input < darkThreshold) state = 0;
    else state = 1;
   

   updateLed();
}





void loop()
{
    for (int i=0; i < buttons; i++) {
      button[i].Update();                   // Update state of all button
      if (button[i].clicks != 0) {          // Save click codes in LEDfunction, as clicks counts are reset at next Update()
        LEDfunction[i] = button[i].clicks; 
      }

      
      if(i < 2) {
        
      if (button[i].clicks == 1) {          // for stairs brightening or dimming
        modeSet(i);
      }
      
       if (button[i].clicks == 2) {         // standby mode
         standbyActive=!standbyActive;
         modeSetAllStandby();
      }
       if (button[i].clicks == 3) {         // confirm of change for stairs brightening or dimming
        alternative=!alternative;
        mode = modeConfirm;
      }
      if (button[i].clicks == 5) {          // party mode
         standbyActive = 0;
         modeSetParty();
      }
       if(LEDfunction[i] == -1) {           // brightening all or dimming all with constant mode
       modeSetAll();
       LEDfunction[i] = 0;
      }
    }
    
    else {
          lightSensorRead();
          sensorState[i-2]=button[i].depressed;
          if ((sensorState[i-2] != lastsensorState[i-2]) && (sensorState[i-2] == (!sensorPull))) {     // triggering lights only when change of state and sensors state active
            if (!state) modeSet(i);
            else lightSensorPrint(i);     
          }
          lastsensorState[i-2] = sensorState[i-2];
    }   
  } 

      
    updateCircuitState(); 
    updateLed();
    readFromSerial();   
    delay(delayTime);
}





void updateLed() {
   for (int diode = 0; diode < ledCount;diode++) {
     Tlc.set(outs[diode], outState[diode]);
    }
    Tlc.update();
}





void updateCircuitState() {
  switch (mode) {
  case modeOff:
    off(); break;
  case modeBrighteningFromBottom:
    brighteningFromBottom(); break;
  case modeDimmingFromBottom:
    dimmingFromBottom(); break;
  case modeBrighteningFromTop:
    brighteningFromTop(); break;
  case modeDimmingFromTop:
    dimmingFromTop(); break;
  case modeBrighteningAll:
    brighteningAll(); break;
  case modeDimmingAll:
    dimmingAll(); break;
  case modePartyBFB:
    partyBFB(); break;
  case modePartyDFB:
    partyDFB(); break;
  case modePartyBFT:
    partyBFT(); break;
  case modePartyDFT:
    partyDFT(); break;
  case modePartyNukeIn:
    partyNukeIn(); break;
  case modePartyNukeOut:
    partyNukeOut(); break;
  case modePartySparkle:
    partySparkle(); break;
  case modePartyRandOn:
    partyRandOn(); break;
  case modePartyRandOff:
    partyRandOff(); break;
  case modeBrighteningAllStandby:
    brighteningAllStandby(); break;
  case modeBrighteningFromBottomAlt:
    brighteningFromBottomAlt(); break;
  case modeDimmingFromBottomAlt:
    dimmingFromBottomAlt(); break;
  case modeBrighteningFromTopAlt:
    brighteningFromTopAlt(); break;
  case modeDimmingFromTopAlt:
   dimmingFromTopAlt(); break;
  case modePartyLarsonScannerR:
    partyLarsonScannerR();break;
  case modePartyLarsonScannerL:
    partyLarsonScannerL(); break;
  case modePartyWaveUp:
    partyWaveUp(); break;
  case modePartyWaveDown:
    partyWaveDown(); break;
  case modeConfirm:
    confirm(); break;
  }
}





void lightSensorRead() {
   //Input Hysteresis for fotoresistor light value
        int input = analogRead(lightSensor);

          if (state) {
            if (input < darkThreshold)
            state = 0;
          }
          else  {
            if (input > lightThreshold) 
            state = 1;
          }          
}




void lightSensorPrint(int button) {
  
        int input = analogRead(lightSensor);
        Serial.print("lightValue: ");
        Serial.println(input);
        if (state) Serial.println("sensors off");
        else Serial.println("sensors on");
        if (button == downSensor) Serial.println("bottom sensor");
        if (button == upSensor) Serial.println("top sensor");
        if (button == downButton) Serial.println("bottom button");
        if (button == upButton) Serial.println("top button");
        
}





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
      Serial.print("alternative ");
      Serial.println(alternative); 
      Serial.print("standbyActive ");
      Serial.println(standbyActive);
      Serial.print("Mode ");
      Serial.println(mode);
      Serial.println("-------------------------------------------------");
      mode = modeOff;
      partyTest = 0;
    }

    
if (cmdStartsWith("synchro1")) {
      Serial.println("--------- light sensor config ----------");
      Serial.print("lightThreshold ");
      Serial.println(lightThreshold);   
      Serial.print("darkThreshold ");
      Serial.println(darkThreshold); 
      Serial.print("light sensor state: ");
      if (state == 0) Serial.println("dark"); 
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
      standbyActive = 0;
      
    }    
  
if (cmdStartsWith("save")) {                                      //save parameters on save command
   cfg.brightnessEeprom = brightness;
   cfg.dimmingStepEeprom = dimmingStep;
   cfg.dimmingAltStepEeprom = dimmingAltStep;
   cfg.dimmingStepForAllEeprom = dimmingStepForAll;
   cfg.brighteningStepEeprom = brighteningStep;
   cfg.brighteningAltStepEeprom = brighteningAltStep;
   cfg.brighteningStepForAllEeprom = brighteningStepForAll;
   cfg.partyStepEeprom = partyStep;
   cfg.partyloopRandMaxEeprom = partyloopRandMax;
   cfg.partyloopBDMaxEeprom = partyloopBDMax;
   cfg.partyloopScannerMaxEeprom = partyloopScannerMax;
   cfg.partyloopWaveMaxEeprom = partyloopWaveMax;
   cfg.partyWaveLengthEeprom = partyWaveLength;
   cfg.partyloopNukeMaxEeprom = partyloopNukeMax;
   cfg.partyloopSparkleMaxEeprom = partyloopSparkleMax;
   cfg.partySparkleFreqEeprom = partySparkleFreq;
   cfg.standbyEeprom = standby;
   cfg.alternativeEeprom = alternative;
   cfg.standbyActiveEeprom = standbyActive;
   cfg.lightThresholdEeprom = lightThreshold;
   cfg.darkThresholdEeprom = darkThreshold;
   EEPROM.put(0, cfg);
   Serial.println("configuration saved");
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
      standbyActive=!standbyActive;
      modeSetAllStandby();

     }
     if (buttonClicks == 3) {
      alternative=!alternative;
      mode = modeConfirm;
      Serial.print("mode: ");
      if (alternative) Serial.println("B");
      else Serial.println("A");
     }
     if (buttonClicks == 5) {
      standbyActive = 0;
      partyTest = 0;
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
      modeSetAllStandby();
     }
     if (buttonClicks == 3) {
      alternative=!alternative;
      mode = modeConfirm;
      Serial.print("mode: ");
      if (alternative) Serial.println("B");
      else Serial.println("A");
     }
     if (buttonClicks == 5) {
      standbyActive = 0;
      partyTest = 0;
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
  if (cmdStartsWith("brighteningStep ")) {
    int exBrighteningStep = atoi(cmd+16);
    brighteningStep = constrain(exBrighteningStep, 1, 2048);
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
    dimmingStep = constrain(exDimmingStep, 1, 2048);
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
    brighteningStepForAll = constrain(exbrighteningStepForAll, 1, 2048);
    Serial.print("brighteningStepForAll: ");
    Serial.println(brighteningStepForAll);
  }
  if (cmdStartsWith("dimmingStepForAll ")) {
    int exdimmingStepForAll = atoi(cmd+18);
    dimmingStepForAll = constrain(exdimmingStepForAll, 1, 2048);
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
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartyLarsonScannerR;
    
  }
  if (cmdStartsWith("wave")) {
    Serial.println("wave on");
    Serial.print("partyloopWaveMax: ");
    Serial.println(partyloopWaveMax);
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartyWaveDown;
  }
   if (cmdStartsWith("nuke")) {
    Serial.println("nuke on");
    Serial.print("partyloopNuke: ");
    Serial.println(partyloopNukeMax);
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartyNukeIn;
    
  }
  if (cmdStartsWith("ping")) {
    Serial.println("ping on");
    Serial.print("partyloopBDMax: ");
    Serial.println(partyloopBDMax);
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartyBFB;
  }

  if (cmdStartsWith("random")) {
    Serial.println("random on");
    Serial.print("partyloopRandMax: ");
    Serial.println(partyloopRandMax);
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartyRandOn;
  }

  if (cmdStartsWith("sparkle")) {
    Serial.println("sparkle on");
    Serial.print("partySparkleFreq: ");
    Serial.println(partySparkleFreq);
     Serial.print("partyloopSparkleMax: ");
    Serial.println(partyloopSparkleMax);
    partyTest = 1;
    standbyActive = 0;
    off();
    mode = modePartySparkle;
  }
  
}





void off()  {                                         //turn all LEDs off
  
   for (int diode = 0; diode < ledCount; diode++)
    if (outState[diode] > 0) outState[diode] = 0;
    
}





void confirm() {                                      //turn on for a moment last and first LED, to confirm the change alternative mode 
   bool changed = false;
   bool on = false;
   int zero;
    if (!standbyActive) zero = 0;                     //check for standby mode
    else zero = standby;
     for (int diode = 1; diode < ledCount-1; diode++)
     if (outState[diode] > zero) on = true;
      if (on) {
        for (int diode = 0; diode < ledCount; diode++) {
        if (outState[diode] > zero) outState[diode] = zero;
        }
      changed = true;
      }
   
      if ((outState[0] < brightness) || (outState[ledCount-1] < brightness)) {
       int newState = outState[0];
      newState = newState + dimmingStep;
      if (newState > brightness) 
      newState = brightness;
      outState[0] = newState;
      outState[ledCount-1] = newState;
      changed = true;
      }
     else {
      for (int diode = 0; diode<ledCount;diode++) outState[diode] = zero;
     }
    
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;     
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
      outState[15] = brightness;
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







void brighteningAll() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      newState = newState + brighteningStepForAll;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      changed = true;
    }
  }
   if (!changed) mode = modeOn;
}



void dimmingAll() {
  bool changed = false;
  int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] > zero) {
      int newState = outState[diode];
      newState = newState - dimmingStepForAll;
      if (newState < zero) newState = zero;
      outState[diode] = newState;
      changed = true;
    }
  }
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;  
}





void brighteningAllStandby() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {
    if (outState[diode] != standby) {
      outState[diode] = standby;
      changed = true;
    }
  }
   if (!changed) mode = modeStandby;
}





void brighteningFromBottom() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      newState = newState + brighteningStep;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      changed = true;
      break;
    }    
  }
    if (!changed) mode = modeDimmingFromBottom;
}



void brighteningFromTop() {
  bool changed = false;
  for (int diode = ledCount-1; diode >= 0; diode--) {   
    if (outState[diode] < brightness) {
      int newState = outState[diode];
      newState = newState + brighteningStep;
      if (newState > brightness) newState = brightness;
      outState[diode] = newState;
      changed = true;
      break;
    }   
  }
  if (!changed) mode = modeDimmingFromTop;
}





void dimmingFromBottom() {
  bool changed = false;
  int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = 0; diode < ledCount; diode++) {   
    if (outState[diode] > zero) {
      int newState = outState[diode];
      newState = newState - dimmingStep;
      if (newState < zero) newState = zero;
      outState[diode] = newState;
      changed = true;
      break;
    }
   }
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;  
}



void dimmingFromTop() {
  bool changed = false;
  int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = ledCount-1; diode >= 0; diode--) {   
    if (outState[diode] > zero) {
      int newState = outState[diode];
      newState = newState - dimmingStep;
      if (newState < zero) newState = zero;
      outState[diode] = newState;
      changed = true;
      break;
    }
  }
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;  
}





void brighteningFromTopAlt() {
  bool changed = false;
  bool revolution = false;
  int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = 1; diode < ledCount; diode++) {   
    if ((outState[diode] < brightness) && (outState[diode-1] > zero)) {
          int newState = outState[diode];
         int beta = map(revolutionSpeed, 4, 16, 8, 1/5); 
        if (revolutionSpeed < 4)
          newState = newState + brighteningAltStep * 18;
          else
           newState = newState + brighteningAltStep * beta;
          if (newState > brightness) newState = brightness;
          outState[diode] = newState;
          newState = outState[diode-1];
           if (revolutionSpeed < 4)
          newState = newState - brighteningAltStep * 18;
          else
           newState = newState - brighteningAltStep * beta;
          if (newState < zero) newState = zero;
          outState[diode-1] = newState;
          changed = true;
          revolution = true;
          break;
    } 
 }  
 if (!revolution) {           //highly experimental code
    if (outState[0] == zero) {
      outState[0] = brightness;
      changed = true;
     revolutionSpeed++;
    }
    else changed = false;
    }
 if (!changed) {
  revolutionSpeed = 0;
  mode = modeDimmingFromTopAlt;
 }
}





void dimmingFromTopAlt() {
  bool changed = false;
  bool revolution = false;
   int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = 1; diode < ledCount; diode++) {   
    if ((outState[diode] > zero) && (outState[diode-1] < brightness)) {
          int newState = outState[diode-1];
          newState = newState + dimmingAltStep;
          if (newState > brightness) newState = brightness;
          outState[diode-1] = newState;
          newState = outState[diode];
          newState = newState - dimmingAltStep;
          if (newState < zero) newState = zero;
          outState[diode] = newState;
      changed = true;
      revolution = true;
      break;
    } 
  }  
  if (!revolution) {
      if (outState[0] == brightness) {
      outState[0] = zero;
      changed = true;
    }
      else changed = false;
    }
  
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;  
}





void brighteningFromBottomAlt() {
  bool changed = false;
  bool revolution = false;
    int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = ledCount-2; diode >= 0; diode--) {   
    if ((outState[diode] < brightness) && (outState[diode+1] > zero)) {
          int newState = outState[diode];
         int beta = map(revolutionSpeed, 4, 16, 8, 1/5); 
        if (revolutionSpeed < 4)
          newState = newState + brighteningAltStep * 18;
          else
           newState = newState + brighteningAltStep * beta;
          if (newState > brightness) newState = brightness;
          outState[diode] = newState;
          newState = outState[diode+1];
        if (revolutionSpeed < 4)
          newState = newState - brighteningAltStep * 18;
          else
           newState = newState - brighteningAltStep * beta;
          if (newState < zero) newState = zero;
          outState[diode+1] = newState;
      changed = true;
      revolution = true;
      break;
    } 
 }  
 if (!revolution) {                         //highly experimental code
    if (outState[ledCount-1] == zero) {
      outState[ledCount-1] = brightness;
      changed = true;
      revolutionSpeed++;
    }
    else changed = false;
    }
 if (!changed) {
  mode = modeDimmingFromBottomAlt;
  revolutionSpeed = 0;
 }
}



void dimmingFromBottomAlt() {
  bool changed = false;
  bool revolution = false;
    int zero;
  if (!standbyActive) zero = 0; 
  else zero = standby;
  for (int diode = ledCount-2; diode >= 0; diode--) {   
    if ((outState[diode] > zero) && (outState[diode+1] < brightness)) {
          int newState = outState[diode+1];
          newState = newState + dimmingAltStep;
          if (newState > brightness) newState = brightness;
          outState[diode+1] = newState;
          newState = outState[diode];
          newState = newState - dimmingAltStep;
          if (newState < zero) newState = zero;
          outState[diode] = newState;
          changed = true;
          revolution = true;
          break;
    } 
  }  
  if (!revolution) {
      if (outState[ledCount-1] == brightness) {
      outState[ledCount-1] = zero;
      changed = true;
    }
      else changed = false;
    }
   if ((!changed) && (!standbyActive)) mode = modeOff;  
   if ((!changed) && (standbyActive)) mode = modeStandby;  
}








void modeSet(int button) {

      lightSensorPrint(button);
    
    if (mode == modeOff) {    //if leds are off, then brightening
      if (button == downButton || button == downSensor) {
      if (alternative) mode = modeBrighteningFromBottomAlt;
      else mode = modeBrighteningFromBottom;
      }
      
    if (button == upButton || button == upSensor) {
       if (alternative) mode = modeBrighteningFromTopAlt;
       else mode = modeBrighteningFromTop;
      }
    }

    if (mode == modeStandby || mode == modeDimmingFromBottomAlt || mode == modeDimmingFromBottom || mode == modeDimmingFromTopAlt || mode == modeDimmingFromTop || 
      mode == modeBrighteningFromBottomAlt || mode == modeBrighteningFromBottom || mode == modeBrighteningFromTopAlt || mode == modeBrighteningFromTop) {    
        if (button == downButton || button == downSensor) {
          if (alternative) mode = modeBrighteningFromBottomAlt;
          else mode = modeBrighteningFromBottom;
        }
    if (button == upButton || button == upSensor) {
       if (alternative) mode = modeBrighteningFromTopAlt;
       else mode = modeBrighteningFromTop;
      }
    }
}





void modeSetAll() {

    if (mode == modeOff || mode == modeStandby || mode == modeDimmingFromTop || mode == modeDimmingFromBottom || mode == modeDimmingFromTopAlt || mode == modeDimmingFromBottomAlt)
      {
        mode = modeBrighteningAll;
        Serial.println("All On");
      }
      
    if (mode == modeOn || mode == modeBrighteningFromTop || mode == modeBrighteningFromBottom || mode == modeBrighteningFromTopAlt || mode == modeBrighteningFromBottomAlt ||
      mode == modePartyBFB || mode == modePartyDFB || mode == modePartyBFT || mode == modePartyDFT || mode == modePartyNukeIn || mode == modePartyNukeOut || mode == modePartySparkle ||
      mode == modePartyRandOn || mode == modePartyRandOff || mode == modePartyLarsonScannerL || mode == modePartyLarsonScannerR || mode == modePartyWaveUp || mode == modePartyWaveDown)
      {
        mode = modeDimmingAll;
        Serial.println("All Off");
      }
}





void modeSetAllStandby() {

    if (mode == modeOff || mode == modeOn || mode == modeDimmingFromTop || mode == modeDimmingFromBottom || mode == modeDimmingFromTopAlt || mode == modeDimmingFromBottomAlt || 
    mode == modeBrighteningFromTop || mode == modeBrighteningFromBottom || mode == modeBrighteningFromTopAlt || mode == modeBrighteningFromBottomAlt || 
    mode == modePartyBFB || mode == modePartyDFB || mode == modePartyBFT || mode == modePartyDFT || mode == modePartyNukeIn || mode == modePartyNukeOut || mode == modePartySparkle ||
    mode == modePartyRandOn || mode == modePartyRandOff || mode == modePartyLarsonScannerL || mode == modePartyLarsonScannerR || mode == modePartyWaveUp || mode == modePartyWaveDown)
    
    mode = modeBrighteningAllStandby;
    
    
    if (mode == modeStandby) mode = modeOff;

    Serial.print("standbyActive: ");
    Serial.println(standbyActive);
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



