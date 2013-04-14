// ============================================================================
// sensor.h
// ============================================================================
//  Created on: Dec 7, 2012
//      Author: Ruud Hendricksen
// ============================================================================

#ifndef SENSOR_H_
#define SENSOR_H_
typedef unsigned short uint16_t;
#define MODE_MIST1431              0
#define MODE_SEN200                1
#define MODE_SEN300                2
//#define STEFAN_BOARD 			   1
// Sensor commands
#define CMD_RESET                  (0x00)
#define CMD_POWER_LEVEL            (0x01)
#define CMD_STATUS                 (0x02)
#define CMD_IC_LOCK                (0xC3)
#define CMD_IC_UNLOCK              (0xC4)
#define CMD_SENSOR_POWER           (0x05)
#define CMD_SENSOR_SHUTDOWN        (0x06)
#define CMD_SENSOR_START           (0x07)
#define CMD_SENSOR_SETUP           (0xC8)
#define CMD_SENSOR_READ_OP         (0x09)
#define CMD_SENSOR_STATUS          (0x0A)
#define CMD_TEST_ENABLE            (0xD8)
#define CMD_TEST_DISABLE           (0x19)
#define CMD_APB_WRITE              (0xCE)
#define CMD_APB_READ               (0x4F)
#define CMD_NVMEM_UNLOCK_BANK      (0xD0)
#define CMD_NVMEM_LOCK_BANK        (0xD2)
#define CMD_NVMEM_SET_PWD          (0xD3)
#define CMD_NVMEM_READ             (0x15)
#define CMD_NVMEM_WRITE            (0x54)
#define CMD_REG_READ               (0x17)
#define CMD_REG_WRITE              (0x56)
#define CMD_SENSOR_MEASURE_DEFAULT (0x20)
#define CMD_SENSOR_MEASURE_CUSTOM  (0x21)

// Sensor blocks
#define SENSOR_NVM                 (0x00)
#define SENSOR_TEMP                (0x01)
#define SENSOR_RH                  (0x02)
#define SENSOR_ALS                 (0x03)
#define SENSOR_ADC                 (0x04)

// Default analog power controls
#define ANALOG_POWER_ON            (0x06)
#define ANALOG_POWER_OFF           (0x07)

// Sensor setup registers
#define REG_DATA                   (0x00)
#define REG_STATUS                 (0x01)
#define REG_CREG1                  (0x10)
#define REG_SREG0                  (0x20)
#define REG_SREG1                  (0x21)
#define REG_SREG2                  (0x22)
#define REG_SREG3                  (0x23)
#define REG_SREG4                  (0x24)
#define REG_SREG5                  (0x25)
#define REG_SREG6                  (0x26)

// Sensor responses
#define RSP_ERR	                   (0xA1A1)
#define RSP_ILLEGAL_MIST1431       (0x5555)
#define RSP_ILLEGAL_SENX00         (0xA2A2)
#define RSP_ACK                    (0xAC00)

// Command return values
#define RET_VAL_SUCCESS            (0)
#define RET_VAL_FAILURE            (-1)
#define RET_VAL_SUCCESS_SEND_CMD   (1)

// Platform definitions
//#define PLATFORM_SPIBUS            ((LPC_UART_TypeDef *)LPC_SSP1)		//from the schematics , this goes to PIN5,6,7
#define PLATFORM_SPIBUS            LPC_SSP1		//from the schematics , this goes to PIN5,6,7
#define STEFAN_BOARD				1
/*pins to be used :
 * P0.9 MOSI1 -> P5
 * P0.8 MISO1 -> P6
 * P0.7 SCK1  -> P7
 * P0.6 SSEL1 -> P8
 * P0.18 INT -> P11
 * P0.17 ENA -> P12
 * P0.0 ENA -> P9 //Stefan board
 * P0.1 INT -> P10 //Stefan board
 *
 */
#ifdef STEFAN_BOARD
#define GPIO_RESET		0
#define GPIO_INT		1
#else
#define GPIO_RESET		17
#define GPIO_INT		18
#endif
#define GPIO_CS			6

#define CLK_TIMER                  TIMER_MATCHPIN( TIMER_32BITS_0, 2 )   // PIO0_1
#define CLK_TIMER_PORTPIN          PCB_PORTPIN(0,1)                      // idem

#define SENSORRESET_PORTPIN        PCB_PORTPIN(1,0)
#define SENSORINT_PORTPIN          PCB_PORTPIN(0,7)
#define SENSORSSEL_PORTPIN         PCB_PORTPIN(0,2)    


// Global variables
extern int mode;
extern int ldo_ovr;
extern int factory_status;

// Function prototypes
void sensor_hardreset( int on );
void sensor_command_reset( void );
int  sensor_sendcommand( int command, int * response );

int  sensor_analogpower( int on );
int  sensor_sensorpower( int sensor, int on );
int  sensor_sensorsetup( int sensor, int reg, int data );
int  sensor_sensorstart( int sensor );
int  sensor_conversionready( int sensor );
int  sensor_waitforconversion( int sensor, int timeout );
int  sensor_readstatus( int sensor, int * status );
int  sensor_readdata( int sensor, int * data );
int  sensor_readapb( int addr, uint16_t * data );
int  sensor_readnvm( int addr, uint16_t * data );
int  sensor_writenvm( int addr, uint16_t data );
void sensor_init( void );

void sensor_active( void );
void sensor_inactive( void );

// TEMP related routines
int   sensor_temp_setsensor( int numofbits );
float sensor_temp_convertdatatotemp( int data );

// RH related routines
void  sensor_rh_readcalib( int RhSensor );
int   sensor_rh_setsensor( int RhSensor, float ftemp, int raw_temp );
float sensor_rh_convertdatatohumidity( int RhSensor, int data );

// ALS related routines
void  sensor_als_readcalib( int AlsSensor );
int   sensor_als_setsensor( int AlsSensor );
float sensor_als_convertdatatolux( int AlsSensor, int data );

// Test functions
//int sensor_testenable( void );
int sensor_testmodeunlocked( void );

// NVM functions
uint16_t nvm_readtype( void );
uint16_t nvm_readsubtype( void );
char * nvm_read12nc( void );
void nvm_detecttest( void );

typedef struct
{
	float temp;
	float lux;
	float hum;

} sensor_readings;

#endif /* SENSOR_H_ */
