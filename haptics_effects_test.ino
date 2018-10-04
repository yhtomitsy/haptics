#include <Sparkfun_DRV2605L.h> //SparkFun Haptic Motor Driver Library 
#include <Wire.h> //I2C library 

SFE_HMD_DRV2605L HMD; //Create haptic motor driver object 


#define trig_Pin 2
#define waitTime 50
#define I2C_ADDR 0x5A

uint32_t timeTrack = 0;
boolean triggered = false;
int wave[2] = {0};
int sequence = 0;
uint8_t finger = 0;
uint8_t data_index = 0;
int duration = 0;



void setup() 
{
  pinMode(trig_Pin, OUTPUT);
  digitalWrite(trig_Pin, LOW);
  
  Serial.begin(9600);
  Wire.begin();
  Wire.setClock(400000); //Increase I2C data rate to 400kHz
  
  HMD.begin();
  HMD.Mode(0);           // Internal trigger input mode -- Must use the GO() function to trigger playback.
  HMD.MotorSelect(0xB9); //0x36 // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
  HMD.Library(6);        //1-5 & 7 for ERM motors, 6 for LRA motors 

}
void loop() 
{
  //uint32_t t = millis(); 
  if(Serial.available()){
      duration = 0;
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
                      wave[0] = (wave[0] * 10) + c - '0'; 
                      break;
                  case 2: 
                      duration = (duration * 10) + c - '0';
                      break;
                  default:
                      break;
              }
              
          }
                     
      }
     
      if(wave[0] > 64)sequence = int((wave[0] - 64) / 8);
      else sequence = int(wave[0] / 8);

      Serial.print("Finger: "); Serial.println(finger);
      Serial.print("sequence: ");Serial.println(sequence);
      Serial.print("wave: ");Serial.println(wave[0]);
      Serial.print("Duration: ");Serial.println(duration);
      
      if(wave[0] > 0){
          HMD.Waveform(sequence, wave[0]);
          HMD.go();
          timeTrack = millis();
          triggered = true;
      }   
      
      data_index = 0;
      finger = 0;
      wave[0] = 0;
  } 
  if(millis() - timeTrack >= duration && triggered){
      HMD.stop();
      //Serial.println(duration);
      triggered = false;
  }
  //Serial.println(1000 / (millis() - t));
}

 
  
