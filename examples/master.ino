/****************************************************
 *   Sketch by Jose Manuel Loureiro and Juan Pinto
              ESP Self Wifi & Gateway, no router
              & Arduino Slave
*****************************************************/
//---------------------------------------------------
// 1.     Configure the framework
//---------------------------------------------------
#include "bconf/MCU_ESP8266.h"              
#include "conf/Gateway.h"
#include "conf/IPBroadcast.h"


#define XSSID "Radio"
#define PASS  "AAAAAAAA"  
//---------------------------------------------------
// 2.     Include framework code and libraries
//---------------------------------------------------
#include "Souliss.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "ArduinoSlave.h"
//----------------------------------------------------
// 4. Slots
//----------------------------------------------------
#define SLOT0             0
#define SLOT1             1
#define SLOT2             2
#define SLOT3             3
#define SLOT4             4//&5
//----------------------------------------------------
// 5. Pines
//----------------------------------------------------
#define
#define


//----------------------------------------------------
// 6. Comandos
//----------------------------------------------------
bool lastSLOT[4];
bool lastSLOT0;
bool lastSLOT1;
bool lastSLOT2;
bool lastSLOT3;

// **************************** DEFINES ***********************
/* CHULETA DE COMANDOS
Esp send:                  int[0]  int[1]  int[2]
                    
digital write         1     pin       value
digital read          2     pin       0(else)
analog write          3     pin       value
analog read           4     pin       0(else)
store variable        5     nVar      value
get  variable         6     nvar      0(else)
configure pin         7     pin       confVal
Get pinConf           8     0         0(else)
clear pinConf         8     1         0(else)
reset arduino         9     0(else)   0(esle) */

#define digital_write       1
#define digital_read        2  //x.0
#define analog_write        3
#define analog_read         4  //x.0
#define store_variable      5
#define get_variable        6  //x.0
#define configure_pin       7
#define send_eeprom_values  8.0
#define clear_pinConf       8.1
#define reset_arduino       9.0

byte pinConf[14];//0,1 serial(not used)
                //D2-D13
String inputString= "";   // a string to hold incoming data
int inputInt[3];
bool stringComplete = false; // whether the string is complete
byte indeX=0;
///* if no needed
int nVar[10];
//*/
bool dr[14];
float ar[8];

//=======================================================
void setup()
{
    Initialize();
    SetAPoint();//**************SetAccessPoint*********//

	SetAsGateway(myvNet_dhcp); 
  //----------------------------------------------------
  //S2. Set vNet Address    
  //----------------------------------------------------
    SetAddress(0xAB01, 0xFF00, 0x0000);
  //----------------------------------------------------
  // S4. Set the typicals for this node
  //----------------------------------------------------
    Set_SimpleLight(SLOT0); 
    Set_SimpleLight(SLOT1); 
    Set_SimpleLight(SLOT2);  
    Set_SimpleLight(SLOT3); 
    Set_T51(SLOT4);

	Slave.Conf();
}

//=======================================================
void loop()
{  
  Slave.Comm();//////////////////////////////////////////////////////////////////////////
  EXECUTEFAST() {                     
    UPDATEFAST();   
  
      FAST_50ms() {   
        Logic_SimpleLight(SLOT0);
          if(mOutput(SLOT0)!=lastSLOT0){
            Slave.Do(digital_write,4,!mOutput(SLOT0));
            lastSLOT0=mOutput(SLOT0);}
        Logic_SimpleLight(SLOT1);        
          if(mOutput(SLOT1)!=lastSLOT1){
            Serial.print("1.5.");
            Serial.print(!mOutput(SLOT1));
            Serial.println(".");
            lastSLOT1=mOutput(SLOT1);}
        Logic_SimpleLight(SLOT2); 
          if(mOutput(SLOT2)!=lastSLOT2){
            Serial.print("1.4.");
            Serial.print(!mOutput(SLOT2));
            Serial.println(".");
            lastSLOT2=mOutput(SLOT2);}
        Logic_SimpleLight(SLOT3);
          if(mOutput(SLOT3)!=lastSLOT3){
            Serial.print("1.3.");
            Serial.print(!mOutput(SLOT3));
            Serial.println(".");
            lastSLOT3=mOutput(SLOT3);}
        }

 /*     FAST_1110ms() {
        Read_T51(SLOT4);
        Serial.print("4.14.0.");
        ImportAnalog(SLOT4,&ar[0]);
        }*/
      FAST_GatewayComms(); 
    }
    EXECUTESLOW() {
        UPDATESLOW();
    } 
 
}  
//=======================================================

/**************************************************************************
/*!
  Start the node as an access point 
  
  from SSLibrary/Base/NetworkSetup.ccp lin250+
*/  
/**************************************************************************/ 
void SetAPoint()
{
  uint8_t i;
  uint8_t ipaddr[4];
  uint8_t gateway[4];   
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(XSSID,PASS);

  IPAddress lIP  = WiFi.softAPIP();
  
  for(i=0;i<4;i++)
  {
    ipaddr[i]  = lIP[i];
    gateway[i] = lIP[i];
  } 

  myvNet_dhcp = ipaddr[3];
  
  for(i=0; i<4; i++)
  {
    if(DEFAULT_BASEIPADDRESS)   DEFAULT_BASEIPADDRESS[i]=ipaddr[i];
    if(DEFAULT_GATEWAY)     DEFAULT_GATEWAY[i] = gateway[i];
  }
  
  U16 vNet_address = (U16)ipaddr[i-1];      
  DEFAULT_BASEIPADDRESS[i-1]=0;     
  Souliss_SetAddress(vNet_address, DYNAMICADDR_SUBNETMASK, 0);  
}