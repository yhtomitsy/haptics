// function for initializing the haptic drivers
void initDRV(){
    //for(uint8_t i = 3; i < 8; i++){
        //tcaselect(i);
        delay(100);
        while(!DRVInit())Serial.print(".");           //HMD.begin();
        setMode(0x02); //HMD.Mode(0x02);              // Internal trigger input mode -- Must use the GO() function to trigger playback.
        if(MOTOR == 6)setFeedbackRegister(0xB6);      //HMD.MotorSelect(0xB6); // 0xB9 // LRA motor, 4x Braking, Medium loop gain, 7.5x back EMF gain
        else setFeedbackRegister(0x36);               //HMD.MotorSelect(0x36); // 0x36 // ERM motor, 4x Braking, Medium loop gain, 1.365x back EMF gain
        setLibrary(MOTOR);                            //HMD.Library(MOTOR);   // 1-5 & 7 for ERM motors, 6 for LRA motors 
    //}
//    Serial.println("Initialization complete...");
}

// function for runing motors according to received input trigger
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
                //Serial.println(i);
                digitalWrite(IN_Pins[i], 0);
                //tcaselect(i + 3);
                if(inWave > 0 && inWave != wave[i]){
//                    Serial.print("wave effect changed for finger: ");Serial.println(i+1);
                    setWaveform(0, inWave);
                    setWaveform(1, 0);
                    wave[i] = inWave;
                }
                if(inWave != 0)digitalWrite(IN_Pins[i], state[i]);
            }
            else if(inWave == 0){
               //digitalWrite(IN_Pins[i], 0);
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

// function for play back if required. it plays back the effect after every 50ms
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

// function for the menu button
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
