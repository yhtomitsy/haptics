bool DRVInit() 
{
    //Get a read from the status register
    //Want this to Read 0xE0, any other value than 0 and you have tripped the over-current protection=wrong motor
    uint8_t status = readDRV2605L(STATUS_REG);
    Serial.print("Status Register 0x"); 
    Serial.println(status, HEX);
    if(status == 0xE0)return true;
    else return false;
    
}
 //Select Mode
 //Write 0x00 to get out of standby and use internal trigger (using GO command)
 //Write 0x01 to get out of standby + use External Trigger (edge triggered)
 //Write 0x02 to get out of standby + External Trigger (level triggered)
 //Write 0x03 to get out of standby + PWM input and analog output
 //Write 0x04 to use Audio to Vibe
 //Write 0x05 to use real time playback
 //Write 0x06 to perform a diagnostic - result stored in diagnostic bit in register 0x00
 //Write 0x07 to run auto calibration
 
void setMode(uint8_t mod)
{
    writeDRV2605L(MODE_REG, mod); 
}

//Select ERM or LRA 
//Set Motor Type Using the Feedback Control Register
//Set ERM or LRA
//Example: 0X39 0011 1001: ERM Mode, 4x Brake factor, Medium Gain, 1.365x Back EMF
//Example: 0xB9 1011 1001: LRA MODE, 4X Brake Factor, Medium Gain, 7.5x Back EMF 
void setFeedbackRegister(uint8_t val)
{
    writeDRV2605L(FEEDBACK_REG, val);
    
    //uint8_t motor = readDRV2605L(FEEDBACK_REG);
    //Serial.print("Feedback Register/Motor Selector 0x"); Serial.println(motor, HEX);
}

//Select Library
void setLibrary(uint8_t lib)
{
  writeDRV2605L(LIB_REG, lib);
}

//Select waveform from list of waveform library effects 
//data sheet page 60. This function selects the sequencer
//and the effects from the library. 
void setWaveform(uint8_t seq, uint8_t wav)
{
    writeDRV2605L(WAVESEQ1+seq, wav);
}

 //Go command
void setGoBit() 
{
  writeDRV2605L(GO_REG, 1);
}

//Stop Command
void clearGoBit() 
{
  writeDRV2605L(GO_REG, 0); //write 0 back to go bit to diable internal trigger
}

//Read function
uint8_t readDRV2605L(uint8_t reg) 
{
    uint8_t var ;
    // Enable I2C Read
    Wire.beginTransmission(I2C_ADDR);
    Wire.write((byte)reg);
    Wire.endTransmission();
    Wire.requestFrom((byte)I2C_ADDR, (byte)1);
    var = Wire.read();
    return var;
}

//Write Function 
void writeDRV2605L(uint8_t where, uint8_t what)
{
   // Enable I2C write
    Wire.beginTransmission(I2C_ADDR);
    Wire.write((byte)where);
    Wire.write((byte)what);
    Wire.endTransmission();
}
