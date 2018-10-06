#include <Sparkfun_DRV2605L.h> //SparkFun Haptic Motor Driver Library 
#include <Wire.h> //I2C library 

SFE_HMD_DRV2605L HMD; //Create haptic motor driver object 

#define MOTOR 6          // 1-5 & 7 for ERM motors, 6 for LRA motors
#define MODE 0x02
#define NUMSENSE 1


int trig_Pin[NUMSENSE] = {2};//, 4};
int en_Pin[NUMSENSE] =  {3};//, 5};
byte state[NUMSENSE] = {0};
int wave[NUMSENSE] = {0};

uint32_t timeTrack = 0;
boolean triggered = false;
uint8_t inWave = 0;
int sequence = 0;
uint8_t finger = 0;
uint8_t data_index = 0;
int duration = 0;


void setup() 
{
    for(uint8_t i = 0; i < NUMSENSE; i++){
        pinMode(en_Pin[i], OUTPUT);
        pinMode(trig_Pin[i], OUTPUT);  
    }
    for(uint8_t i = 0; i < NUMSENSE; i++){
        digitalWrite(trig_Pin[i], LOW);
        digitalWrite(en_Pin[i], HIGH);
    }
    
    Serial.begin(9600);
    Wire.begin();
    Wire.setClock(400000); //Increase I2C data rate to 400kHz
    initHaptic();
    
    for(uint8_t i = 0; i < NUMSENSE; i++){
        digitalWrite(en_Pin[i], LOW);
    }
}

void loop() 
{   
//  uint32_t t = millis(); 
  if(Serial.available()){
      duration = 0;
//      for(uint8_t i = 0; i < 2; i++){
//          digitalWrite(en_Pin[i], HIGH);
//      }
      int buff = Serial.available();
      for(uint8_t i = 0; i < buff; i++){
          char c = (char)Serial.read();
          if(c == '\r')
          {
             // do nothing 
          }
          else if(c == '\n')
          {
              break;
          }
          else if(c == ';')
          {
              data_index++;
          }
          else{
              switch(data_index)
              {
                  case 0:
                      finger = (finger * 10) +  c - '0';
                      break;
                  case 1: 
                      inWave = (inWave * 10) + c - '0'; 
                      break;
                  case 2: 
                      duration = (duration * 10) + c - '0';
                      break;
                  default:
                      break;
              }
              
          }
                     
      }
     
      Serial.print("Finger: "); Serial.println(finger);
//      Serial.print("sequence: ");Serial.println(sequence);
      Serial.print("wave: ");Serial.println(inWave);
      Serial.print("Duration: ");Serial.println(duration);
      for (byte i = 0; i < NUMSENSE; i++) 
      {
            state[i] = bitRead(finger, i);
            if(state[i] && inWave > 0 && inWave != wave[i])
            {
                Serial.println("flag");
                digitalWrite(en_Pin[i], HIGH);
                digitalWrite(trig_Pin[i], LOW);
                setWaveform(0, inWave);
                setWaveform(1, 0);
                digitalWrite(trig_Pin[i], HIGH);
                wave[i] = inWave;
//                triggered =  true; 
            }
      }
      
      data_index = 0;
      finger = 0;
      inWave = 0;
      timeTrack = millis();
  } 
  
  if(millis() - timeTrack >= 1500){
      for(uint8_t i = 0; i < NUMSENSE; i++){
          digitalWrite(trig_Pin[i], LOW);
          digitalWrite(en_Pin[i], LOW);
      }
//      HMD.stop();
//      Serial.println(duration);
      triggered = false;
  }
  
//  wave_test();
//  Serial.println(1000 / (millis() - t));
}


void initHaptic()
{
//    while(!DRVInit()){
//      Serial.print(".");
//      delay(500);
//    }
    DRVInit();
    setMode(0x02);
    if(MOTOR == 6)setFeedbackRegister(0xB6); // 0xB6 // LRA motor, 4x Braking, Medium loop gain, 7.5x back EMF gain
    else setFeedbackRegister(0x36);          // 0x36 // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
    setLibrary(MOTOR);

}
  
