
void wave_test(){
    for(finger = 1; finger <= 123; finger++){  // scroll through effects
        if(Serial.available()){
            finger = 0;
            duration = 0;
            int buff = Serial.available();
            for(uint8_t k = 0; k < buff; k++){
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
            if(finger < 0)finger = 0;
            if(finger > 123)finger = 123;
        }
        
        
        
        if(finger % 8 == 0) sequence = int(finger / 8) - 1;
        else sequence = int(finger / 8);
        if(finger > 64)sequence -= 8;
        
        HMD.Waveform(sequence, finger);
        Serial.print("Sequence:");Serial.print(sequence);Serial.print("  ");
        Serial.print("wave:");Serial.println(finger);
        
        
        for(int j = 40; j <= 320; j+=20){
            Serial.println(j);
            HMD.go();
            delay(j);
            HMD.stop();
            delay(1000);
        }
        
    }
}

