/*
 * EmbeddedProject.c
 *
 * Created: 10-3-2016 12:32:59
 * Author : Jasper
 */ 

#include "main.h"


int main(void)
{	
	init();
    while (1) 
    {
		

		
		i2c();
		dynamicUpdate();
		aom();
    }
	return(0);
}




void i2c(){
	
	
	if(data_flag) {
		
		switch(data_ont[0]) {
			case 'w': rijVooruit();
				automatic = 0;
				break;
			case 'a': naarLinks();
				automatic = 0;
				break;
			case 's': rijAchteruit();
				automatic = 0;
				break;
			case 'd': naarRechts();
				automatic = 0;
				break;
			case 'k': decrementSpeed();
				automatic = 0;
				break;
			case 'm': incrementSpeed();
				automatic = 0;
				break;
			case 'o':

			 stopDriving();			
				break;
			case 'i': compass = data_ont[1];
				break;
			case 'q' :
			automatic = automatic ? 0 : 1;
			break; 
				 
			default: compass = data_ont[0] *2;
			/*case 'p': writeTotalDistance();
				break;*/
		}
		control_timer = 0;
		data_flag = FALSE;
	}
	if(control_timer == 1000){
		stopDriving();
	}	
}

void testCycle(){
	wait(2);
	rijVooruit();
	
	wait(2);
	naarLinks();
	
	wait(2);
	rijAchteruit();	
	
	wait(2);
	naarRechts();
	
	if(snelheid > 210)
		snelheid = 0;
	else
		snelheid += 25;
}

void wait(uint8_t seconden){
	for(int i = 0; i < 4*seconden; i++)
		_delay_ms(250);
}

