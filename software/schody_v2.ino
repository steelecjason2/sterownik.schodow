#include <Tlc5940.h>
#include "ClickButton.h"
#include <EEPROM.h>

#define sensorPull LOW                                // HIGH for PIR sensor
#define baudrate 115200                               //  bluetooth module or diag. console

const int downButtonPin = 6;
const int upButtonPin = 5;
const int downSensorPin = 7;
const int upSensorPin = 8;
const int lightSensor = A1;
const int adjustmentPin = 12;
const int sensorPin0 = A0;


const int buttons = 4;                                // Nr. of buttons in the array
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
const int modeBrighteningStandby = 16;
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
const int modeWait = 29;

const int ledCount = 16;                              //nr. of led strips
int outs[ledCount]; 
int outState[ledCount];

int brightness = 1024;
int delayTime = 7;                                    // main loop delay
int prevValue = -1;                                   //for random number generator
int dimmingStep = 12;
int dimmingAltStep = 255;
int dimmingStepForAll = 8;
int brighteningStep = 96;
int brighteningAltStep = 100;
int brighteningStepForAll = 24;
int partyStep = 256;
int partyloopRand = 1;                                //must be 1 
int partyloopRandMax = 2;                             //number of randomOn and randomOff loops in partymode
int partyloopBD = 1;                                  //must be 1
int partyloopBDMax = 3;                               //number of brightening and dimming loops in partymode
int partyloopScanner = 1;                             //must be 1
int partyloopScannerMax = 4;                          //number of larsonScanner loops in partymode
int partyWaveLength = 4;                              //must be 1
int partyloopWave = 1;                                //must be 1
int partyloopWaveMax = 1024;                          //number of wave loops in partymode
int partyloopNuke = 1;                                //must be 1
int partyloopNukeMax = 2;                             //number of nuke loops in partymode
int partySparkleFreq = 8;
int partyloopSparkle = 1;   
int partyloopSparkleMax = 1024;                       //number of nuke loops in partymode
int standby = 4;                                      //brightness value for standby mode
int mode = modeOff;
int revolutionSpeed = 0;                              //counter of party loop
bool alternative;                                     //alternative mode for brightening and dimming mode
bool standbyActive;                                   //for standby mode
char cmd[50];                                         // command to store from serial port
int cmdIndex;
int mark = 1;                                         //for wave mode, up and down
bool partyTest;                                       //for testing party modes, no loop, just one party mode
bool stateLight;                                      // for light sensor, 0 is dark state
int lightThreshold = 700;                             // for hysteresis of light sensor
int darkThreshold  = 300;                             // for hysteresis of light sensor
int nextMode;

int sensorState[] = {0,0};                            // for single trigger sensors status change
int lastSensorState[] = {0,0};                        // for single trigger sensors status change


unsigned long timeNow = 0;
unsigned long timePrevSensor = 0;
unsigned long timeDiffSensor = 1000;                  // period for light status check
unsigned long timePrevMode = 0;
unsigned long waitDuration = 3000;                    // time between brightening and dimming





void setup()
{

  delay(500);                                         // wait for bluetooth module to start
  cmdIndex = 0;
  Serial.begin(baudrate);

  mode = 0;
  alternative = false;
  standbyActive = false;
  partyTest = false;


  if(!loadConfig()){                                  //loading settings from eeprom memory, and checking if there is any configuration
    Serial.println("configuration not loaded!");
    saveConfig();                                     // overwrite with the default settings
  }
  
  if (standbyActive == 1) mode = modeBrighteningStandby;

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
   
    int input = analogRead(lightSensor);              //check for light state
    if (input < darkThreshold) stateLight = false;
    else stateLight = true;

    for (int i = 0; i < ledCount; i++) {
      outs[i] = ledCount - i - 1;                     //outputs for led strips, inverted, first on top, last on bottom
      outState[i] = 0;
    } 

   updateLed();

}

void loop()
{
  timeNow = millis();
  
    for (int i = 0; i < buttons; i++) {
      button[i].Update();                             // Update state of all buttons
      if (button[i].clicks != 0) {                    // Save click codes in LEDfunction, as clicks counts are reset at next Update()
        LEDfunction[i] = button[i].clicks; 
      }
      
      if(i < 2) {
        
        if (button[i].clicks == 1) {                   // for stairs brightening or dimming
          modeSet(i);
        }
      
        if (button[i].clicks == 2) {                    // standby mode
          standbyActive=!standbyActive;
          modeSetStandby();
        }
        if (button[i].clicks == 3) {                    // confirm of change for stairs brightening or dimming
          alternative=!alternative;
          mode = modeConfirm;
        }
        if (button[i].clicks == 5) {                    // party mode
          standbyActive = 0;
          modeSetParty();
        }
        if(LEDfunction[i] == -1) {                      // brightening all or dimming all with constant mode
        modeSetAll();
        LEDfunction[i] = 0;
        }
      }
    
    else {
          lightSensorRead();
          sensorState[i-2] = button[i].depressed;
          if ((sensorState[i-2] != lastSensorState[i-2]) && (sensorState[i-2] == (!sensorPull))) {     // triggering lights only when change of state and sensors state active
            if (!stateLight) modeSet(i);
            else lightSensorPrint(i);     
          }
          lastSensorState[i-2] = sensorState[i-2];
    }   
} 
  
  if (timeNow - timePrevSensor >= timeDiffSensor || timeNow < timePrevSensor) {
    timePrevSensor = timeNow;
    modeSetStandby();
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
  case modeBrighteningStandby:
      if (standbyActive && stateLight == 0) {
        brighteningStandby(); 
      }
      else {
        mode = modeOff;
      }
  break;
  case modeWait:
    waitForNextMode(); break;
  }
}



