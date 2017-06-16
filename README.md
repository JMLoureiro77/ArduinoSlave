La comunicacion sereial bidireccional era muy inestable, por eso en la version 0.3 cambiamos a iic

La comunicacion iic permite mas de un esclavo. Master1 es el ejemplo para un solo esclavo y Master 2 es el ejemplo para dos o mas esclavos.

Slave es el codigo que hay que cargar en arduino. Es para un Arduino NANO o UNO y facilmente adaptable a Mega

El "wiring" que he probado entre un wemosD1 y un Arduino Nano (version 5v) es SDA-SDA y SCL-SCL DIRECTAMENTE.
No funciono con resistencias pullup ni con conversor de nivel,pero si horas y horas DIRECTAMENTE.
El codigo master es para ESP8266 y permite cambiar SDA-SCL en el setup, con wire.begin(SDApin,SCLpin)

La parte de configuración de pines en esclavo necesita mejorar. 
Sigo buscando una manera de resetear arduino por software lo que permitiria configurarlo con ESP01, por el momento se me ocurre usar un pin de ESP para resetear al esclavo despues de enviar la configuración de pines

La parte de configuracion de pines de esclavo como analog output necesita mejorar. 

Esto ya no es una libreria, solo un par de codigos, uno para Master y otro para Slave.

# ArduinoSlave

Arduino library to Control Arduino by I2C with ESP8266 or other

To upgrade esp8266 with:

- 8 analog reads 0-5v
- 11 digital 5v IOs
- PWM 
- Variable exchange 
- ESP unsupported libraries can be put on Slave & send values

By jmloureiro77 & Juan Pinto
