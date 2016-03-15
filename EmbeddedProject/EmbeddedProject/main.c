/*
 * EmbeddedProject.c
 *
 * Created: 10-3-2016 12:32:59
 * Author : Jasper
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "rp6aansluitingen.h"

#define FWD 0
#define BWD 1

/*				*
****functies*****
*				*/
void rijVooruit(uint8_t afstand);
void rijAchteruit(uint8_t afstand);
void naarLinks(int graden);
void naarRechts(int graden);

//setters
void setMotorPower(uint8_t right, uint8_t left);
void setMotorPowerDynamic(uint8_t right_des, uint8_t left_des);
void setMotorSpeed(float speedRight, float speedLeft);
void setMotorDirection(uint8_t left, uint8_t right);

//getters
float getDistanceByInterrupts(uint8_t interrupts);
float getTotalDistance();

//initialisering
void init();

/*				 *
****variabelen****
*				 */
//power on the motor
uint8_t curPower_left = 0;
uint8_t curPower_right = 0;

//current motor speed
float curSpeed_left = 0;
float curSpeed_right = 0;
float desiredSpeed_left = 0;
float desiredSpeed_right = 0;
float targetSpeed = 0;

//timers
#define SPEED_TIMER_TRIGGER 200 //5x/sec
uint8_t speed_timer = 0;

//motor direction
uint8_t curDirection_left = 0;
uint8_t curDirection_right = 0;

//distance the device has travelled in # of interrupts
uint8_t motorDistance_left = 0;
uint8_t motorDistance_right = 0;
uint8_t motorDistanceLast_left = 0;
uint8_t motorDistanceLast_right = 0;
uint8_t motorDistanceTotal_left = 0;
uint8_t motorDistanceTotal_right = 0;

int main(void)
{	
	init();
    while (1) 
    {
		setMotorPowerDynamic(200, 200);
		//setMotorPowerDynamic(0,0);	
    }
	return(0);
}

void init(){
	cli();
	
	//set pins I/O
	//DDRA = 0x00; //00000000
	//DDRB =  //11011011
	DDRC= DIR_R | DIR_L; //111111xx
	DDRD = MOTOR_R | MOTOR_L; //01110010
	
	//initialiseer PWM
	TCCR1A = (0 << WGM10) | (1 << WGM11) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B =  (1 << WGM13) | (0 << WGM12) | (1 << CS10);
	ICR1 = 210; 
	OCR1AL = 0;
	OCR1BL = 0;
	setMotorDirection(FWD, FWD);
	
	//initialiseer interrupts: int0 links, int1 rechts
	MCUCR = (0 << ISC11) | (1 << ISC10) | (0 << ISC01) | (1 << ISC00);
	GICR = (1 << INT2) | (1 << INT1);

	//initialiseer Timer 0: 100µs cycle
	TCCR0 =   (0 << WGM00) | (1 << WGM01)				//Counter mode:CTC Top:OCR0 Update:Immediate TOV0flag set on:MAX
			| (0 << COM00) | (0 << COM01)				//normal port OC0 disconnected
			| (0 << CS02)  | (1 << CS01) | (0 << CS00); //8bit prescaler
	OCR0  = 99;											//output compare register
	
	sei();
}

void setMotorPower(uint8_t right, uint8_t left){
	if(right > 210) right = 210;
	if(left > 210) left = 210;
	OCR1AL = right;
	OCR1BL = left;
}

void setMotorPowerDynamic(uint8_t right_des, uint8_t left_des){
	while(right_des != curPower_right && left_des != curPower_left){
		if(right_des < curPower_right) curPower_right--;
		if(right_des > curPower_right) curPower_right++;
		if(left_des < curPower_left) curPower_left--;
		if(left_des > curPower_left) curPower_left++;
		setMotorPower(curPower_right, curPower_left);
	}
}

void setMotorSpeed(float speedRight, float speedLeft){
	if(speedRight < 13) 
		desiredSpeed_right = 0;
	else
		desiredSpeed_right = speedRight;

	if(speedLeft < 13) 
		desiredSpeed_left = 0;	
	else
		desiredSpeed_left = speedLeft;
}

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

void rijVooruit(uint8_t afstand){ //afstand in mm
	if(curDirection_left != FWD && curDirection_right != FWD){
		setMotorPowerDynamic(0, 0);
		setMotorDirection(FWD, FWD);
		setMotorPowerDynamic(150, 150);
	}//TODO gebruik afstand
}

void rijAchteruit(uint8_t afstand){// afstand in mm
	if(curDirection_left != BWD && curDirection_right != BWD){
		setMotorPowerDynamic(0, 0);
		setMotorDirection(BWD, BWD);
		setMotorPowerDynamic(150, 150);
	}//TODO gebruik afstand
}

void naarLinks(int graden){
	//TODO gebruik kompas
}

void naarRechts(int graden){
	//TODO gebruik kompas
}

float getDistanceByInterrupts(uint8_t interrupts){
	return interrupts * 0.25; //return distance in mm
}

float getTotalDistance(){
	return getDistanceByInterrupts((motorDistanceTotal_right + motorDistanceTotal_left)/2);
}

//external interrupt int0 left motor sensor
ISR (INT0_vect){
	motorDistance_left++;

	if(curDirection_right == curDirection_left)
		motorDistanceTotal_left++;
}

//external interrupt int1 right motor sensor
ISR (INT1_vect){
	motorDistance_right++;

	if(curDirection_right == curDirection_left)
		motorDistanceTotal_right++;
}

//timer interrupt for calc purposes
ISR (TIMER0_COMP_vect){
	//calculate speed of both sides
	if(speed_timer++ > SPEED_TIMER_TRIGGER){
		curSpeed_right = getDistanceByInterrupts(motorDistance_right - motorDistanceLast_right) * 5; //in mm/s
		curSpeed_left = getDistanceByInterrupts(motorDistance_left - motorDistanceLast_left) * 5;	 //in mm/s

		motorDistanceLast_left = motorDistance_left;
		motorDistanceLast_right = motorDistance_right;
		speed_timer = 0;
	}
}