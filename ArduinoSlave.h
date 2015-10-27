/****************************************************************************
	Souliss - ArduinoSlave
    Copyright (C) 2015  Jose Manuel Loureiro & Juan Pinto

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
	Originally developed by Jose Manuel Loureiro & Juan Pinto
	
*****************************************************************************/
#include <Arduino.h>

#define digital_write       1
#define digital_read        2  //x.0
#define analog_write        3
#define analog_read         4  //x.0
#define store_variable      5
#define get_variable        6  //x.0
#define configure_pin       7

#define digital_in
#define digital_out //start low
#define digital_out_start_high
#define analog out //(PWM)
#define digital_input_pullup

#ifndef Slave_h
# define Slave_h

#ifndef SlavePin2
# define SlavePin2			  	0
#endif
#ifndef SlavePin3
# define SlavePin3			  	0
#endif
#ifndef SlavePin4
# define SlavePin4			  	0
#endif
#ifndef SlavePin5
# define SlavePin5			  	0
#endif
#ifndef SlavePin6
# define SlavePin6			  	0
#endif
#ifndef SlavePin7
# define SlavePin7			  	0
#endif
#ifndef SlavePin8
# define SlavePin8			  	0
#endif
#ifndef SlavePin9
# define SlavePin9			  	0
#endif
#ifndef SlavePin10
# define SlavePin10			  	0
#endif
#ifndef SlavePin11
# define SlavePin11			  	0
#endif
#ifndef SlavePin12
# define SlavePin12			  	0
#endif
#ifndef SlavePin13
# define SlavePin13			  	0
#endif

class Slave
{
	public: 
	void Conf();
	void Comm();
	void dW(uint8 pin, bool val);		//digital write
	void dR(uint8 pin);					//digital read
	void aW(uint8 pin, uint16_t aWVal);	//analog write
	void aR(uint8 pin);					//analog read
	void sV(uint8 nVar, uint16_t nVarV); //send variable, store variable
	void gV(uint8 nVar);				//get variable
	float aV[8];			//analog read output
	bool dD[14];			//digital read output
	uint16_t nVar[10];		//valiable value
	

	private:
	bool val;
	const char* inputString;
	bool lastDWVal[14];
	uint16_t lastAWVal[14];
	uint16_t lastARVal[8];
	uint8_t indeX;
	uint16_t inputInt[3];
	bool stringComplete;
};

#endif