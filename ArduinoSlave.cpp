#include "ArduinoSlave.h"
//#include <Arduino.h>



void Slave::Conf()
{
   Serial.begin(9600);
	delay(2000);//wait arduino On
	Serial.print("7.2."); Serial.print(SlavePin2,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.3."); Serial.print(SlavePin3,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.4."); Serial.print(SlavePin4,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.5."); Serial.print(SlavePin5,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.6."); Serial.print(SlavePin6,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.7."); Serial.print(SlavePin7,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.8."); Serial.print(SlavePin8,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.9."); Serial.print(SlavePin9,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.10."); Serial.print(SlavePin10,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.11."); Serial.print(SlavePin11,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.12."); Serial.print(SlavePin12,DEC); Serial.println("."); 
	delay(10);
	Serial.print("7.13."); Serial.print(SlavePin13,DEC); Serial.println("."); 
	delay(10);
    Serial.println("9.0.0."); //end pinConf
	delay(1000); //wait arduino pi conf
}


//=======================================================
void Slave::Comm() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar != '.') {
      inputString += inChar;
      }
    if (inChar == '.') {
		inputString += '\n';//
		      //inputString += '\n';//
     // inputInt[indeX]=inputString.toInt();
      inputInt[indeX]=atoi(inputString);

      inputString = "\n";
      indeX++;
      }
    if (indeX == 3) {
      indeX=0;
      stringComplete = true;
    }
  }//while
  if (stringComplete) {
    switch(inputInt[0]){
      case 2:             //digital read
        if(1<inputInt[1]<14){
          dV[inputInt[1]]=inputInt[2];}
      break;
      case 4:             //analog read
        if(0<=inputInt[1]<8){
          aV[inputInt[1]]=inputInt[2];}
      break;
///* if no need
      case 6:           //get nVar
        nVar[inputInt[1]]=inputInt[2];    
      break;
//*/
/*      case 8:
        if (1<inputInt[1]<14){//read arduino pinConf
          pinConf[inputInt[1]]=inputInt[2];    
        }
      break;*/
      default:
      break;
    }//switch
    inputInt[0]=0;
    inputInt[1]=0;
    inputInt[2]=0;
    stringComplete = false;
  }//if stringcomplete
}  

void Slave::dW(uint8 pin, bool val){
	if(val!=lastDWVal[pin]){
	Serial.print("1.");
	Serial.print(pin);Serial.print(".");
	Serial.print(val);Serial.println(".");
	lastDWVal[pin]=val;
	}
}

void Slave::dR(uint8 pin){//digital read
	Serial.print("2.");
	Serial.print(pin);Serial.print(".0.");
}

void Slave::aW(uint8 pin, uint16_t aWVal){//analog write
	if(val!=lastAWVal[pin]){
	Serial.print("3.");
	Serial.print(pin);Serial.print(".");
	Serial.print(aWVal);Serial.println(".");
	lastAWVal[pin]=aWVal;
	}
}

void Slave::aR(uint8 pin){
	Serial.print("4.");
	Serial.print(pin);Serial.println(".0.");
}

void Slave::sV(uint8 nVar, uint16_t nVarV){ //send variavle, store variable
	Serial.print("5.");
	Serial.print(nVar);Serial.print(".");
	Serial.print(nVarV);Serial.println(".");
}

void Slave::gV(uint8 nVar){
	Serial.print("6.");
	Serial.print(nVar);Serial.println(".0.");
}
