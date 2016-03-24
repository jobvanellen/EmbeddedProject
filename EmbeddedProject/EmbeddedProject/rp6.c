/*
 * i2cw.c
 *
 * Created: 3/19/2016 2:14:44 PM
 * Author : john
 */ 


#include <avr/io.h>
#include <util/twi.h>
#include "i2c.h"
#include "rp6aansluitingen.h"

#define TRUE 0xFF;
#define FALSE 0;

/*de interrupt routine van de i2c
de functie slaaftwi() staat in de library
*/

ISR(TWI_vect) {

	slaaftwi();

}

uint8_t data_ont[20]; //max 20
volatile uint8_t data_flag = FALSE;
volatile uint8_t databyte=0x33;

void ontvangData(uint8_t [],uint8_t);
uint8_t verzendByte();

int main(void)
{

	DDRC=0xFF;
	initUSART();
	init_i2c_slave(8);
	
	/*ontvangData is de functie die uitgevoerd wordt 
	wanneer een byte via de i2c bus ontvangen wordt
	*/
	init_i2c_ontvang(ontvangData); 
	
	/*verzendByte is de functie die aangeroepen wordt
	wanneer de slave een byte naar de master verzend*/
	init_i2c_verzend(verzendByte);
	
	sei(); //De slave van i2c werkt met interrupt
	
    /* Replace with your application code */
    while (1) 
    {    
		if(data_flag) {
		writeInteger(data_ont[0],10);	
			switch(data_ont[0]) {
				case 1:PORTC |= SL1;
				break;
				case 2:PORTC |= SL2;
				break;
				case 3:PORTC |= SL3;
				break;
				case 4:PORTC &= ~SL1;
				break;
				case 5:PORTC &= ~SL2;
				break;
				case 6:PORTC &= ~SL3;
				break;
				
				
			}
			
		    data_flag = FALSE;	
		}
    }
	
}
 /*slave heeft data ontvangen van de master
 data[] een array waarin de ontvangen data staat
 tel het aantal bytes dat ontvangen is*/
 
void ontvangData(uint8_t data[],uint8_t tel){
	for(int i=0;i<tel;++i)
	    data_ont[i]=data[i];
	data_flag = TRUE;
	writeString("o\n\r");
}

/* het byte dat de slave verzend naar de master
in dit voorbeeld een eenvoudige teller
*/

uint8_t verzendByte() {
		return databyte++;
}