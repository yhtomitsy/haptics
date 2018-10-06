#include <Sparkfun_DRV2605L.h> //SparkFun Haptic Motor Driver Library 
#include <Wire.h> //I2C library 

#define TCAADDR 0x70
#define touchButton 2
#define MOTOR 2          // 1-5 & 7 for ERM motors, 6 for LRA motors

//I2C address
#define I2C_ADDR 0x5A

//The Status Register (0x00): The Device ID is bits 7-5. For DRV2605L it should be 7 or 111. 
//bits 4 and 2 are reserved. Bit 3 is the diagnostic result. You want to see 0. 
//bit 1 is the over temp flag, you want this to be 0
//bit 0 is  over current flag, you want this to be zero. 
// Ideally the register will read 0xE0.
#define STATUS_REG 0x00 

//The Mode Register (0x01): 
//Default 010000000 -- Need to get it out of Standby
//Set to 0000 0000=0x00 to use Internal Trigger
//Set to 0000 0001=0x01 to use External Trigger (edge mode)(like a switch on the IN pin)
//Set to 0000 0010=0x02 to use External Trigger (level mode)
//Set to 0000 0011=0x03 to use PWM input and analog output
//Set to 0000 0100=0x04 to use Audio to Vibe 
//Set to 0000 0101=0x05 to use Real-Time Playback
//Set to 0000 0110=0x06 to perform a diagnostic test - result stored in Diagnostic bit in register 0x00
//Set to 0000 0111 =0x07 to run auto calibration 
#define MODE_REG 0x01

//The Feedback Control Register (0x1A)
//bit 7: 0 for ERM, 1 for LRA -- Default is 0
//Bits 6-4 control brake factor
//bits 3-2 control the Loop gain
//bit 1-0 control the BEMF gain
#define FEEDBACK_REG 0x1A

//The Real-Time Playback Register (0x02)
//There are 6 ERM libraries. 
#define RTP_REG 0x02

//The Library Selection Register (0x03)
//See table 1 in Data Sheet for 
#define LIB_REG 0x03

//The waveform Sequencer Register (0X04 to 0x0B)
#define WAVESEQ1 0x04 //Bit 7: set this include a wait time between playback                                                                                                                                                                                 
#define WAVESEQ2 0x05
#define WAVESEQ3 0x06
#define WAVESEQ4 0x07
#define WAVESEQ5 0x08
#define WAVESEQ6 0x09
#define WAVESEQ7 0x0A
#define WAVESEQ8 0x0B

//The Go register (0x0C)
//Set to 0000 0001=0x01 to set the go bit
#define GO_REG 0x0C

//The Overdrive Time Offset Register (0x0D)
//Only useful in open loop mode
#define OVERDRIVE_REG 0x0D

//The Sustain Time Offset, Positive Register (0x0E)
#define SUSTAINOFFSETPOS_REG 0x0E

//The Sustain Time Offset, Negative Register (0x0F)
#define SUSTAINOFFSETNEG_REG 0x0F

//The Break Time Offset Register (0x10)
#define BREAKTIME_REG 0x10

//The Audio to Vibe control Register (0x11)
#define AUDIOCTRL_REG 0x11

//The Audio to vibe minimum input level Register (0x12)
#define AUDMINLVL_REG 0x12

//The Audio to Vibe maximum input level Register (0x13)
#define AUDMAXLVL_REG 0x13

// Audio to Vibe minimum output Drive Register (0x14)
#define AUDMINDRIVE_REG 0x14

//Audio to Vibe maximum output Drive Register (0x15)
#define AUDMAXDRIVE_REG 0X15

//The rated Voltage Register (0x16)
#define RATEDVOLT_REG 0x16

//The Overdive clamp Voltage (0x17)
#define OVERDRIVECLAMP_REG 0x17

//The Auto-Calibration Compensation - Result Register (0x18)
#define COMPRESULT_REG 0x18

//The Auto-Calibration Back-EMF Result Register (0x19)
#define BACKEMF_REG 0x19

//The Control1 Register (0x1B)
//For AC coupling analog inputs and 
//Controlling Drive time 
#define CONTROL1_REG 0x1B

//The Control2 Register (0x1C)
//See Data Sheet page 45
#define CONTROL2_REG 0x1C

//The COntrol3 Register (0x1D)
//See data sheet page 48
#define CONTROL3_REG 0x1D

//The Control4 Register (0x1E)
//See Data sheet page 49
#define CONTROL4_REG 0x1E

//The Control5 Register (0x1F)
//See Data Sheet page 50
#define CONTROL5_REG 0X1F

//The LRA Open Loop Period Register (0x20)
//This register sets the period to be used for driving an LRA when 
//Open Loop mode is selected: see data sheet page 50.
#define OLP_REG 0x20

//The V(Batt) Voltage Monitor Register (0x21)
//This bit provides a real-time reading of the supply voltage 
//at the VDD pin. The Device must be actively sending a waveform to take 
//reading Vdd=Vbatt[7:0]*5.6V/255
#define VBATMONITOR_REG 0x21

//The LRA Resonance-Period Register 
//This bit reports the measurement of the LRA resonance period
#define LRARESPERIOD_REG 0x22

SFE_HMD_DRV2605L HMD; //Create haptic motor driver object 

int EN_Pins[] = {35, 14, 16, 20, 22};
int IN_Pins[] = {36, 15, 17, 21, 23};

byte state[5] = {0};          // state of enable/trigger pin
int inWave = 0;               // incoming wave effect number
int wave[5] = {0};            // previous wave effect for each finger
uint32_t timeTrack = 0;       // tracking for haptic effect
boolean menuOpened = false;   // menu state
uint8_t data_index = 0;       // determines data to parse
int duration = 0;             // duration of effect

// Incoming finger values
// 0 - none
// 1 - thumb
// 2 - index finger
// 4 - middle finger
// 8 - ring finger
// 16 - pinky finger
// to get multiple fingers, add their values. 
uint8_t finger = 0;           // incoming finger value

void tcaselect(uint8_t i) {
    if (i > 7) return;
    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}

void setup(){
    Serial.begin(2000000);
    Wire.begin();
    Wire.setClock(400000); //Increase I2C data rate to 400kHz
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
    initDRV();
}

void loop(){
//    wave[0] = 0;
    menuButton();
    listenForFeedback();
    //playBack();
    //delay(10);
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
//        Serial.print("Finger: "); Serial.println(finger);
//        Serial.print("inwave: ");Serial.println(inWave);
//        Serial.print("wave: ");Serial.println(inWave);
//        Serial.print("Duration: ");Serial.println(duration);
      
        for (byte i = 0; i < 5; i++) {
            state[i] = bitRead(finger, i);
            if(state[i]){
                digitalWrite(IN_Pins[i], 0);
                tcaselect(i + 3);
                if(inWave > 0 && inWave != wave[i]){
//                    Serial.print("wave effect changed for finger: ");Serial.println(i+1);
                    setWaveform(0, inWave);
                    setWaveform(1, 0);
                    wave[i] = inWave;
                }
                digitalWrite(IN_Pins[i], state[i]);
            }
//            Serial.print(i);Serial.print(": ");Serial.println(state[i]);
        }
        //Serial.println();
        timeTrack = millis();
        data_index = 0;
        finger = 0;
        inWave = 0;
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
