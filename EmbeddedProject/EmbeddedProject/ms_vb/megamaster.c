/*
 * test2.c
 *
 * Created: 3/20/2016 6:09:06 PM
 * Author : john
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include "i2c_mst.h"


#define DEVICE_ADRES   8


int main(void)
{
	PORTD = 0x03; //pullup SDA en SCL
	initUSART();
    uint8_t data[10];
    init_master();

	writeString("Een testje\n\r");
    uint8_t teller=1;
	while (1)
	{
		verzenden(DEVICE_ADRES,teller++);   //verzend een 1 naar de slave
	    for(uint8_t i=0;i<8;i++) _delay_ms(250);

		ontvangen(DEVICE_ADRES,data,1);     //ontvang 1 byte van slave
	    writeString("\n\rdata van de RP6 "); writeInteger(data[0],10);				
		for(uint8_t i=0;i<8;i++) _delay_ms(250);
	
	}

}