void waitForNextMode(){
  if (timeNow - timePrevMode >= waitDuration || timeNow < timePrevMode) {
    mode = nextMode;
  }
}

void postponeNextMode(int _nextMode) {
  mode = modeWait;
  timePrevMode = timeNow;
  nextMode = _nextMode;
}


void lightSensorRead() {
   //Input Hysteresis for fotoresistor light value
        int input = analogRead(lightSensor);

          if (stateLight) {
            if (input < darkThreshold)
            stateLight = false;
          }
          else  {
            if (input > lightThreshold) 
            stateLight = true;
          }          
}

void  off()  {                                         //turn all LEDs off
  
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++)
    if (outState[diode] > 0) {
      outState[diode] = 0;
      changed |= true;
    }
  return changed;
}
    


void confirm() {                                      //turn on for a moment last and first LED, to confirm the change alternative mode 
   bool changed = false;
   bool on = false;
     for (int diode = 1; diode < ledCount-1; diode++)
     if (outState[diode] > 0) on = true;
      if (on) {
        for (int diode = 0; diode < ledCount; diode++) {
        if (outState[diode] > 0) outState[diode] = 0;
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
      for (int diode = 0; diode<ledCount;diode++) outState[diode] = 0;
     }
    
   if (!changed) 
    mode =  modeBrighteningStandby;    
}


void modeSet(int button) {

  lightSensorPrint(button);

  if (mode == modeOff || mode == modeStandby || mode == modeDimmingFromBottomAlt || mode == modeDimmingFromBottom || mode == modeDimmingFromTopAlt || mode == modeDimmingFromTop || 
      mode == modeBrighteningFromBottomAlt || mode == modeBrighteningFromBottom || mode == modeBrighteningFromTopAlt || mode == modeBrighteningFromTop) {    
        
    if (button == downButton) {
      if (alternative) mode = modeBrighteningFromBottomAlt;
      else mode = modeBrighteningFromBottom;
      }

    if (button == upButton) {
      if (alternative) mode = modeBrighteningFromTopAlt;
      else mode = modeBrighteningFromTop;
      }
  }
  // sensor is not triggering at dimming from top cycle, when person is passing last sensor. 
    if ((button == downSensor) && (mode == modeOff || mode == modeStandby ||            
        mode == modeBrighteningFromBottomAlt || mode == modeBrighteningFromBottom || mode == modeDimmingFromBottom || mode == modeDimmingFromBottomAlt)) {
      if (alternative) mode = modeBrighteningFromBottomAlt;
      else mode = modeBrighteningFromBottom;
      }
  // sensor is not triggering at dimming from bottom cycle, when person is passing last sensor.
    if ((button == upSensor) && (mode == modeOff || mode == modeStandby ||              
        mode == modeBrighteningFromTopAlt || mode == modeBrighteningFromTop || mode == modeDimmingFromTop || mode == modeDimmingFromTopAlt)) {
      if (alternative) mode = modeBrighteningFromTopAlt;
      else mode = modeBrighteningFromTop;
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
      mode == modePartyRandOn || mode == modePartyRandOff || mode == modePartyLarsonScannerL || mode == modePartyLarsonScannerR || mode == modePartyWaveUp || mode == modePartyWaveDown || mode == modeWait)
      {
        mode = modeDimmingAll;
        Serial.println("All Off");
      }
}

#pragma region standby


bool canEnableStandby() {
  if (standbyActive
    && mode == modeOff)
  {
    return true;
  }
  return false;
}


//standbymode when is dark, and standbyactive
void modeSetStandby() {

  lightSensorRead();

  if (stateLight == false && canEnableStandby())
    mode = modeBrighteningStandby;
  else if (stateLight == true && mode == modeStandby)
    mode = modeOff;
   
}

// first and last two leds on at standbymode
void brighteningStandby() {
  bool changed = false;
  for (int diode = 2; diode < ledCount-2; diode++) {
    outState[diode] = 0;
    if ((outState[0] != standby) || (outState[ledCount-1] != standby)){
      outState[0] = standby;
      outState[ledCount-1] = standby;
      outState[1] = standby/8;
      outState[ledCount-2] = standby/8;
      changed = true;
    }  
  }
   if (!changed) mode = modeStandby;
}

#pragma endregion standby

#pragma region brightening

bool calculateBright(int diode, int step, int maxLvl = brightness){
  if (outState[diode] < maxLvl) {
    int newState = outState[diode] + (int)((double)brightness / step);
    outState[diode] = newState > maxLvl ? maxLvl : newState;
    return true;
  }
  return false;
}


void brighteningAll() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {
    changed |= calculateBright(diode, brighteningStepForAll);
  }
  if (!changed) mode = modeOn;
}


