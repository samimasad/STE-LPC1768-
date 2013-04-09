// ============================================================================
// sensor.c
// ============================================================================
//  Created on: Dec 7, 2012
//      Author: Ruud Hendricksen
// ============================================================================


#include "lpc17xx.h"
#include "half.h"
#include "lpc17xx_ssp.h"
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "sensor.h"
// Global variables

int mode           = MODE_SEN300;
int ldo_ovr        = 0;
int factory_status = 0;

static char buf[40];

// External function prototypes
#if 0
extern void modem_debug( char * dbg );
extern void modem_error( char * dbg );
extern void modem_send_text( char * token, char * val );
#endif
// ============================================================================
// LOW LEVEL SPI FUNCTIONS
// ============================================================================




/**
* Select/Deselect the sensor chip.
*/
static void sensor_spichipselect( int select ) {

#if 0
	//not needed , LPC will take care of chip select
  if ( select ) {
    gpio_clr( SENSORSSEL_PORTPIN );
  } else {
    gpio_set( SENSORSSEL_PORTPIN );
  }
#endif
}

/**
* Configures the SPI bus using the specific platform defines
*/
void sensor_spiconfigure( void ) {
  static int done = 0;
  if ( !done ) {
	  SSP_CFG_Type config ;
	  SSP_ConfigStructInit(&config);
	  config.CPOL = 1 ;
	  config.CPHA = 1 ;
	  config.ClockRate = 1000000 ;
	  SSP_Init(PLATFORM_SPIBUS,&config);
	  SSP_Cmd(PLATFORM_SPIBUS, ENABLE);

	//ssp_Init(PLATFORM_SPIBUS, 1, 1);             // bus, cpol, cpha : SSP0 on LPC1343
    //ssp_SetSpeed(PLATFORM_SPIBUS, 1000000 );     // bus, bitrate    : SSP0 on LPC1343
    
    //gpio_alloc( SENSORSSEL_PORTPIN, PCB_MODE_PULLUP, GPIO_OUTPUT );
    done = 1;
  }
  sensor_spichipselect( 0 );
}

/**
* Send 16 bits over the SPI bus.
*/
static void sensor_spisend16(int val) {
	//uint8_t buf[2];
	//buf[0] = (val >> 8) & 0xFF;
	//buf[1] = val & 0xFF;
	uint16_t data ;
	data = (uint16_t) val ;

	taskENTER_CRITICAL();
	SSP_SendData(PLATFORM_SPIBUS,data);
	taskEXIT_CRITICAL();
/*
 ssp_Lock(PLATFORM_SPIBUS);
 ssp_Send(PLATFORM_SPIBUS, buf, 2);
 ssp_Unlock(PLATFORM_SPIBUS);
 */
}

/**
* Receive 16 bits from the SPI bus and return them in an integer.
*/
static int sensor_spireceive16( void ) {

	 /*
	  uint8_t buf[2];

  ssp_Lock(PLATFORM_SPIBUS);
  ssp_Receive(PLATFORM_SPIBUS, buf, 2, 0);
  ssp_Unlock(PLATFORM_SPIBUS);
  */
	uint16_t data ;
	taskENTER_CRITICAL();
	data = SSP_ReceiveData(PLATFORM_SPIBUS);
	taskEXIT_CRITICAL();
	/*
  unsigned int ret = 0;
  ret |= ( buf[0] << 8 );
  ret |= buf[1];
  */
  return( (int)data );
}

// ============================================================================
// LOW LEVEL SENSOR FUNCTIONS
// ============================================================================

#define CLK_STATE_NONE    0
#define CLK_STATE_TIMER   1
#define CLK_STATE_ZERO    2
#ifdef STEFAN_BOARD
#define GPIO_RESET		0
#define GPIO_INT		1
#else
#define GPIO_RESET		17
#define GPIO_INT		18
#endif
/**
* Controls the clock to the sensor (1 MHz to MIST1431, LOW to SenX00)
*/
static void sensor_sensorclk( int enable ) {

	if(enable)
	{
		//get enable signal to low

		GPIO_ClearValue(0 , 1L<<GPIO_RESET);





	}
	else
	{
		//get it to high
		GPIO_SetValue(0 , 1L<<GPIO_RESET);



	}




#if 0
  static int clk_state = CLK_STATE_NONE;

  if ( mode == MODE_MIST1431 ) {
    if ( clk_state == CLK_STATE_ZERO ) gpio_free( CLK_TIMER_PORTPIN );
    
    if ( enable && ( clk_state != CLK_STATE_TIMER ) ) {
	  CLKOUTDIV=clk_get_cclk()/1000000; // enable, 1mhz
      clk_state = CLK_STATE_TIMER;
    } else if ( clk_state == CLK_STATE_TIMER ) {
	  CLKOUTDIV=0; // disable
      clk_state = CLK_STATE_NONE;
    }
  }

  else {
    if ( clk_state == CLK_STATE_TIMER ) CLKOUTDIV=0; // disable
    
    if ( clk_state != CLK_STATE_ZERO ) {
      gpio_alloc( CLK_TIMER_PORTPIN, PCB_MODE_PULLUP, GPIO_OUTPUT );
      gpio_clr( CLK_TIMER_PORTPIN ); 
      clk_state = CLK_STATE_ZERO;
    }
  }
#endif

}

