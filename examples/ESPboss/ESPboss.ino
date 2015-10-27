/****************************************************
      Sketch by jmloureiro77 and Juan Pinto
              
          ESP boss & Arduino Slave
*****************************************************/
// digital_in
// digital_out //start low
// digital_out_start_high
// analog_out //(PWM)
// digital_input_pullup

#define SlavePin2 digital_out
#define SlavePin3 digital_out
#define SlavePin4 digital_out
#define SlavePin5 digital_out

#include "ArduinoSlave.h"

Slave Slave;

void setup(){
    Slave.Conf();
}
void loop(){  
    Slave.Comm();
        
    Slave.dW(2,!Slave.dV[2]);
    Slave.dW(3,!Slave.dV[3]);
    Slave.dW(4,!Slave.dV[4]);
    Slave.dW(5,!Slave.dV[5]);

    If (millis()%1000==0){
        Slave.aR(0);
        float d =Slave.aV[0]
    }
}