void brighteningFromBottom() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    changed = calculateBright(diode, brighteningStep);
    if (changed)
      break;
  }
  if (!changed)
    postponeNextMode(modeDimmingFromBottom);
}

void brighteningFromTop() {
  bool changed = false;
  for (int diode = ledCount-1; diode >= 0; diode--) {   
    changed = calculateBright(diode, brighteningStep);
    if (changed)
      break;
  }
  if (!changed)
    postponeNextMode(modeDimmingFromTop);
}

#pragma endregion brightening

#pragma region brighteningALT

void brighteningFromTopAlt() {
  bool changed = false;
  bool revolution = false;
  for (int diode = 1; diode < ledCount; diode++) {   
    if ((outState[diode] < brightness) && (outState[diode-1] > 0)) {
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
          if (newState < 0) newState = 0;
          outState[diode-1] = newState;
          changed = true;
          revolution = true;
          break;
    } 
 }  
 if (!revolution) {           //highly experimental code
    if (outState[0] == 0) {
      outState[0] = brightness;
      changed = true;
     revolutionSpeed++;
    }
    else changed = false;
    }
 if (!changed) {
  revolutionSpeed = 0;
  postponeNextMode(modeDimmingFromTopAlt);
 }
}

void brighteningFromBottomAlt() {
  bool changed = false;
  bool revolution = false;
  for (int diode = ledCount-2; diode >= 0; diode--) {   
    if ((outState[diode] < brightness) && (outState[diode+1] > 0)) {
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
          if (newState < 0) newState = 0;
          outState[diode+1] = newState;
      changed = true;
      revolution = true;
      break;
    } 
 }  
 if (!revolution) {                         //highly experimental code
    if (outState[ledCount-1] == 0) {
      outState[ledCount-1] = brightness;
      changed = true;
      revolutionSpeed++;
    }
    else changed = false;
    }
 if (!changed) {
  postponeNextMode(modeDimmingFromBottomAlt);
  revolutionSpeed = 0;
 }
}

#pragma endregion brighteningALT

#pragma region dimming

bool calculateDimm(int diode, int step){
  
  if (outState[diode] > 0) {
    int newState = outState[diode] - (int)((double)brightness / step);
    outState[diode] = newState < 0 ? 0 : newState;
    return true;
  }
  return false;
}

void dimmingAll() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {
    changed |= calculateDimm(diode, dimmingStepForAll);
  }
  if (!changed)
    mode = modeBrighteningStandby;
}

void dimmingFromBottom() {
  bool changed = false;
  for (int diode = 0; diode < ledCount; diode++) {   
    changed = calculateDimm(diode, dimmingStep);
    if (changed)
      break;
   }
   if (!changed)
    mode = modeBrighteningStandby; 
}

void dimmingFromTop() {
  bool changed = false;
  for (int diode = ledCount-1; diode >= 0; diode--) {
    changed = calculateDimm(diode, dimmingStep);
    if (changed)
      break;
  }
  if (!changed)
    mode = modeBrighteningStandby;
}

#pragma endregion dimming

#pragma region dimmingALT

void dimmingFromTopAlt() {
  bool changed = false;
  bool revolution = false;
  for (int diode = 1; diode < ledCount; diode++) {   
    if ((outState[diode] > 0) && (outState[diode-1] < brightness)) {
          int newState = outState[diode-1];
          newState = newState + dimmingAltStep;
          if (newState > brightness) newState = brightness;
          outState[diode-1] = newState;
          newState = outState[diode];
          newState = newState - dimmingAltStep;
          if (newState < 0) newState = 0;
          outState[diode] = newState;
      changed = true;
      revolution = true;
      break;
    } 
  }  
  if (!revolution) {
      if (outState[0] == brightness) {
      outState[0] = 0;
      changed = true;
    }
      else changed = false;
    }
  
   if (!changed)
    mode =  modeBrighteningStandby;  

}

void dimmingFromBottomAlt() {
  bool changed = false;
  bool revolution = false;
  for (int diode = ledCount-2; diode >= 0; diode--) {   
    if ((outState[diode] > 0) && (outState[diode+1] < brightness)) {
          int newState = outState[diode+1];
          newState = newState + dimmingAltStep;
          if (newState > brightness) newState = brightness;
          outState[diode+1] = newState;
          newState = outState[diode];
          newState = newState - dimmingAltStep;
          if (newState < 0) newState = 0;
          outState[diode] = newState;
          changed = true;
          revolution = true;
          break;
    } 
  }  
  if (!revolution) {
      if (outState[ledCount-1] == brightness) {
      outState[ledCount-1] = 0;
      changed = true;
    }
      else changed = false;
    }
   if (!changed) 
    mode =  modeBrighteningStandby;   
}

#pragma endregion dimmingALT