/**
* Controls if sensor is in reset
*/
static void sensor_sensorreset( int enable ) {
  static int first = 1;
  if ( first ) {
    first = 0;
    //gpio_alloc( SENSORRESET_PORTPIN, PCB_MODE_PULLUP, GPIO_OUTPUT );

	//set the enable pin to output but put it to high initially

    GPIO_SetDir(0, 1L<<GPIO_RESET , 1);


  }
  //if ( enable ) gpio_clr( SENSORRESET_PORTPIN );
  //else gpio_set( SENSORRESET_PORTPIN );
  if ( enable )
  {
	  GPIO_ClearValue(0 , 1L<<GPIO_RESET);
  }
  else
  {
	  GPIO_SetValue(0 , 1L<<GPIO_RESET);
  }

}

/**
* Returns the status of the SINT pin of the sensor
*/
static int sensor_interrupt() {
  static int first = 1;
  if ( first ) {
    first = 0;
    //gpio_alloc( SENSORINT_PORTPIN, PCB_MODE_PULLUP, GPIO_INPUT );

    GPIO_SetDir(0, 1L<<GPIO_INT , 0);

  }
  //return( gpio_getval( SENSORINT_PORTPIN ) );
  sprintf( buf, "\r\nSensor sensor_interrupt results %x", GPIO_ReadValue(0) );
  sprintf( buf, "\r\nSensor sensor_interrupt masked results %x", GPIO_ReadValue(0)&  1L<<GPIO_INT );
   //modem_debug( buf );
   console_uart_sendString(buf);

  return(GPIO_ReadValue(0) &  1L<<GPIO_INT );

}

// ============================================================================
// LOW LEVEL FUNCTIONS
// ============================================================================

/**
* Wait until sensor drives the SINT line HIGH.
* - Returns 0 when a timeout occurs.
*/
static int sensor_waitforinterrupt( void ) {
  int timeout = 50;
  //while ( !sensor_interrupt() && ( --timeout >= 0 ) ) rtk_tsk_sleep(10);    // 1);
  while ( !sensor_interrupt() && ( --timeout >= 0 ) ) vTaskDelay(10);    // 1);
  return( timeout >= 0 );
}

/**
* Wait for sensor to be ready to receive a new command (drives the SINT line LOW).
* - Returns 0 when a timeout occurs.
*/
static int sensor_waitforreadytoreceive( void ) {
  int timeout = 50;
  //while ( sensor_interrupt() && ( --timeout >= 0 ) ) rtk_tsk_sleep(10);    // 1);
  while ( sensor_interrupt() && ( --timeout >= 0 ) ) vTaskDelay(10);    // 1);
  return( timeout >= 0 );
}

/**
* Hard reset of sensor wit the RESETN line.
* - Also control the sensor clock
* - And catches the first Status report of the sensor into a global variable
*/
void sensor_hardreset( int on ) {
  if ( on ) {
    sensor_sensorclk( 0 );
    vTaskDelay(2);
    sensor_sensorreset( 1 ) ;
  } else {
    //rtk_tsk_sleep( 2 );
	vTaskDelay(2);
    sensor_sensorreset( 0 ) ;
    
    // When reset is gone, clock starts automatically on SENx00, but must be 
    // done manually for MIST1431
    sensor_sensorclk( 1 );
    vTaskDelay(2);
    // Give clock some time
    //rtk_tsk_sleep( 2 );
    vTaskDelay(2);
    
    // When there has been a reset (POR or SW driven), then the sensor expects
    // the host to read 16-bit and this is the Sensor IC status.
    // Once the status is read, then SINT will go low, which will suggest that
    // the Sensor is accepting commands.
    if ( sensor_waitforinterrupt() ) {
      sensor_spichipselect( 1 );
      factory_status = sensor_spireceive16();
      sensor_spichipselect( 0 );
    }
  }
}

// ============================================================================
// SPI LEVEL COMMAND FUNCTIONS
// ============================================================================

