/**************************************************************************

   This code is ESP side of the WaterStation
   with Souliss Integration
   First Slot is general Ready/Off switch, the same as joystick button
   on WS first screen
   Next are the valves (8)
   
   With this code we have ESP Access Point with XSSID name and you can use
   Android App "Souliss" to control it:
   Install App & link your phone to XSSID wifi
   Give to the app 192.168.4.1 as local direction of Soulis in Network Op.
   Next, in DB options, Get Souliss Nodes.
   
   But you need configure this node to include it on YOUR Souliss Network
   Visit: http://Souliss.net
   
   
   A little user manual is: https://www.youtube.com/watch?v=4QT0pNQ_CRE
   
**************************************************************************/
//-------------------------------------------------------------------------
// 1.     Configure the framework
//-------------------------------------------------------------------------
#include "bconf/MCU_ESP8266.h"              
#include "conf/Gateway.h"
#include "conf/IPBroadcast.h"

#define XSSID "Radio"		//AP ssid
#define PASS  "AAAAAAAA"  
//-------------------------------------------------------------------------
// 2.     Include framework code and libraries
//-------------------------------------------------------------------------
#include "Souliss.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

//-------------------------------------------------------------------------
// 4. Slots
//-------------------------------------------------------------------------
#define SLOT0             0		//Ready/Off Station
#define SLOT1             1		//first valve
#define SLOT2             2
#define SLOT3             3
#define SLOT4             4
#define SLOT5             5
#define SLOT6             6
#define SLOT7             7
#define SLOT8             8		//last valve

//-------------------------------------------------------------------------
// 5. Pines
//-------------------------------------------------------------------------
//for water station no need pin conf, it's in arduino side
#include "ArduinoSlave.h"
//-------------------------------------------------------------------------
// 7. Objects
//-------------------------------------------------------------------------

Slave Slave;

//=========================================================================
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
	for (i=0;1<9;i++){
		Set_SimpleLight(i);
	} 
    //Slave.Conf();not for Water Station
}
//=========================================================================
void loop()
{  
	EXECUTEFAST() {                     
		UPDATEFAST();   
  
		FAST_50ms() { 
			for (i=0;1<9;i++){
				Logic_SimpleLight(i);
				Slave.dW(i+2,!mOutput(i));//(!)if inverted relay
			}
		}
		// to sync first slot with state shown use the joystick button on 
		// LCD first screen
		FAST_GatewayComms(); 
		Slave.Comm();

    }
	EXECUTESLOW() {
		UPDATESLOW();
    } 

}  
/**************************************************************************
/*
  Start the node as an access point 
  no needed for normal Souliss use when it's on your SS network
  
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