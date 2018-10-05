#include <Sparkfun_DRV2605L.h> //SparkFun Haptic Motor Driver Library 
#include <Wire.h> //I2C library 

#define TCAADDR 0x70
#define touchButton 2

SFE_HMD_DRV2605L HMD; //Create haptic motor driver object 

int EN_Pins[] = {35, 14, 16, 20, 22};
int IN_Pins[] = {36, 15, 17, 21, 23};
byte state[5] = {0};
int wave[2] = {0};
int sequence = 0;
int fingerNum = 0;
uint32_t timeTrack = 0;
boolean menuOpened = false;

void tcaselect(uint8_t i) {
    if (i > 7) return;
    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}

void setup(){
    Serial.begin(2000000);
    Wire.begin();

    pinMode(touchButton, INPUT);
    
    for(uint8_t i = 0; i < 5; i++){
        pinMode(EN_Pins[i], OUTPUT);
        pinMode(IN_Pins[i], OUTPUT);  
    }
    for(uint8_t i = 0; i < 5; i++){
        digitalWrite(IN_Pins[i], LOW);
        digitalWrite(EN_Pins[i], HIGH);
    }
    
    
    delay(1000);
    for(uint8_t i = 3; i < 8; i++){
        tcaselect(i);
        delay(100);
        HMD.begin();
        HMD.Mode(0x02);        // Internal trigger input mode -- Must use the GO() function to trigger playback.
        HMD.MotorSelect(0x36); // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
        HMD.Library(2);        //1-5 & 7 for ERM motors, 6 for LRA motors 
        HMD.Waveform(1, 13);
    }
    
}

void loop(){
    wave[0] = 0;
    menuButton();
    listenForFeedback();
    playBack();
    delay(10);
 }

 void menuButton(){
    if(digitalRead(touchButton) && !menuOpened){
        Serial.println("1");  
        menuOpened = true;
    }
    else if(!digitalRead(touchButton) && menuOpened){
        menuOpened =  false;
        Serial.println("0");
    }
 }

void listenForFeedback(){
    if(Serial.available()){
        char c = (char)Serial.read();
        fingerNum =  c - '0'; // Get num from somewhere
        if(Serial.available()){
            c = (char)Serial.read();
            if(c != ';' && c != '\r')fingerNum = (fingerNum * 10) + c - '0'; // Get num from somewhere
        }
        int buff = Serial.available();
        for(uint8_t i = 0; i < buff; i++){
            c = (char)Serial.read();
            if(c != ';' && c != '\r'){
                wave[0] = (wave[0] * 10) + c - '0'; // Get num from somewhere
            }
            if(c == '\r') 
            if(c == '\n')break;           
        }
        //Serial.print("wave: ");Serial.println(wave[0]);
        for (byte i = 0; i < 5; i++) {
            state[i] = bitRead(fingerNum, i);
            if(state[i]){
                digitalWrite(IN_Pins[i], 0);
                tcaselect(i + 3);
                delay(10);
                wave[1] = wave[0];
                if(wave[1] > 64)sequence = int((wave[1] - 64) / 8);
                else sequence = int(wave[1] / 8);
                if(wave[1] > 0)HMD.Waveform(sequence, wave[1]);
            }
            digitalWrite(IN_Pins[i], state[i]);
            //Serial.print(i);Serial.print(": ");Serial.println(state[i]);
        }
        //Serial.println();
        timeTrack = millis();
    }
}

void playBack(){
    if(millis() - timeTrack > 50){
        for (byte i = 0; i < 5; i++) {
            if(state[i]){
                digitalWrite(IN_Pins[i], 0);
                delay(1);
                digitalWrite(IN_Pins[i], state[i]);
                //Serial.print(i);Serial.print(": ");Serial.println(state[i]);
            }
        }
        timeTrack = millis();
    }
}

 /*int seq = 0; //There are 8 sequence registers that can queue up to 8 waveforms
    for(int wave = 1; wave <=123; wave++) //There are 123 waveform effects 
    {
       HMD.Waveform(seq, wave);
       HMD.go();
       delay(600); //give enough time to play effect 
       Serial.print("Waveform Sequence:      ");
       Serial.print(seq);
       Serial.print("Effect No.:      ");
       Serial.println(wave);
  
      if (wave%8==0) //Each Waveform register can queue 8 effects
      {
          seq=seq+1;
      }
      if (wave%64==0) // After the last register is used start over 
      {
          seq=0;
      }
    }*/