/**
* Send a Sensor command and wait for the response.
* - Return the 16-bit response as an integer.
* - Returns 1 on success.
*/
static int sensor_response( int numcmd, int * command, int * response ) {
  int ok = 0;
  int i;

//dio_printf( DIO_VUART0, "111\n");
  
  // Select the Sensor SPI device
  sensor_spichipselect( 1 );

//dio_printf( DIO_VUART0, "222\n");
  
  if ( sensor_waitforreadytoreceive() ) {
    
//dio_printf( DIO_VUART0, "333\n");

    // Send command word(s) over SSP
    for ( i=0; i<numcmd; i++ ) {
      sensor_spisend16( command[i] );
    }
    
//dio_printf( DIO_VUART0, "444\n");

    // After sending the command, de-select the Sensor SPI device ...
    sensor_spichipselect( 0 );
    
//dio_printf( DIO_VUART0, "555\n");

    // ... and wait for the sensor to respond ...
    if ( sensor_waitforinterrupt() ) {
//      dio_printf( DIO_VUART0, "666\n");

      // Select the Sensor SPI device to read the response
      sensor_spichipselect( 1 );
      
//dio_printf( DIO_VUART0, "777\n");

      // Read response over SSP
      *response = sensor_spireceive16();
      ok = 1;
      
//dio_printf( DIO_VUART0, "888\n");

      // Wait for the command to completely finish
      if ( sensor_waitforreadytoreceive() ) {
        // Cycle finished
//dio_printf( DIO_VUART0, "999\n");

      }
      //else modem_debug( "error" );  // Missing SINT low at cycle-end
      else console_uart_sendString(  "\r\nerror" );
      
    }
//dio_printf( DIO_VUART0, "000\n");

  }
  
  // After reading the data, de-select the Sensor SPI device
  sensor_spichipselect( 0 );

//    if        ( numcmd == 1 ) {
//      sprintf( buf, ", %2x, %2x,     ,     , %4x, ", command[0] >> 8, command[0] & 0xFF, *response );
//    } else if ( numcmd == 2 ) {
//      sprintf( buf, ", %2x, %2x, %4x,     , %4x", command[0] >> 8, command[0] & 0xFF, command[1], *response );
//    } else if ( numcmd == 3 ) {
//      sprintf( buf, ", %2x, %2x, %4x, %4x, %4x", command[0] >> 8, command[0] & 0xFF, command[1], command[2], *response );
//    } else {
//      sprintf( buf, "??" );
//    }
//    modem_send_text( "c", buf );

  
  return( ok );
}

// ============================================================================
// COMMAND FUNCTIONS
// ============================================================================

/**
* Print Command error.
*/
static void sensor_printcommanderror( int command ) {
  sprintf( buf, "\r\nSensor error on command %d", command );
  //modem_debug( buf );
  console_uart_sendString(buf);
}

/**
* Send a Sensor command.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_sendcommand( int command, int * response ) {
  int ok = sensor_response( 1, &command, response );
  if ( !ok ) sensor_printcommanderror( command );
  return( ok );
}

/**
* Send a Sensor command with 1 additional 16-bit parameter.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_sendcommand1( int command, int data, int * response ) {
  int cmd[2];
  cmd[0] = command;
  cmd[1] = data;
  int ok = sensor_response( 2, cmd, response );
  return( ok );
}

/**
* Send a Sensor command with 2 additional 16-bit parameters.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_sendcommand2( int command, int data1, int data2, int * response ) {
  int cmd[3];
  cmd[0] = command;
  cmd[1] = data1;
  cmd[2] = data2;
  int ok = sensor_response( 3, cmd, response );
  return( ok );
}

// ============================================================================
// HIGH-LEVEL COMMAND FUNCTIONS
// ============================================================================

/**
* Perform a soft Sensor reset.
*/
void sensor_command_reset( void ) {
  int cmd = ( CMD_RESET << 8 );
  int response;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Sensor Reset: status = 0x%x", response );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
}

/**
* Request the Sensor status.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_command_status( int * pstatus ) {
  int cmd = ( CMD_STATUS << 8 );
  int response;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Sensor Status : status = 0x%x", response );
    //modem_debug( buf );
    console_uart_sendString(buf);
    if ( pstatus ) *pstatus = response;
    return( 1 );
  }
  return( 0 );
}

// ============================================================================
// GENERIC SENSOR FUNCTIONS
// ============================================================================

/**
* Converts standard response into a string
*/
char * response2text( int response )
{
  static char smallbuf[8];
  if ( response == 0xac00 ) return( "OK" );
  sprintf( smallbuf, "0x%04x", response );
  return( smallbuf );
}

/**
* Power the analogue supply to the Sensors.
* - Returns 1 on success.
* - On SenX00, analog power supply should never be switched off
*/
int sensor_analogpower( int on ) {
  if ( mode == MODE_MIST1431 ) {
    int cmd = ( CMD_POWER_LEVEL << 8 );
    if ( ldo_ovr ) {
      cmd |= ( on ) ? ANALOG_POWER_ON : ANALOG_POWER_OFF;
    } else {
      int ldo_ana = ( factory_status >> 9 ) & 0x07;
      cmd |= ( ldo_ana << 1 );
      cmd |= ( on ) ? 0 : 1;
    }
    int response = 0;
    if ( sensor_sendcommand( cmd, &response ) ) {
      sprintf( buf, "Analog power %s: %s", ((on)?"ON":"OFF"),
              response2text(response) );
      //modem_debug( buf );
      console_uart_sendString(buf);
    }
    return( response == RSP_ACK );
  }
  return( 1 );
}

/**
* Power a Sensor.
* - Returns 1 on success.
*/
int sensor_sensorpower( int sensor, int on ) {
  int cmd = ( on ) ? CMD_SENSOR_POWER : CMD_SENSOR_SHUTDOWN;
  cmd = ( cmd << 8 ) | sensor;
  int response = 0;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Powering S%d %s (0x%x): %s", sensor, ((on)?"ON":"OFF"), 
            cmd, response2text(response) );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
  return( response == RSP_ACK );
}

