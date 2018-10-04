#include <Sparkfun_DRV2605L.h> //SparkFun Haptic Motor Driver Library 
#include <Wire.h> //I2C library 

SFE_HMD_DRV2605L HMD; //Create haptic motor driver object 
int wave[2] = {0};
int sequence = 0;
#define trig_Pin 2
#define waitTime 50

uint32_t timeTrack = 0;
boolean triggered = false;

void setup() 
{
  pinMode(trig_Pin, OUTPUT);
  digitalWrite(trig_Pin, LOW);
  
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000); //Increase I2C data rate to 400kHz
  
  HMD.begin();
  HMD.Mode(0); // Internal trigger input mode -- Must use the GO() function to trigger playback.
  HMD.MotorSelect(0x36); // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
  HMD.Library(2);       //1-5 & 7 for ERM motors, 6 for LRA motors 

}
void loop() 
{
  uint32_t t = millis();
  wave[0] = 0;
  if(Serial.available()){
      int buff = Serial.available();
      for(uint8_t i = 0; i < buff; i++){
          char c = (char)Serial.read();
          if(c != ';' && c != '\r' && c != '\n'){
              wave[0] = (wave[0] * 10) + c - '0'; // Get num from somewhere
          }
          if(c == '\r') 
          if(c == '\n')break;           
      }
      Serial.print("wave: ");Serial.println(wave[0]);
      if(wave[0] > 64)sequence = int((wave[0] - 64) / 8);
      else sequence = int(wave[0] / 8);
      Serial.print("sequence: ");Serial.println(sequence);
      if(wave[0] > 0){
        HMD.Waveform(sequence, wave[0]);
        HMD.go();
        timeTrack = millis();
        triggered = true;
      }
  } 
  if(millis() - timeTrack >= waitTime && triggered){
      HMD.stop();
      //timeTrack = millis();
      //Serial.println("kamaliza");
      triggered = false;
  }
  Serial.println(1000 / (millis() - t));
}

 
  
