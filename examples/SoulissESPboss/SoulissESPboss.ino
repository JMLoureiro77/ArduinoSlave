/****************************************************
   Sketch by Jose Manuel Loureiro and Juan Pinto
   
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
#define SlavePin2 digital_out_start_high
#define SlavePin3 digital_out_start_high
#define SlavePin4 digital_out_start_high
#define SlavePin5 digital_out_start_high

#include "ArduinoSlave.h"
//----------------------------------------------------
// 7. Objects
//----------------------------------------------------

Slave Slave;

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
  EXECUTEFAST() {                     
    UPDATEFAST();   
  
      FAST_50ms() {   
        Logic_SimpleLight(SLOT0);
        Slave.dW(2,!mOutput(SLOT0));
        Logic_SimpleLight(SLOT1);
        Slave.dW(3,!mOutput(SLOT1));
        Logic_SimpleLight(SLOT2);
        Slave.dW(4,!mOutput(SLOT2));
        Logic_SimpleLight(SLOT3);
        Slave.dW(5,!mOutput(SLOT3));
        }

     FAST_1110ms() {
        Read_T51(SLOT4);
        Slave.aR(0);
        ImportAnalog(SLOT4,&Slave.aV[0]);
        }
      FAST_GatewayComms(); 
      Slave.Comm();

    }
    EXECUTESLOW() {
        UPDATESLOW();
    } 
 
}  
/**************************************************************************
/*!
  Start the node as an access point 
  
  from Souliss Library/Base/NetworkSetup.ccp lin250+
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
