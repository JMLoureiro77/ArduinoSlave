# ArduinoSlave
Code pairs to control Arduino(one or more) from ESP8266 using IIC comunication.

MASTER1 is ESP's code to control one slave.
MASTER2 is ESP's code to control two or more slaves.
Slave is the code to flash in Arduino NANO(328P based)(&& Micro && UNO)
SlaveMEGA is the code to flash in Arduino MEGA.

IIC address of slave must be known by Master

Tested direct wiring SDA-SDA SCL-SCL between wemosD1 and Arduino Nano (5v version).
With pullup resistors or level converter not work for me.

SDA-SCL can be changed on ESP using  wire.begin(SDApin,SCLpin)
By default are D4 and D3 on WemosD1 or D1 and D2 on NodeMCU 1.0


To upgrade esp8266 with:

- analog reads 0-5v
- 5v digital  IOs
- PWM 
- Variable exchange 
- ESP unsupported libraries can be put on Slave & send values

Some more information in Spanish can be found in http://jmloureiro77.blogspot.com/2017/06/conunicacion-entre-esp8266-y-arduino.html

By jmloureiro77 & Juan Pinto
