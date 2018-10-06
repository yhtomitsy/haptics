// function for initializing the haptic drivers
void initDRV(){
    for(uint8_t i = 3; i < 8; i++){
        tcaselect(i);
        delay(100);
        while(!DRVInit())Serial.print(".");           //HMD.begin();
        setMode(0x02); //HMD.Mode(0x02);              // Internal trigger input mode -- Must use the GO() function to trigger playback.
        if(MOTOR == 6)setFeedbackRegister(0xB6);      //HMD.MotorSelect(0xB6); // 0xB9 // LRA motor, 4x Braking, Medium loop gain, 7.5x back EMF gain
        else setFeedbackRegister(0x36);               //HMD.MotorSelect(0x36); // 0x36 // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
        setLibrary(MOTOR);                            //HMD.Library(MOTOR);   // 1-5 & 7 for ERM motors, 6 for LRA motors 
    }
    Serial.println("Initialization complete...");
}