void init(){
	cli();
	
	//set pins I/O
	//DDRA = 0x00; //00000000
	//DDRB =  //11011011
	DDRC= 0xFF; //111111xx
	DDRD = MOTOR_R | MOTOR_L; //01110010
	
	//initialize PWM (timer1)
	TCCR1A = (0 << WGM10) | (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B =  (1 << WGM13) | (0 << WGM12) | (1 << CS10);
	ICR1 = 210; 
	OCR1AL = 0;
	OCR1BL = 0;
	setMotorDirection(FWD, FWD);
	
	//initialize interrupts: int0 links, int1 rechts
	MCUCR = (0 << ISC11) | (1 << ISC10) | (0 << ISC01) | (1 << ISC00);
	GICR = (1 << INT2) | (1 << INT1);

	//initialiseer Timer 0: 100µs cycle
	TCCR0 =   (0 << WGM00) | (1 << WGM01)				//Counter mode:CTC Top:OCR0 Update:Immediate TOV0flag set on:MAX
			| (0 << COM00) | (0 << COM01)				//normal port OC0 disconnected
			| (0 << CS02)  | (1 << CS01) | (0 << CS00); //8bit prescaler
	OCR0  = 99;											//output compare register
	TIMSK = (1 << OCIE0);
	
	//////////////////////////////////////
	/*			i2c init functies		*/
	//////////////////////////////////////
	initUSART();
	init_i2c_slave(0x20);	
	/*ontvangData is de functie die uitgevoerd wordt 
	wanneer een byte via de i2c bus ontvangen wordt
	*/
	init_i2c_ontvang(ontvangData); 
	
	/*verzendByte is de functie die aangeroepen wordt
	wanneer de slave een byte naar de master verzend*/
	init_i2c_verzend(verzendByte);
		
	sei();
}

//always call this to maintain speed control 
void dynamicUpdate(){
	//amend motor speeds
	/*if(curSpeed_right < desiredSpeed_right) curPower_right++;
	if(curSpeed_left < desiredSpeed_left) curPower_left++;
	if(curSpeed_right > desiredSpeed_right) curPower_right--;
	if(curSpeed_left > desiredSpeed_left) curPower_left--;
	setMotorPower(curPower_right, curPower_left);*/
	
	if(curPower_left || curPower_right)
	TCCR1A = (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
	else
	TCCR1A = 0;
	
	if(getTotalDistance() >= 200){
		motorDistanceTotalCM ++;
		motorDistanceTotal_left = 0;
		motorDistanceTotal_right = 0;
	}
	if(getBumperRight() || getBumperRight())
	{
		
		stopDriving();
		
	}

		
	
	
}

void setMotorPower(uint8_t right, uint8_t left){
	if(right > 210) right = 210;
	if(left > 210) left = 210;
	OCR1AL = right;
	OCR1BL = left;
}

//sets the desired speed of the left and right motor which is adapted dynamically through the dynamicUpdate method
void setMotorSpeed(float speedRight, float speedLeft){// obsolete
	if(speedRight < 50)
	desiredSpeed_right = 0;
	if(speedRight > 1200)
	desiredSpeed_right = 1200;
	else
	desiredSpeed_right = speedRight;

	if(speedLeft < 50)
	desiredSpeed_left = 0;
	if(speedLeft > 1200)
	desiredSpeed_left = 1200;
	else
	desiredSpeed_left = speedLeft;
}

void setMotorPowerDynamic(uint8_t right_des, uint8_t left_des){
	while(right_des != curPower_right && left_des != curPower_left){
		if(right_des < curPower_right) curPower_right--;
		if(right_des > curPower_right) curPower_right++;
		if(left_des < curPower_left) curPower_left--;
		if(left_des > curPower_left) curPower_left++;
		setMotorPower(curPower_right, curPower_left);
		_delay_ms(2);
	}
}

void incrementSpeed(){
	snelheid += 25;
}

void decrementSpeed(){
	snelheid -= 25;
}

//sets the direction of the left and right motor respectively, only call when speed = 0
void setMotorDirection(uint8_t left, uint8_t right){
	if(left)
		PORTC |= DIR_L;
	else
		PORTC &= ~DIR_L;
		
	if(right)
		PORTC |= DIR_R;
	else
		PORTC &= ~DIR_R;
		
	curDirection_right = right;
	curDirection_left = left;
}

void rijVooruit(){
	if(!(curDirection_left == FWD && curDirection_right == FWD)){//zet snelheid naar 0 en verander de richting als dat nodig is
		setMotorPowerDynamic(0, 0);
		setMotorDirection(FWD,FWD);		
	}
	setMotorPowerDynamic(snelheid, snelheid);
}

void rijAchteruit(){
	if(!(curDirection_left == BWD && curDirection_right == BWD)){//zet snelheid naar 0 en verander de richting als dat nodig is
		setMotorPowerDynamic(0, 0);
		setMotorDirection(BWD,BWD);		
	}
	setMotorPowerDynamic(snelheid, snelheid);
}

void naarLinks(uint16_t cLeft){
	

	if(!(curDirection_left == BWD && curDirection_right == FWD)){//zet snelheid naar 0 en verander de richting als dat nodig is
		setMotorPowerDynamic(0, 0);
		setMotorDirection(BWD,FWD);
	}
	setMotorPowerDynamic(snelheid, snelheid);
	 

	//TODO gebruik kompas
}

void naarRechts(){
	if(!(curDirection_left == FWD && curDirection_right == BWD)){//zet snelheid naar 0 en verander de richting als dat nodig is
		setMotorPowerDynamic(0, 0);
		setMotorDirection(FWD,BWD);
	}
	setMotorPowerDynamic(snelheid, snelheid);
	//TODO gebruik kompas
}

void stopDriving(){
	
	if(automatic)
	{
		setMotorSpeed(100, 100);
		rijAchteruit(); 
		_delay_ms(250);
		
		
		naarRechts();
		wait(3);
		
		//rijVooruit();
	}
	else
	{	
	setMotorPower(0,0);
	curPower_left = 0;
	curPower_right = 0;
	}
}

uint8_t getBumperRight(void){
	PORTC &= ~SL3;
	DDRC &= ~SL3;
	uint8_t tmp = PINC & SL3;
	return tmp;
}

uint8_t getBumperLeft(void){
	PORTB &= ~SL6;
	DDRB &= ~SL6;
	uint8_t tmp = PINB & SL6;
	return tmp;
}

//each interrupt = .25 mm, therefore this returns #interrupts *.25
float getDistanceByInterrupts(uint8_t interrupts){
	return interrupts * 0.25; //return distance in mm
}

//returns the total driven distance in amount of interrupts
float getTotalDistance(){
	return (motorDistanceTotal_right + motorDistanceTotal_left)/2.0;
}

//writes the total distance to the arduino
void writeTotalDistance(){
	uint16_t a = 123;
	writeInteger(a, 10); //motorDistanceTotalCM
}

 /*slave heeft data ontvangen van de master
 data[]	-	 een array waarin de ontvangen data staat
 tel	-	 het aantal bytes dat ontvangen is*/ 
void ontvangData(uint8_t data[],uint8_t tel){
	for(int i=0;i<tel;++i)
	    data_ont[i]=data[i];
	data_flag = TRUE;
	//writeString("o\n\r");
}



void aom()

{	while(automatic){
		
	
	i2c();
	dynamicUpdate();
	rijVooruit();
	
}
}

/* het byte dat de slave verzend naar de master
in dit voorbeeld een eenvoudige t eller
*/
uint8_t verzendByte() {
	uint8_t temp = motorDistanceTotalCM;
	motorDistanceTotalCM = 0;
	return temp; //databyte++;
}

//external interrupt int0 left motor sensor
ISR (INT0_vect){
	motorDistance_left++;	//increment the amount of interrupts on the left side

	if(curDirection_right == curDirection_left)//if the car is not rotating increment the total distance
		motorDistanceTotal_left++;
}

//external interrupt int1 right motor sensor
ISR (INT1_vect){
	motorDistance_right++;	//increment the amount of interrupts on the right side

	if(curDirection_right == curDirection_left)//if the car is not rotating increment the total distance
		motorDistanceTotal_right++;
}

//timer interrupt for calc purposes
ISR (TIMER0_COMP_vect){
	
	if(ms_timer++ >= 9) {//1 interrupt per ms
		
		//calculate speed of both sides TODO maybe extra
		/*
		if(speed_timer++ > SPEED_TIMER_TRIGGER){
			curSpeed_right = motorDistance_right; //getDistanceByInterrupts(motorDistance_right) * 5; //in mm/s
			curSpeed_left = motorDistance_left; //getDistanceByInterrupts(motorDistance_left) * 5;	 //in mm/s
		
			//set the last distances to the current distances so the delta can be used next cycle
			motorDistanceLast_left = motorDistance_left;
			motorDistanceLast_right = motorDistance_right;
			motorDistance_right = 0;
			motorDistance_left = 0;
			speed_timer = 0;	//reset the timer so it triggers again	
		}
	
		if(update_timer++ > 2){
			dynamicUpdate();
			update_timer = 0;
		}*/
		
		ms_timer = 0;
		
		control_timer ++;
	}
}

//i2c interrupt
ISR(TWI_vect) {
	/*snelheid = 100;
	rijVooruit();*/
	slaaftwi();

}
