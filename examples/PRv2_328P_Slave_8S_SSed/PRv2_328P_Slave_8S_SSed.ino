/**************************************************************************************************  
Por jmloureiro77.blogspot.com
CC-BY 3.0

Programador de riego V2 328P SS ED:**
  8 sectores
  4 arranques diarios por sector
  tiempo mínimo de riego 1 minuto, máximo 99 minutos
  control manual
  puesta en hora de reloj
  porcentaje de tiempo de riego para ajustar programación sin cambiarla
  guarda los datos en la memoria EEPROM y recupera la programación después de de reiniciarse

** para integrar el programador de riego en Souliss utilizo una modificación de la libreria
   https://github.com/JMLoureiro77/ArduinoSlave. Modifico el codigo en este lado, no en Boss
   
  - en boss no hace falta declarar los pines pero
  tendremos en cuenta que el Slave_pin 2(SLOT_0) es el interuptor general
  pines 3,4,5,6,7,8,9,10 son los interruptores de los sectores
  (solo en lo referente a la comunicación con boss, aqui podeis usar los pines que querais)
  -funiona tb sin SS

Presupuesto y componentes:
  Arduino Pro-Mini (2€)
  Joystick (1€)
  RTC DS1307 (1€)
  Módulo de 8 Relés (6€)
  LCD1602 (2€)
  Cables (1€)
  Transformador USB (1€)
  Transformador 24vac (12€)
  Caja electrica (6€)

  Total <32€

  Instrucciones de uso en; https://www.youtube.com/watch?v=4QT0pNQ_CRE
**************************************************************************************************/
//---------------------------------------------------
// 1. Librerias
//---------------------------------------------------
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <RTClib.h>
#include <EEPROMex.h>

//----------------------------------------------------
// 2. Pines
//----------------------------------------------------
#define xPin     A1   
#define yPin     A0   
#define kPin     10   
#define pinS1     2   
#define pinS2     3   
#define pinS3     4   
#define pinS4     5   
#define pinS5     6   
#define pinS6     7   
#define pinS7     8   
#define pinS8     9   
//SDA            A4
//SCL            A5

//----------------------------------------------------
// 3. Variables y Comandos
//----------------------------------------------------
//leerJoystick
byte joyRead;
byte joyPos; 
byte lastJoyPos;
long lastDebounceTime = 0; 
long debounceDelay = 70;               //user define

//Control Joystick
bool PQCP;
byte editMode;

//sistema de menus
byte mNivel1;  
byte nS;  

//Hora
DateTime now;
int horaAc;// Hora actual en minutos (0 a (1440-1))
byte lastMinute = 0;
byte lastSecond = 0;
byte timer;
byte timer2;

//
bool IO=1;
byte percent=100;

byte clearSave;

//
byte buffer[2];
byte nH;


byte progRec[8][9];// Tr, h1,m1...h4,m4 (for eprom)

byte controlPin[8]={pinS1,pinS2,pinS3,pinS4,pinS5,pinS6,pinS7,pinS8};
bool controlS[8];  //horario
byte TAM[8];
bool a;

//boss comm
bool boss[8];
String inputString= "";
int inputInt[3];
bool stringComplete = false;
byte index=0;

//----------------------------------------------------
// 4. Objetos
//----------------------------------------------------
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27,16,2); 

//====================================================
// SETUP
//====================================================
void setup() {
    Serial.begin(9600);//bossComm
//----------------------------------------------------
// S1. Pines
//----------------------------------------------------
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(kPin, INPUT_PULLUP);
  for(int i=0;i<8;i++){
    digitalWrite(controlPin[i],HIGH);
    pinMode(controlPin[i], OUTPUT);
  }
//----------------------------------------------------
// S2. Objetos
//----------------------------------------------------
  rtc.begin();
  lcd.init();
  lcd.backlight(); 
//----------------------------------------------------
// S3. Program
//----------------------------------------------------
  eepromRead();
}

