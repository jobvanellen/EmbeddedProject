#include "rp6aansluitingen.h"
#include "i2c.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <util/delay.h>

#define FWD 0
#define BWD 1

#define TRUE 0xFF;
#define FALSE 0;

/*				*
****functies*****
*				*/
uint8_t getBumperLeft(void); 
uint8_t getBumperRight(void);
void rijVooruit();
void rijAchteruit();
void naarLinks();
void naarRechts();
void testCycle();
void wait(uint8_t seconden);
void stopDriving();
void incrementSpeed();
void decrementSpeed();

//setters
void setMotorPower(uint8_t right, uint8_t left);
void setMotorPowerDynamic(uint8_t right_des, uint8_t left_des);
void setMotorSpeed(float speedRight, float speedLeft);
void setMotorDirection(uint8_t left, uint8_t right);

//getters
float getDistanceByInterrupts(uint8_t interrupts);
float getTotalDistance();
uint8_t getBumperLeft(void);
uint8_t getBumperRight(void);

//initialisering
void init();
void dynamicUpdate();

/*				 *
****variabelen****
*				 */
//power on the motor
uint8_t curPower_left = 0;
uint8_t curPower_right = 0;
uint8_t snelheid = 100;

//current motor speed in mm/s
float curSpeed_left = 0;
float curSpeed_right = 0;
float desiredSpeed_left = 0;
float desiredSpeed_right = 0;

//timers
#define SPEED_TIMER_TRIGGER 200 //5x/sec
uint16_t speed_timer = 0;
uint16_t update_timer = 0;
uint8_t ms_timer = 0;
uint16_t control_timer = 0;

//motor direction
uint8_t curDirection_left = 0;
uint8_t curDirection_right = 0;

//distance the device has traveled in # of interrupts
uint8_t motorDistance_left = 0;
uint8_t motorDistance_right = 0;
uint8_t motorDistanceLast_left = 0;
uint8_t motorDistanceLast_right = 0;
uint16_t motorDistanceTotal_left = 0;
uint16_t motorDistanceTotal_right = 0;
uint16_t motorDistanceTotalCM = 0;

//compass direction
uint16_t compass = 0;

/*				 *
****i2c dingen****
*				 */
uint8_t data_ont[20]; //max 20
volatile uint8_t data_flag = FALSE;
volatile uint8_t databyte=0x00;
void ontvangData(uint8_t [],uint8_t);
uint8_t verzendByte();
void i2c();
void writeTotalDistance();
