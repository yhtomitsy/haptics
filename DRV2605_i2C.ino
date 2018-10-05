bool DRVInit() 
{
    //Get a read from the status register
    //Want this to Read 0xE0, any other value than 0 and you have tripped the over-current protection=wrong motor
    uint8_t status = readDRV2605L_(STATUS_REG);
    
    Serial.print("Status Register 0x"); 
    Serial.println(status, HEX);
    return true;
}

//Read function
uint8_t readDRV2605L_(uint8_t reg) 
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
void writeDRV2605L_(uint8_t where, uint8_t what)
{
   // Enable I2C write
    Wire.beginTransmission(I2C_ADDR);
    Wire.write((byte)where);
    Wire.write((byte)what);
    Wire.endTransmission();
}