//====================================================
// LOOP
//====================================================
void loop() {
  now = rtc.now();
  horaAc = now.hour()*60 + now.minute();

  bossComm();
  
  controlJoystick();

  menu();

  for(int i=0;i<8;i++){
    //si ON y app o programa o arranque manual
    if (IO&&(controlS[i]||(TAM[i]>0)||boss[i])){
      digitalWrite(controlPin[i], LOW);//off relay
    } else {
    digitalWrite(controlPin[i], HIGH);//on relay
    }
  }//for  

  if(now.second()!=lastSecond){
    controlH();
    if (timer2==4){timer2=0;}
    timer++;
    timer2++;
    lastSecond=now.second();}
  if (timer>30&&!(controlS[0]||controlS[1]||controlS[2]||controlS[3]||controlS[4]||controlS[5]||controlS[6]||controlS[7])
                      &&!(TAM[0]||TAM[1]||TAM[2]||TAM[3]||TAM[4]||TAM[5]||TAM[6]||TAM[7])){
    lcd.noBacklight();
  }else{lcd.backlight();  }
  if (timer==90){
    lcd.clear();
    mNivel1=0;}//vuelve a pantalla de inicio tras 90s de inactividad       
  if (timer>250){timer=91;}//32k                

  if(now.minute()!=lastMinute){
  for(int i=0;i<8;i++){
    if (TAM[i]>0){TAM[i]--;}
  }//for  
  lastMinute=now.minute();}
}
void controlH(){
   for(int g=0;g<8;g++){//sector
    for(int i=1;i<8;i+=2){//si t!=0 y hini !=0 y horario
      if((progRec[g][0]!=0)&&(((progRec[g][i]*60)+(progRec[g][i+1]))!=0)&&((horaAc>=((progRec[g][i]*60)+(progRec[g][i+1])))&&(horaAc<((progRec[g][i]*60)+(progRec[g][i+1])+int(progRec[g][0]*(percent/100)))))){
         a=1;
      }
    }//for horas
    if (a==1){controlS[g]=1;}
    if (a==0){controlS[g]=0;}
    a=0;
  }//for sector
}
//====================================================
// Menu
//====================================================
void menu(){
  switch (mNivel1){
    case 0:
      menu0();//pantalla de inicio
    break;
    case 1:
        if(nS>0){menu11();
        }else{menu1();}
    break;
    case 2:
      menu2();
    break;
    case 3:
        if(nS>0){menu31();
        }else{menu3();}
    break;
    case 4:
      menu4(); //
    break;
    case 5:
      menu5(); //
    break;
  }//switch
}
//----------------------------------------------------
// Pantalla de inicio
//----------------------------------------------------
void menu0(){
  lcd.setCursor(0,0);
  printHour(now.hour());   
  printDigits(now.minute());
  lcd.setCursor(7,0);
  if(IO){lcd.print(" READY ");}
  if(!IO){lcd.print("ALL OFF");}
//  if(timer & 0x01){ par
  if(timer2==1){
    lcd.setCursor(0,1);
    lcd.print("S1 ");
    lcd.setCursor(4,1);
    lcd.print("S2 ");
    lcd.setCursor(8,1);
    lcd.print("S3 ");
    lcd.setCursor(12,1);
    lcd.print("S4 ") ;  
  }else if(timer2==2){
    lcd.setCursor(0,1);
    if (boss[0]){lcd.print("APP");
    }else if (TAM[0]>0){lcd.print("mON");
    }else if (controlS[0]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(4,1);
    if (boss[1]){lcd.print("APP");
    }else if (TAM[1]>0){lcd.print("mON");
    }else if (controlS[1]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(8,1);
    if (boss[2]){lcd.print("APP");
    }else if (TAM[2]>0){lcd.print("mON");
    }else if (controlS[2]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(12,1);
    if (boss[3]){lcd.print("APP");
    }else if (TAM[3]>0){lcd.print("mON");
    }else if (controlS[3]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
  }else if(timer2==3){
    lcd.setCursor(0,1);
    lcd.print("S5 ");
    lcd.setCursor(4,1);
    lcd.print("S6 ");
    lcd.setCursor(8,1);
    lcd.print("S7 ");
    lcd.setCursor(12,1);
    lcd.print("S8 ") ;  
  }else if(timer2==4){
    lcd.setCursor(0,1);
    if (boss[4]){lcd.print("APP");
    }else if (TAM[4]>0){lcd.print("mON");
    }else if (controlS[4]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(4,1);
    if (boss[5]){lcd.print("APP");
    }else if (TAM[5]>0){lcd.print("mON");
    }else if (controlS[5]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(8,1);
    if (boss[6]){lcd.print("APP");
    }else if (TAM[6]>0){lcd.print("mON");
    }else if (controlS[6]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
    lcd.setCursor(12,1);
    if (boss[7]){lcd.print("APP");
    }else if (TAM[7]>3){lcd.print("mON");
    }else if (controlS[7]>0){lcd.print("hON");
    }else {lcd.print("OFF");}
  }
}
//----------------------------------------------------
// MANUAL START                                 Menu 1
//----------------------------------------------------
void menu1(){
  lcd.setCursor(0,0);
  lcd.print("Manual Start");
  lcd.setCursor(0,1);
  lcd.print("go right & edit");
}
//-------------------------------------------------1.1
void menu11(){
  lcd.setCursor(0,0);
  lcd.print("Sector ");
  lcd.print(nS);
  lcd.setCursor(13,0);
  if(TAM[nS-1]>0)lcd.print("ON ");
  if(TAM[nS-1]==0)lcd.print("OFF");
  lcd.setCursor(0,1);
  lcd.print("left time ");
  printHour(TAM[nS-1]);
}
//----------------------------------------------------
// SET PERCENT                                  Menu 2
//----------------------------------------------------
void menu2(){
  lcd.setCursor(0,0);
  lcd.print("SET % of w Time");
  lcd.setCursor(6,1);
  lcd.print(percent);
  if (editMode>0){
    lcd.setCursor(15,1);
    lcd.print("#");
  } else {
    lcd.setCursor(15,1);
    lcd.print(" ");}
}
//----------------------------------------------------
// SET PROGRAM                                  Menu 3
//----------------------------------------------------
void menu3(){
  lcd.setCursor(0,0);
  lcd.print("SET PROGRAM");
  lcd.setCursor(0,1);
  lcd.print("go right & edit");
}
//-------------------------------------------------3.1
void menu31(){
  lcd.setCursor(0,0);
  if (editMode>0){lcd.print("#");
  } else {lcd.print("S");}
  lcd.setCursor(1,0);
  lcd.print(nS);
  lcd.setCursor(0,1);
  lcd.print("T ");//tr
  printHour(progRec[nS-1][0]);//tr
  lcd.setCursor(5,0);
  printHour(progRec[nS-1][1]);//h1
  lcd.setCursor(7,0);
  printDigits(progRec[nS-1][2]);
  lcd.setCursor(11,0);
  printHour(progRec[nS-1][3]);//h2
  lcd.setCursor(13,0);
  printDigits(progRec[nS-1][4]);
  lcd.setCursor(5,1);
  printHour(progRec[nS-1][5]);//h3
  lcd.setCursor(7,1);
  printDigits(progRec[nS-1][6]);
  lcd.setCursor(11,1);
  printHour(progRec[nS-1][7]);//h4
  lcd.setCursor(13,1);
  printDigits(progRec[nS-1][8]);
}
//----------------------------------------------------
// Clear / Save EEPROM                          Menu 4
//----------------------------------------------------
void menu4(){
  lcd.setCursor(4,0);
  lcd.print("EEPROM");
  if (editMode){
    lcd.setCursor(5,1);
      if (clearSave==0){
      lcd.print("Cancel");}
      if (clearSave==1){
      lcd.print("Save");}
      if (clearSave==2){
      lcd.print("Clear");}
    lcd.setCursor(15,1);
    lcd.print("#");
  } else {
    lcd.setCursor(2,1);
    lcd.print("Clear / Save");
    lcd.setCursor(15,1);
    lcd.print(" ");}
}
//----------------------------------------------------
void progRecClear(){
  for(int a=0;a<8;a++){
    for(int b=0;b<8;b++){
      progRec[a][b]=0;
    }
  }
}
//----------------------------------------------------
void eepromWrite(){
  int address=0;
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
       EEPROM.writeByte(address, progRec[i][j]);
       address++;
    }
  }
}
//----------------------------------------------------
void eepromRead(){
  int address=0;
  for(int i=0;i<8;i++){
    for(int j=0;j<8;j++){
      progRec[i][j]=EEPROM.readByte(address);
      address++;
    }
  }
}
//----------------------------------------------------
// Set HORA                                     Menu 5
//----------------------------------------------------
void menu5(){
  lcd.setCursor(0,0);
  lcd.print("Set Time   ");
  if (editMode>0){
    lcd.setCursor(10,0);
    lcd.print("#");} 
  lcd.setCursor(11,0);
  printHour(buffer[0]);   
  printDigits(buffer[1 ]);
  lcd.setCursor(0,1);
  lcd.print("Actual   ");
  lcd.setCursor(11,1);
  printHour(now.hour());   
  printDigits(now.minute());
}
//====================================================
// Control Joystic
//====================================================
void controlJoystick(){
  leeJoystick();
  if(PQCP) {
    PQCP=0;
    timer=0;
//-------------------------------------------------------------------------------------------
// JOYSTICK BUTTON== joyPos=5
//-------------------------------------------------------------------------------------------
    if (joyPos==5&&mNivel1==0){IO=!IO;
//manual start
//percent
    }else if (joyPos==5&&mNivel1==2&&editMode==0){
      editMode=2;
    }else if (joyPos==5&&mNivel1==2&&editMode>0){
      editMode=0;
      
//set prog
    }else if (joyPos==5&&mNivel1==3&&nS>0&&editMode==0){
      editMode=3;
      nH=0;

    }else if (joyPos==5&&mNivel1==3&&editMode>0){//on exit
      editMode=0;

//eprom
    }else if (joyPos==5&&mNivel1==4&&editMode==0){
      editMode=4;
      lcd.clear();
    }else if (joyPos==5&&mNivel1==4&&editMode>0){//on exit
      if (clearSave==1){//save
      eepromWrite();
      }
      if (clearSave==2){//clear
        progRecClear();
        eepromWrite();
      } 
      editMode=0;

//sethora
      }else if (joyPos==5&&mNivel1==5&&editMode==0){
      editMode=5;
      nH=0;
        buffer[0]=now.hour();
        buffer[1]=now.minute();
     }else if (joyPos==5&&mNivel1==5&&editMode>0){//on exit
       nH=0;
       DateTime dt(2015, 1, 1, buffer[0], buffer[1], 0);
       rtc.adjust(dt);
       editMode=0;
    }
//-------------------------------------------------------------------------------------------
//OTHER CONTROLS
//-------------------------------------------------------------------------------------------
    switch (editMode){
        case 0:           //no editMode
          if (mNivel1==1&&joyPos==3&&TAM[nS-1]>0&&nS>0){    //UP
            TAM[nS-1]--;}
          if (mNivel1<5&&joyPos==3&&(mNivel1!=1||nS==0)){    
            lcd.clear();   
            mNivel1++;
            nS=0;}
          if (mNivel1==1&&joyPos==4&&TAM[nS-1]<99&&nS>0){   //DOWN
            TAM[nS-1]++;}
          if (mNivel1>0&&joyPos==4&&(mNivel1!=1||nS==0)){    
            lcd.clear();    
            mNivel1--;
            nS=0;}
          if ((mNivel1==1||mNivel1==3)&&joyPos==1&&nS<8){   //RIGHT
            lcd.clear();    
            nS++;}
          if ((mNivel1==1||mNivel1==3)&&joyPos==2&&nS>0){   //LEFT
            lcd.clear();    
            nS--;}
       case 1: // editMode Manual start
       break;
       case 2: // editMode Ser Percent
          if (joyPos==4&&percent<250){//UP
            lcd.clear();    
            percent+=10;} 
          if (joyPos==3&&percent>10){//DOWN
            lcd.clear();    
            percent-=10;}   
        break;
       case 3: // editMode Set Program
          if (joyPos==4&&nH==0&&progRec[nS-1][0]<99){progRec[nS-1][0]++; }  //arriba
          if (joyPos==4&&(nH==1||nH==3||nH==5||nH==7)&&progRec[nS-1][nH]<23){
            progRec[nS-1][nH]++; }//arriba
          if (joyPos==4&&(nH==2||nH==4||nH==6||nH==8)&&progRec[nS-1][nH]<59){
            progRec[nS-1][nH]++; } //arriba
          if (joyPos==3&&progRec[nS-1][nH]>0){progRec[nS-1][nH]--;}   //abajo
          if (joyPos==1&&nH<8){nH++;  }      //derecha
          if (joyPos==2&&nH>0){nH--;  }       //izq
        break;
       case 4: // editMode Save/clear Proram
          if (joyPos==3&&clearSave<2){//DOWN
            lcd.clear();    
            clearSave++;} 
          if (joyPos==4&&clearSave>0){//UP
            lcd.clear();    
            clearSave--;}   
        break;
       case 5: // editMode Set Time
          if (joyPos==4&&buffer[0]<23&&nH==0){
            buffer[0]++; }//arriba
          if (joyPos==4&&buffer[1]<59&&nH==1){
            buffer[1]++; } //arriba
          if (joyPos==3&&buffer[nH]>0){buffer[nH]--;}   //abajo
          if (joyPos==1&&nH<1){nH++;  }      //derecha
          if (joyPos==2&&nH>0){nH--;  }       //izq
        break;
        }//!edit
  }//PQCP
}
//-------------------------------------------------------------------------------------------
int leeJoystick(){
//-------------------------------------------------------------------------------------------
  int x = analogRead(xPin);
  int y = analogRead(yPin);
  int k = digitalRead(kPin);
  if(x>900){joyRead=1;        //x+
  }else if(x<100){joyRead=2;  //x-
  }else if(y>900){joyRead=3;  //y+
  }else if(y<100){joyRead=4;  //y-
  }else if(!k){joyRead=5;
  }else{joyRead=0;}

  if (joyRead != lastJoyPos){lastDebounceTime = millis();}
  if(((millis() - lastDebounceTime) > debounceDelay)&&(joyRead!=joyPos)){
    joyPos=joyRead;
    if(!PQCP){PQCP=1;}
    }
  if(((millis() - lastDebounceTime) > (4*debounceDelay))&&(joyPos==3||joyPos==4)){
    joyPos=joyRead;                     //repeat time
    if(!PQCP){PQCP=1;}
    }
  lastJoyPos=joyRead;
}
//////////////////////////////////imprime horas
void printHour(byte digits){
  if(digits < 10){
    lcd.print(' ');
    lcd.print(digits,DEC);}
  else {lcd.print(digits,DEC);}
}
//////////////////////////////////imprime minutos :00
void printDigits(byte digits){
  lcd.print(":");
  if(digits < 10){
    lcd.print('0');
    lcd.print(digits,DEC);}
  else {lcd.print(digits,DEC);}
}
//=================================================================
void bossComm() {
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
      case 1:             
        if(inputInt[1]==2){//interruptor general
          IO=!IO;}
        if(2<inputInt[1]<12&&(inputInt[2]==0||inputInt[2]==1)){
          boss[inputInt[1]-3]=inputInt[2];}
      break;
      case 2:             //digital read
        if(1<inputInt[1]<14){
          Serial.print("2.");
          Serial.print(inputInt[1]);
          Serial.print(".");
          Serial.print(boss[inputInt[1]]);
          Serial.println(".");}
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
