/*
 * i2cw.c
 *
 * Created: 3/19/2016 2:14:44 PM
 * Author : john
 */ 


int main(void)
{	
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
