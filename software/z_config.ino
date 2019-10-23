typedef struct                //configuration with list of settings to save in eeprom memory
{
    char version[6];
    int brightness;
    int dimmingStep;
    int dimmingAltStep;
    int dimmingStepForAll;
    int brighteningStep;
    int brighteningAltStep;
    int brighteningStepForAll;
    int partyStep;
    int partyloopRandMax;
    int partyloopBDMax;
    int partyloopScannerMax;
    int partyloopWaveMax;
    int partyWaveLength;
    int partyloopNukeMax;
    int partyloopSparkleMax;
    int partySparkleFreq;
    int standby;
    bool alternative;
    bool standbyActive;
    int lightThreshold;
    int darkThreshold;
    unsigned long waitDuration;
    
} settings;

#define config_version "ver01"
#define config_start 16

  settings cfg = {
      config_version,
      1024,                     //int brightness = 1024;            
      32,                       //int dimmingStep = 12;
      255,                      //int dimmingAltStep = 255;
      128,                      //int dimmingStepForAll = 8;
      32,                       //int brighteningStep = 96;
      100,                      //int brighteningAltStep = 100;
      128,                      //int brighteningStepForAll = 24;
      256,                      //int partyStep = 256;
      2,                        //int partyloopRandMax = 2;         number of randomOn and randomOff loops in partymode
      3,                        //int partyloopBDMax = 3;           number of brightening and dimming loops in partymode
      4,                        //int partyloopScannerMax = 4;      number of larsonScanner loops in partymode
      1024,                     //int partyloopWaveMax = 1024;      number of wave loops in partymode
      4,                        //int partyWaveLength = 4;          
      2,                        //int partyloopNukeMax = 2;         number of nuke loops in partymode
      1024,                     //int partyloopSparkleMax = 1024;   number of nuke loops in partymode
      8,                        //int partySparkleFreq = 8;
      128,                      //int standby = 4;                  brightness value for standby mode
      false,                    //int alternative = 0;              alternative mode for brightening and dimming mode
      false,                    //int standbyActive = 0;            for standby mode              
      700,                      //int lightThreshold = 700;         for hysteresis of light sensor
      300,                      //int darkThreshold  = 300;         for hysteresis of light sensor
      3000                      //unsigned long waitDuration = 3000; time between brightening and dimming
};


// load whats in EEPROM in to the local configuration if it is a valid setting
bool loadConfig() {

  // is it correct?
  if (EEPROM.read(config_start + 0) == config_version[0] &&
      EEPROM.read(config_start + 1) == config_version[1] &&
      EEPROM.read(config_start + 2) == config_version[2] &&
      EEPROM.read(config_start + 3) == config_version[3] &&
      EEPROM.read(config_start + 4) == config_version[4]){
 
  // load (overwrite) the local configuration struct

    for (unsigned int i=0; i<sizeof(cfg); i++){
      *((char*)&cfg + i) = EEPROM.read(config_start + i);
    }
    Serial.println("configuration loaded:");
    Serial.println(cfg.version);

    brightness = cfg.brightness;
    dimmingStep = cfg.dimmingStep;
    dimmingAltStep = cfg.dimmingAltStep;
    dimmingStepForAll = cfg.dimmingStepForAll;
    brighteningStep = cfg.brighteningStep;
    brighteningAltStep = cfg.brighteningAltStep;
    brighteningStepForAll = cfg.brighteningStepForAll;
    partyStep = cfg.partyStep;
    partyloopRandMax = cfg.partyloopRandMax;
    partyloopBDMax = cfg.partyloopBDMax;
    partyloopScannerMax = cfg.partyloopScannerMax;
    partyloopWaveMax = cfg.partyloopWaveMax;
    partyWaveLength = cfg.partyWaveLength;
    partyloopNukeMax = cfg.partyloopNukeMax;
    partyloopSparkleMax = cfg.partyloopSparkleMax;
    partySparkleFreq = cfg.partySparkleFreq;
    standby = cfg.standby;
    alternative = cfg.alternative;
    standbyActive = cfg.standbyActive;
    lightThreshold = cfg.lightThreshold;
    darkThreshold = cfg.darkThreshold;
    waitDuration = cfg.waitDuration;

    return true; // return 1 if config loaded 

  }
  return false; // return 0 if config NOT loaded

}


// save the configuration in to EEPROM

void saveConfig() {
  for (unsigned int i=0; i<sizeof(cfg); i++)
    EEPROM.write(config_start + i, *((char*)&cfg + i));
    Serial.println("configuration saved");
}