/**
* Sets-up a Sensor.
* - Returns 1 on success.
*/
int sensor_sensorsetup( int sensor, int reg, int data ) {
  int cmd = ( CMD_SENSOR_SETUP << 8 ) | sensor;
  int response = 0;
  if ( sensor_sendcommand2( cmd, reg, data, &response ) ) {
    sprintf( buf, "Setting-up S%d (0x%x,0x%x): %s", 
            sensor, reg, data, response2text(response) );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
  return( response == RSP_ACK );
}

/**
* Start a Sensor Conversion.
* - Returns 1 on success.
*/
int sensor_sensorstart( int sensor ) {
  int cmd = ( CMD_SENSOR_START << 8 ) | sensor;
  int response = 0;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Starting S%d: %s", sensor, response2text(response) );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
  return( response == RSP_ACK );
}

/**
* Check if a sensor conversion is ready by checking the irq flags
*/
int sensor_conversionready( int sensor ) {
  int irq_mask, status;
  
  switch (sensor) {
  case SENSOR_ADC:  irq_mask = 0x8000; break;
  case SENSOR_ALS:  irq_mask = 0x4000; break;
  case SENSOR_RH:   irq_mask = 0x2000; break;
  case SENSOR_TEMP: irq_mask = 0x1000; break;
  default: return( 0 );
  }
  
  sensor_command_status( &status );
  
  return( ( status & irq_mask ) != 0 );
}

/**
* Wait until a Sensor is ready.
* - Returns 0 when a timeout occurs.
*/
int sensor_waitforconversion( int sensor, int timeout ) {
  
  while ( timeout > 0 && !sensor_conversionready(sensor) ) {
    timeout -= 5;
    //rtk_tsk_sleep( 5 );
    vTaskDelay(5);
  }
  
  return( timeout > 0 );
}

/**
* Read status from a Sensor.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_readstatus( int sensor, int * pstatus ) {
  int cmd = ( CMD_SENSOR_STATUS << 8 ) | sensor;
  int response = 0;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Reading S%d: status = 0x%x", sensor, response );
    //modem_debug( buf );
    console_uart_sendString(buf);
    if ( pstatus ) *pstatus = response;
    return( 1 );
  }
  return( 0 );
}

/**
* Read data from a Sensor.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_readdata( int sensor, int * data ) {
  int cmd = ( CMD_SENSOR_READ_OP << 8 ) | sensor;
  int response = 0;
  if ( sensor_sendcommand( cmd, &response ) ) {
    sprintf( buf, "Reading S%d: data = 0x%x", sensor, response );
    //modem_debug( buf );
    console_uart_sendString(buf);
    *data = response;
    return( 1 );
  }
  return( 0 );
}

/**
* Read one 16-bit value from the NVM at a certain address.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
int sensor_readnvm( int addr, uint16_t * data ) {
  int cmd = ( CMD_NVMEM_READ << 8 ) | addr;
  int response = 0;
  if ( sensor_sendcommand( cmd, &response ) ) {
    *data = response;
    sprintf( buf, "NVM[%02x] = 0x%x (r)", addr, (volatile uint16_t *)*data );
    //modem_debug( buf );
    console_uart_sendString(buf);
    return( 1 );
  }
  return( 0 );
}

/**
* Write one 16-bit value to the NVM at a certain address.
*/
int sensor_writenvm( int addr, uint16_t data ) {
  int cmd = ( CMD_NVMEM_WRITE << 8 ) | addr;
  int response = 0;
  if ( sensor_sendcommand1( cmd, data, &response ) ) {
    sprintf( buf, "NVM[%02x] = 0x%x (w) : %s", 
            addr, data, response2text(response) );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
  return( response == RSP_ACK );
}

/**
* Read one 16-bit value from the NVM at a certain address.
* - Return the 16-bit response via the (int) reference parameter.
* - Returns 1 on success.
*/
//int sensor_readapb( int addr, uint16_t * data ) {
//  int cmd = ( CMD_APB_READ << 8 ) | addr;
//  int response = 0;
//  if ( sensor_sendcommand( cmd, &response ) ) {
//    *data = response;
//    sprintf( buf, "NVM[%02x] = 0x%x (r)", addr, (volatile uint16_t *)*data );
//    modem_debug( buf );
//    return( 1 );
//  }
//  return( 0 );
//}
int sensor_readapb( int addr, uint16_t * data ) {
  int cmd = ( CMD_APB_READ << 8 );
  int response = 0;
  if ( sensor_sendcommand1( cmd, addr, &response ) ) {
    *data = response;
    sprintf( buf, "NVM[%02x] = 0x%x (r)", addr, (volatile uint16_t *)*data );
    //modem_debug( buf );
    console_uart_sendString(buf);
    return( 1 );
  }
  return( 0 );
}

/**
* Write one 16-bit value to the NVM at a certain address.
*/
int sensor_writeapb( int addr, uint16_t data ) {
  int cmd = ( CMD_APB_WRITE << 8 );
  int response = 0;
  if ( sensor_sendcommand2( cmd, addr, data, &response ) ) {
    sprintf( buf, "NVM[%02x] = 0x%x (w) : %s", 
            addr, data, response2text(response) );
    //modem_debug( buf );
    console_uart_sendString(buf);
  }
  return( response == RSP_ACK );
}

// ============================================================================
// NVM SPECIFIC FUNCTIONS
// ============================================================================

int       CalibRead = 0;
uint16_t  Calib[56] ;

/**
* Read all MIST calibration values in one go (see Table 58).
* - Results are stored in the global array Calib.
* - Returns 1 on success.
*/
int MistNvmReadCalib( void ) {
  int addr = 0x40;
  uint16_t *pCalib;
  
  pCalib = Calib;
  while ( addr < 0x78 && sensor_readnvm( addr, pCalib ) ) {
    addr++;
    pCalib++;
  }
  if ( addr < 0x78 ){
    //modem_debug( "Error reading calibration values from NVM" );
	  console_uart_sendString("Error reading calibration values from NVM" );
    return( 0 );
  }
  
  CalibRead = 1;
  return( 1 );
}

// ============================================================================
// NVM DATASHEET SECTION
// ============================================================================
// Address 20 contains typenumber in hex
// Address 21 contains subtypenumber in hex
// Address 22 contains first 4 digits of 12 nc
// Address 23 contains middle 4 digits of 12 nc
// Address 24 contains last 4 digits of 12 nc
//
// So SEN300/01 with 12nc 9352 991 54115 becomes
// 20 -> 0300h
// 21 -> 0001h
// 22 -> 9352h
// 23 -> 9915h
// 24 -> 4115h
//
// So SEN200/01 with 12nc 9353 006 43115 becomes
// 20 -> 0200h
// 21 -> 0001h
// 22 -> 9353h
// 23 -> 0064h
// 24 -> 4115h
// ============================================================================

#define NVM_DATASHEET  0x20

char buf12nc[16];

uint16_t nvm_readtype( void ) {
  // Assumes that sensor is active
  uint16_t typenumber;
  sensor_readnvm( NVM_DATASHEET + 0, &typenumber );
  return( typenumber );
}

uint16_t nvm_readsubtype( void ) {
  // Assumes that sensor is active
  uint16_t subtypenumber;

  sensor_readnvm( NVM_DATASHEET + 1, &subtypenumber );
  return( subtypenumber );
}

char * nvm_read12nc( void ) {
  // Assumes that sensor is active
  uint16_t fourchars;
  int i,j;
  for ( i=0; i<3; i++ ) {
    sensor_readnvm( NVM_DATASHEET + 2 + i, &fourchars );
    for (j=0; j<4; j++ ) {
      buf12nc[ (4*i) + j ] = ( ( fourchars >> ( ( 3-j ) * 4 ) ) & 0xf ) + '0';
    }
    buf12nc[12] = '\0';
  }
  return( buf12nc );
}

void nvm_detecttest( void ) {
  // Assumes that sensor is active
  
//  // SEN200/01 with 12nc 9353 006 43115 becomes
//  sensor_writeapb( 0x20, 0x0200 );   // Type
//  sensor_writeapb( 0x21, 0x0001 );   // Subtype
//  sensor_writeapb( 0x22, 0x9353 );
//  sensor_writeapb( 0x23, 0x0064 );
//  sensor_writeapb( 0x24, 0x3115 );
  
  // SEN200/01 with 12nc 9353 006 43115 becomes
  sensor_writeapb( 0x20, 0x0300 );   // Type
  sensor_writeapb( 0x21, 0x0001 );   // Subtype
  sensor_writeapb( 0x22, 0x9353 );
  sensor_writeapb( 0x23, 0x9915 );
  sensor_writeapb( 0x24, 0x4115 );
}

// ============================================================================
// TEMP SPECIFIC FUNCTIONS
// ============================================================================

/**
* Set-up the TEMP sensor.
* - Returns the (expected) conversion time when started (or -1 on error).
*/
int sensor_temp_setsensor( int numofbits ) {
    // conversion t  3  4  5  6  7   8   9   10 bits
    int delays[] = { 0, 0, 0, 0, 2, 15, 50, 100 };
    if ( numofbits >= 3 && numofbits <= 10 ) {
      int sreg0 = ( ( numofbits - 3 ) << 1 ) | 0x01;  // Voltage calib disabled, n bits, calibrated value
      if ( sensor_sensorsetup( SENSOR_TEMP, REG_SREG0, sreg0 ) )
        return( delays[numofbits-3] );
    }
  return( -1 );
}

/**
* Converts the TEMP sensor value into degrees Celcius (as a float).
*/
float sensor_temp_convertdatatotemp( int data ) {
  int negative = ( data & 0x8000 );
  data &= 0x7FFF;
  float temp = (float)data / 64.0;  // 0x40;
  if ( negative ) temp = -temp;
  temp -= 273.15;
  return( temp );
}

// ============================================================================
// RH SPECIFIC FUNCTIONS
// ============================================================================

#define CALIB_OFFSET_RH   4
uint16_t c0[4], c1[4], c2[4], c3[4];

void sensor_rh_readcalib( int RhSensor ) {
  int addr = 0x40 + CALIB_OFFSET_RH + (4*RhSensor);
  sensor_readnvm( addr + 0, &c0[RhSensor] );
  sensor_readnvm( addr + 1, &c1[RhSensor] );
  sensor_readnvm( addr + 2, &c2[RhSensor] );
  sensor_readnvm( addr + 3, &c3[RhSensor] );
}

/**
* Set-up the RH sensor.
* - Assumes that calib parameters have been read into c0, c1, c2, c3
* - Returns the (expected) conversion time when started (or -1 on error).
* - For MIST1431, float conversion needs to take place from ftemp
* - For SenX00, the raw temp value is used without extra conversion
*/
int sensor_rh_setsensor( int RhSensor, float ftemp, int raw_temp ) {
  
  if ( mode == MODE_MIST1431 ) {
    sensor_sensorsetup( SENSOR_RH, REG_SREG0, 0xec07 ); // Resolution setup
  
    sensor_sensorsetup( SENSOR_RH, REG_SREG1, c0[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG2, c1[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG3, c2[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG4, c3[RhSensor] );  // Calib
  
    if ( ftemp < -200.0 ) ftemp = 25;        // If temp not available, then use 25 degrees Celsius
    sensor_sensorsetup( SENSOR_RH, REG_SREG5, HalfCompress( ftemp + 273.15 ) );  // Temp
  } else {
    sensor_sensorsetup( SENSOR_RH, REG_SREG0, c0[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG1, c1[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG3, c2[RhSensor] );  // Calib
    sensor_sensorsetup( SENSOR_RH, REG_SREG4, c3[RhSensor] );  // Calib

    if ( raw_temp == 0 ) raw_temp = 0x5CA8;  // If temp not available, then use 25 degrees Celsius
    sensor_sensorsetup( SENSOR_RH, REG_SREG5, raw_temp );  // Temp
  }
  
  sensor_sensorsetup( SENSOR_RH, REG_SREG6, ( 1 << RhSensor ) );  // Select
  
  return( 18 );
}

/**
* Converts the RH sensor value into % (as an integer).
* - Returns a value between [0-100%] (normally)
* - For MIST1431 and SenX00 different conversions are needed
*/
float sensor_rh_convertdatatohumidity( int RhSensor, int data ) {
  float rh;
  if ( mode == MODE_MIST1431 ) {
    rh = HalfDecompress( data );
  } else {
    if ( c0[RhSensor] & 0x20 ) {
      // Raw format
      rh = (float)data;     // Unknown what this means
    } else {
      if ( c0[RhSensor] & 0x10 ) {
        // unlimited, calibrated
        int sign = ( data & 0x8000 ) ? -1 : 1;
        data &= 0x7FFF;
//        rh = data >> 8;
        rh = (float)( data >> 8 );
        rh += (float)( data & 0xFF ) / 256.0f;
        rh *= sign;
      } else {
        // Limited, calibrated
//        rh = data >> 9;        
        rh = (float)( data >> 9 );
        rh += (float)( data & 0x1FF ) / 512.0;
      }
    }
  }
  
  sprintf( buf, "Rh-raw (%d) 0x%x", RhSensor, data );
  //modem_debug( buf );
  console_uart_sendString( buf );
  
  return( rh );
}

// ============================================================================
// ALS SPECIFIC FUNCTIONS
// - We have 12 sensors:
//   0 -  5: Red + Blue sensor
//   6 - 11: Red sensor only
// ============================================================================

#define CALIB_OFFSET_ALS_MIST_R  0x54

#define CALIB_OFFSET_ALS_SEN_OP  0x54
#define CALIB_OFFSET_ALS_SEN_R   0x66      // R
#define CALIB_OFFSET_ALS_SEN_RB  0x69      // R+B

uint16_t m[6], n[6], opmode[6], int_time[6], int_cal[6];
uint16_t defaultm[6]={0x56d,0x56d,0x502,0x501,0x513,0x4ee};

/**
* Read calibration parameters into global variables m, n, opmode, int_time and int_cal
* - NVM address differs per sensor
*/
void sensor_als_readcalib( int AlsSensor ) {
  int addr;
  int rb = 1;
  if ( AlsSensor >= 6 ) { rb = 0; AlsSensor -= 6; }   // 0-5 = R+B; 6-11 = RED
  
  if ( mode != MODE_MIST1431 ) {
    addr = CALIB_OFFSET_ALS_SEN_OP;
    addr += ( 3 * AlsSensor );
    sensor_readnvm( addr + 0, &opmode[AlsSensor] );
    if (!opmode[AlsSensor]) {
      opmode[AlsSensor]=0xe1;
      sprintf( buf, "Opmode not found, use default %x", opmode[AlsSensor] );
      //modem_debug( buf );
      console_uart_sendString( buf );
    }
    sensor_readnvm( addr + 1, &int_time[AlsSensor] );
    sensor_readnvm( addr + 2, &int_cal[AlsSensor] );
    if (!int_cal[AlsSensor]) {
      int_cal[AlsSensor]=0x09;
      sprintf( buf, "Cal not found, use default %x", int_cal[AlsSensor] );
      //modem_debug( buf );
      console_uart_sendString( buf );
    }
  }
  
  if ( mode == MODE_MIST1431 ) addr = CALIB_OFFSET_ALS_MIST_R;
  else addr = CALIB_OFFSET_ALS_SEN_R;

  if ( rb ) addr += 3;   // R+B values are 3 positions further
  
  addr += ( 6 * AlsSensor );
  sensor_readnvm( addr + 1, &m[AlsSensor] );
  if (!m[AlsSensor]) {
    m[AlsSensor]=defaultm[AlsSensor];
    sprintf( buf, "M not found, use default %d", m[AlsSensor] );
    //modem_debug( buf );
    console_uart_sendString( buf );
  }
  sensor_readnvm( addr + 0, &n[AlsSensor] );
  if (!n[AlsSensor]) {
    n[AlsSensor]=1;
    sprintf( buf, "N not found, use default %d", n[AlsSensor] );
    //modem_debug( buf );
    console_uart_sendString( buf );
  }
}

/**
* Set-up the ALS sensor.
* - Returns the (expected) conversion time when started (or -1 on error).
*/

//                        0   1   2    3    4    5    6    7
int integrationtime[] = { 16, 16, 20, 100, 200, 400, 400, 400 };

int sensor_als_setsensor( int AlsSensor ) {
  int sreg0;
  int rb = 1;
  if ( AlsSensor >= 6 ) { rb = 0; AlsSensor -= 6; }
  
  if ( mode == MODE_MIST1431 ) {
    // sreg0 = 0xe1;  // Enable diode, gain=1, start
    sreg0 = 0xe1;  // Enable diode, gain=1, start
    // sreg0 = 0xe5;  // Enable diode, gain=1, start
    // sreg0 = 0xe3;
    // sreg0 = 0xa3;  // Enable diode, gain=1, start
  } else {
    // sreg0 = 0xE1;   // 11100011
    sreg0 = opmode[AlsSensor];   // 11100011
  }
  sensor_sensorsetup( SENSOR_ALS, REG_SREG0, sreg0 );
  
  if ( mode != MODE_MIST1431 ) {
    // sensor_sensorsetup( SENSOR_ALS, REG_SREG1, 0x000d );
    // sensor_sensorsetup( SENSOR_ALS, REG_SREG2, 0x009d );
    sensor_sensorsetup( SENSOR_ALS, REG_SREG1, int_time[AlsSensor] );
    sensor_sensorsetup( SENSOR_ALS, REG_SREG2, int_cal[AlsSensor]  );
  }
  
  if ( mode == MODE_MIST1431 ) {
    // Due to a bug in the current silicon, the ALS diode mux has been reversed
    AlsSensor = 5 - AlsSensor;
  }
  
  int sreg6 = ( 1 << AlsSensor );
  if ( rb ) sreg6 |= 0x40;
  sensor_sensorsetup( SENSOR_ALS, REG_SREG6, sreg6 );
  
  // Return integration time
  return( integrationtime[ sreg0 & 0x07 ] );
}

/**
* Convert the ALS sensor value into a LUX value.
* - Assumes that calib parameters have been read
*/

////                0      1     2     3      4       5      6
//// Int.times:   notsup. 16ms  20ms  100ms  200ms  400ms  notsup.
//float if2h[] = {   1.0, 32.1, 38.5, 192.5,  385,   760,   1.0 };
//float if2n[] = {   1.0,  1.0,  1.2,  5.9,  11.9,  23.8,   1.0 };
//
//float if1h[] = {   1.0, 10.2, 14.3, 89.7,   198,   358,   1.0 };
//float if1n[] = {   1.0,  1.0,  1.2,  3.4,   6.5,  16.9,   1.0 };

//                0        1        2        3         4         5       6
// Int.times:   notsup.  16ms      20ms    100ms     200ms     400ms   notsup.
float if2h[] = {   1.0,  1.0,     1.2007,  6.0051,  12.0056,  23.8805,  1.0 };
float if2n[] = {   1.0,  0.0267,  0.0321,  0.1606,   0.3215,  0.6427,   1.0 };

float if1h[] = {   1.0,  1.0,     1.1955,  5.8351,  11.7510,  25.2164,  1.0 };
float if1n[] = {   1.0,  0.0373,  0.0431,  0.1692,   0.3245,   0.6444,  1.0 };

float sensor_als_convertdatatolux( int AlsSensor, int data ) {
  float lux;
  int rb = 1;
  if ( AlsSensor >= 6 ) { rb = 0; AlsSensor -= 6; }
  
  //if ( m[AlsSensor] == 0 || n[AlsSensor] == 0 ) modem_error( "no als m+n" );
  if ( m[AlsSensor] == 0 || n[AlsSensor] == 0 ) console_uart_sendString(  "no als m+n" );
  
  int ls_gain = ( opmode[AlsSensor] & ( 1 << 6 ) ) != 0;
  int ls_time = opmode[AlsSensor] & 0x07;
  
  sprintf( buf, "M=%d, N=%d, G=%d, T=%d", m[AlsSensor], n[AlsSensor], ls_gain, ls_time );
  //modem_debug( buf );
  console_uart_sendString( buf );

  // Old formula
  //  lux = (data * 8750) / m[AlsSensor] + n[AlsSensor];
  //  sprintf( buf, "Old lux %d", (int)lux );
  //  modem_debug( buf );

  // New formula
  // lux = (float)(data * 11997) / (float)m[AlsSensor];
  lux = (float)(data * 1200) / (float)m[AlsSensor];
  
  if ( ls_gain == 0 ) {
    // Bit6 is 0: HIGH gain
    lux -= (float)n[AlsSensor] * if1h[ls_time];
    lux /= if2h[ls_time];
  } else {
    // Bit6 is 1: NORMAL gain
    lux -= (float)n[AlsSensor] * if1n[ls_time];
    lux /= if2n[ls_time];
  }
  
  return( lux );
}

// ============================================================================
// Startup / switchoff
// ============================================================================

/**
* Performs standard startup sequence
*/
void sensor_active( void ) {
  // Reset MIST
  sensor_hardreset( 1 );
  sensor_hardreset( 0 );
  
  // Reset MIST (again) in SW
  sensor_command_reset();
  
  // Enable analog power to sensors
  sensor_analogpower( 1 );
}

/**
* Performs standard switchoff sequence
*/
void sensor_inactive( void ) {
  // Stop and put MIST in reset (to save power)
  sensor_analogpower( 0 );
  sensor_hardreset( 1 );
}

// ============================================================================
// Testing
// ============================================================================

//int sensor_testenable( void ) {
//  int cmd = ( CMD_TEST_ENABLE << 8 ) | 0;
//  int response = 0;
////  if ( sensor_sendcommand2( cmd, 0xff, 0xff, &response ) ) {
//  if ( sensor_sendcommand2( cmd, 0x00, 0x00, &response ) ) {
//    sprintf( buf, "Test enable: %s", response2text(response) );
//    modem_debug( buf );
//  } else {
//    sprintf( buf, "Test enable: %s", response2text(response) );
//    modem_debug( buf );
//  }
//  return( response == RSP_ACK );
//}
//
//int sensor_testdisable( void ) {
//  int cmd = ( CMD_TEST_DISABLE << 8 ) | 0;
//  int response = 0;
//  if ( sensor_sendcommand( cmd, &response ) ) {
//    sprintf( buf, "Test disable: %s", response2text(response) );
//    modem_debug( buf );
//  }
//  return( response == RSP_ACK );
//}

int sensor_testmodeunlocked( void ) {
  return( ( factory_status & ( 1 << 3 ) ) == 0 );
}

// ============================================================================
// Init
// ============================================================================

void sensor_init( void ) {
  int response;
  PINSEL_CFG_Type PinCfg;

  /*pins to be used :
   * P0.9 MOSI1 -> P5
   * P0.8 MISO1 -> P6
   * P0.7 SCK1  -> P7
   * P0.6 SSEL1 -> P8
   * P0.18 INT -> P11 //my board
   * P0.17 ENA -> P12 //my board
   * P0.0 INT -> P9 //Stefan board
   * P0.1 ENA -> P10 //Stefan board
   */
  PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = 0;

	PinCfg.Funcnum = 3;
	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
#ifdef STEFAN_BOARD
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
#else
	PinCfg.Funcnum = 0;
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&PinCfg);
/*
	//just check if we can play around with the LEDs
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, 1L<<15 , 1);
	GPIO_SetDir(0, 1L<<16 , 1);
	GPIO_ClearValue(0,(1L<<15)+(1L<<16));
	GPIO_SetValue(0,(1L<<15)+(1L<<16));
*/
#endif


  sensor_spiconfigure();
#if 0
  CLKOUTCLKSEL=0x03; // select main clock as src
  CLKOUTUEN=0x00; // apply clock selection
  CLKOUTUEN=0x01;
  CLKOUTDIV=0; // disable it for now
  IOCON_PIO0_1 = 0x01; // make P0.1 clkout pin
#endif


  sensor_active();
  sensor_sendcommand( CMD_SENSOR_MEASURE_DEFAULT << 8 | 0x10, &response );

  // mode=(response!=0x5555) ? MODE_SEN300 : MODE_MIST1431; 

  if ( response == 0x5555 ) {
    mode = MODE_MIST1431;
  } else {
    // Unprogrammed (0000h, FFFFh) and !0200h == SEN300
    if ( nvm_readtype() != 0x0200 ) mode = MODE_SEN300;
    else mode = MODE_SEN200;
  }
  
  sensor_inactive();
  
  // TODO
  // - Also distinguish between SEN200 and SEN300 by calling ALS function ....
}
