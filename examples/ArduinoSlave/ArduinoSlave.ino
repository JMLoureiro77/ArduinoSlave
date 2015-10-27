/******************************************************************** 
	Arduino Slave
   
	by jmloureiro77 & Juan Pinto

	flash 5496 bytes (17% 328P)
	RAM    271 bytes (13% 328P)

Recive char y convierte en tres int[3]=  int[0].int[1].int2[2].
  *tb hay que mandar el último punto


Esp send:                  int[0]  int[1]  int[2]
                    
digital write         1     pin       value
digital read          2     pin       0(else)
analog write          3     pin       value
analog read           4     pin       0(else)
store variable        5     nVar      value
get  variable         6     nvar      0(else)
configure pin         7     pin       confVal
send pinConf          8     0         0(else)
clear pinConf         8     1         0(else)
end pinConf           9     0(else)   0(esle) 

confVal
  1   digital in
  2   digital out start low
  3   digital out start high
  4   analog out (PWM)
  5   digital input_pullup

uso con IDE serial monitor 
upload the code
8.0.0.    clear arduino pinConf
7.13.2.   configura el pin 13 como salida digital start low
9.0.0.    guarda conf en eeprom y resetea arduino
1.13.1.   enciende el led de pin 13
1.13.0.   apaga el led del pin 13
5.0.1380. guarda 1380 en nVar[0]
6.0.0.    devuelve el valor de nVar[0] "6.0.1380."

Integración con SS
Un ESP con el codigo de ss y sin declarar usart

*********************************************************************/
byte pinConf[14];//0,1 serial(not used)
                //D2-D13
String inputString= "";
int inputInt[3];
bool stringComplete = false;
byte index=0;
bool needReset=0;
///*
int nVar[10];
//*/
bool a=1;
//===============================================================
void setup() {
  Serial.begin(9600);

//pin conf
while (a){
    serialEvent();
    for (int i=2;i<14;i++) {
      if(pinConf[i]==1){        //digital in
        pinMode(i,INPUT);}
      if(pinConf[i]==2){        //digital out start low
        digitalWrite(i,LOW);
        pinMode(i,OUTPUT);}
      if(pinConf[i]==3){        //digital out start high
        digitalWrite(i,HIGH);
        pinMode(i,OUTPUT);}
      if(pinConf[i]==4&&i==(3||5||6||9||10||11)){//analog out
        pinMode(i,OUTPUT);}
      if(pinConf[i]==5&&i){     //digital input_pullup
        pinMode(i,INPUT_PULLUP);}
    }//for
  }//while
}

//===============================================================
void loop() {

  serialEvent();
   
  if(millis()%15000==0){
    int a=1;}//No quitar, sin no funciona
/*  for (int i=2;i<21;i++) {//serial monitor
      Serial.print(i);
      Serial.print(": ");
      Serial.println(pinConf[i]);
    }
  }*/
}


//===============================================================
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar != '.') {
      inputString += inChar;
      }
    if (inChar == '.') {
      inputInt[index]=inputString.toInt();
      inputString = "";
      index++;
      }
    if (index == 3) {
      index=0;
      stringComplete = true;
    }
  }//while
  if (stringComplete) {
    switch(inputInt[0]){
      case 1:             //digital write
        if(1<inputInt[1]<14&&(inputInt[2]==0||inputInt[2]==1)){
          digitalWrite(inputInt[1],inputInt[2]);}
      break;
      case 2:             //digital read
        if(1<inputInt[1]<14){
          Serial.print("2.");
          Serial.print(inputInt[1]);
          Serial.print(".");
          Serial.print(digitalRead(inputInt[1]));
          Serial.println(".");}
      break;
      case 3:             //analog write
        if((inputInt[1]==(3||5||6||9||10||11))&&(0<=inputInt[2]<1024)){
          analogWrite(inputInt[1],inputInt[2]);}
      break;
      case 4:             //analog read
        if(13<inputInt[1]<22){
          Serial.print("4.");
          Serial.print(inputInt[1]);
          Serial.print(".");
          Serial.print(analogRead(inputInt[1]));
          Serial.println(".");}
      break;
///* if no need
      case 5:           //store nVar
        nVar[inputInt[1]]=inputInt[2];
      break;
      case 6:           //send nVar
        Serial.print("6.");
        Serial.print(inputInt[1]);
        Serial.print(".");           
        Serial.print(nVar[inputInt[1]]);
        Serial.println(".");           
      break;
//*/
      case 7://configure pin
        if((1<inputInt[1]<14)&&(0<=inputInt[2]<6)){
          pinConf[inputInt[1]]=inputInt[2];
          needReset=1;}
      break;
      case 8:
       if (inputInt[1]==0){//send pinConf
          for (int i=2;i<14;i++) {
            Serial.print("8.");
            Serial.print(i);
            Serial.print(".");           
            Serial.print(pinConf[i]);
            Serial.println(".");           
          }
        }
        if(inputInt[1]==1){//clear pinConf
          for (int i=2;i<14;i++) {
            pinConf[i]=0;
          }
        }
      break;
      case 9://en pincof
         a=0;
      break;
      default:
      break;
    }//switch
    inputInt[0]=0;
    inputInt[1]=0;
    inputInt[2]=0;
    stringComplete = false;
  }//if stringcomplete
}
