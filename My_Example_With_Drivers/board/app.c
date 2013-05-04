// app.c - SenDemokit on lpc1343
// 2012 nov 20  v1    Ruud Hendricksen  Created
// 2013 jan 28  v1.1  Boris Cobelens    Fixed IR LED control and MIST clock (sensor.c, app.c)

#define ASSERT_MODNAME "SenDemokit/app"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>



#include "sensor.h"
#include "board.h"
#include "FreeRTOS.h"
#include "queue.h"
#define MAX_UARTMESS_LEN     256

#define TYPE_TEMP            1
#define TYPE_HUMIDITY        2
#define TYPE_ALS             3

#define APP_NAME             "LPC 17xx AOA Demo"
#define QUEUEWAIT_MS				 1000
// --------------------------------------------------------------------------
// Global variables
// --------------------------------------------------------------------------

char uart_message_in[MAX_UARTMESS_LEN+4];

// --------------------------------------------------------------------------
// Settings
// --------------------------------------------------------------------------

int verbose = 0, units = 0;

float ftemp = -273.15;
float als;
float humidity;

int   raw_temp_data, raw_rh_data, raw_als_data;
char  buf[40];

int CurAlsSensor, CurRhSensor;

int nvmindex = -1;
//Global Queues
extern xQueueHandle queuToLCD ;
extern xQueueHandle queuToUSB ;

// --------------------------------------------------------------------------
// Float printing
// --------------------------------------------------------------------------

int printfloat(char *buf, float fval )
{
  char * start = buf;
    int  ival;
    float  fraction;
    
    if (fval < (float)0.0) {
        // Sign
        *(buf++) = '-';
        fval = -fval;    // make fval positive
    }

    // Split off integer part and print
    ival     = (int)fval;
    fraction = fval - (float)ival;
    buf     += sprintf( buf, "%d", ival );
    
    if ( fraction > (float)0.0 ) {
      // Decimal point
      *(buf++) = '.';
      *(buf)   = '\0';
      
      int maxfrac = 8;
      do {
        // Take next digit behind the comma and print
        fraction *= (float)10.0;
        ival = (int)fraction;
        buf += sprintf( buf, "%d", ival );
        
        // Substract and test
        fraction -= (float)ival;
      } while ( ( --maxfrac > 0 ) && ( fraction > (float)0.0 ) );
      
    }

    return( buf - start );
}

// --------------------------------------------------------------------------
// Send Measurements
// --------------------------------------------------------------------------

void sendTemp( float ftemp ) {  
//  if ( units ) 
//       sprintf( buf, "%d.%03d C", (int)ftemp, (int)(ftemp * 1000) % 1000 );
//  else sprintf( buf, "%d.%03d",   (int)ftemp, (int)(ftemp * 1000) % 1000 );
  int len = printfloat( buf, ftemp );
  if ( units ) sprintf( buf+len, " C" );
  console_uart_sendString("\r\nTemp:");
  console_uart_sendString(buf);


}

void sendAls( int sensor, float als ) {  
  static char token[6];
  sprintf( token, "als%d", sensor );
  int len = printfloat( buf, als );
  if ( units ) sprintf( buf+len, " Lux" );
  console_uart_sendString("\r\nLux:");
  console_uart_sendString(buf);


}

void sendRh( int sensor, float humidity ) {  
  static char token[6];
  sprintf( token, "rh%d", sensor );
//  if ( units ) sprintf( buf, "%d.%d %%", humidity/10, humidity%10 );
//  else sprintf( buf, "%d.%d", humidity/10, humidity%10 );
  int len = printfloat( buf, humidity );
  if ( units ) sprintf( buf+len, " %%" );
  console_uart_sendString("\r\nhumidity:");
  console_uart_sendString(buf);
}

// --------------------------------------------------------------------------
// One Automatic Full Mist Reading
// --------------------------------------------------------------------------

//void auto_full() {
void vSensorTask1( void *pvParameters ){
  
  float ftemp        = -273.15;
  float humidity     = 0;
  float als          = 0;
  int raw_temp_data;
  int raw_als_data;
  int raw_rh_data;
  int RhSensor;
  int AlsSensor;

  // Activate Sensor
 // sensor_active();
  
  while(1){
	  // Power TEMP and start conversion: takes 100 ms
	  sensor_sensorpower( SENSOR_TEMP, 1 );
	  sensor_temp_setsensor( 10 );
	  sensor_sensorstart( SENSOR_TEMP );

	  // Wait some time
	  //rtk_tsk_sleep( 100 );                                             // 80
	  vTaskDelay(100);

	  // Mist temp should (could) be ready now...
	  if ( sensor_waitforconversion( SENSOR_TEMP, 100 ) ) {
		sensor_readdata( SENSOR_TEMP, &raw_temp_data );
		ftemp = sensor_temp_convertdatatotemp( raw_temp_data );
		sendTemp( ftemp);

		// Unpower temp
		sensor_sensorpower( SENSOR_TEMP, 0 );
	  }

	  // 4x RH
	  // Power RH
	  sensor_sensorpower( SENSOR_RH, 1 );

	  for (RhSensor=0; RhSensor<4; RhSensor++ ) {

		// Read calibration data for Rh
		// - takes about 16 ms
		// - used in RhSetSensor
		sensor_rh_readcalib( RhSensor );

		// Start conversion: takes 20 ms
		sensor_rh_setsensor( RhSensor, ftemp, raw_temp_data );
		sensor_sensorstart( SENSOR_RH );

		//rtk_tsk_sleep( 20 );
		vTaskDelay(20);

		// Wait until RH is ready
		if ( sensor_waitforconversion( SENSOR_RH, 50 ) ) {
		  sensor_readdata( SENSOR_RH, &raw_rh_data );
		  humidity = sensor_rh_convertdatatohumidity( RhSensor, raw_rh_data );
		  sendRh( RhSensor, humidity );
		}
	  }
	  // Unpower RH
	  sensor_sensorpower( SENSOR_RH, 0 );

	  if ( mode != MODE_SEN200 ) {
		// 12x ALS
		// Power ALS
		sensor_sensorpower( SENSOR_ALS, 1 );

		for (AlsSensor=0; AlsSensor<12; AlsSensor++ ) {

		  // Read calibration data for Als
		  // - takes about 16 ms
		  sensor_als_readcalib( AlsSensor );

		  // Start conversion: takes 16 ms when using e1
		  // - SENx00: must be called after reading calibration
		  int integrationtime = sensor_als_setsensor( AlsSensor );
		  sensor_sensorstart( SENSOR_ALS );
		  raw_als_data = 1;

		  // Wait for the programmed integrationtime
		  //rtk_tsk_sleep( integrationtime );
		  vTaskDelay(integrationtime * 100); //each tick is 1 us

		  if ( sensor_waitforconversion( SENSOR_ALS, 500 ) ) {
			sensor_readdata( SENSOR_ALS, &raw_als_data );
			als = sensor_als_convertdatatolux( AlsSensor, raw_als_data );
			sendAls( AlsSensor, als );
		  }
		}
		// Unpower ALS
		sensor_sensorpower( SENSOR_ALS, 0 );
	  } else {
		for (AlsSensor=0; AlsSensor<12; AlsSensor++ ) sendAls( AlsSensor, 0 );
	  }
	  vTaskDelay(100);
  }
  // Switch off Sensor
  //sensor_inactive();
}

// --------------------------------------------------------------------------
// One Automatic Temp Mist Reading
// --------------------------------------------------------------------------

void auto_temp( void ) {
  
  float ftemp = -273.15;
  int raw_temp_data;
  
  // Activate Sensor
  //sensor_active();
  
  // Power TEMP and start conversion
  sensor_sensorpower( SENSOR_TEMP, 1 );
  sensor_temp_setsensor( 10 );
  sensor_sensorstart( SENSOR_TEMP );
  
  // Wait some time
  //rtk_tsk_sleep( 80 );
  vTaskDelay(8);
  // Mist temp should (could) be ready now...
  if ( sensor_waitforconversion( SENSOR_TEMP, 100 ) ) {
  
    sensor_readdata( SENSOR_TEMP, &raw_temp_data );
    ftemp = sensor_temp_convertdatatotemp( raw_temp_data );  
    sendTemp( ftemp);
  
    // Unpower temp
    sensor_sensorpower( SENSOR_TEMP, 0 );
  }
  
  // Switch off Sensor
 // sensor_inactive();
}

// --------------------------------------------------------------------------
// One Automatic ALS Mist Reading
// --------------------------------------------------------------------------

void auto_als( int AlsSensor ) {
  
  float als = 0;
  int raw_als_data;
  
  if ( mode != MODE_SEN200 ) {
    
    // Activate Sensor
   // sensor_active();
    
    // Power ALS
    sensor_sensorpower( SENSOR_ALS, 1 );
      
    // Read calibration data for Als
    // - takes about 16 ms
    sensor_als_readcalib( AlsSensor );
      
    // Start conversion: takes 16 ms when using e1
    // - SENx00: must be called after reading calibration
    int integrationtime = sensor_als_setsensor( AlsSensor );
    sensor_sensorstart( SENSOR_ALS );
    raw_als_data = 1;
      
    // Wait the programmed integrationtime
    //rtk_tsk_sleep( integrationtime );
    vTaskDelay(integrationtime * 100); //each tick is 1 us
    
    // ALS should (could) be ready now...
    if ( sensor_waitforconversion( SENSOR_ALS, 200 ) ) {
      sensor_readdata( SENSOR_ALS, &raw_als_data );
      als = sensor_als_convertdatatolux( AlsSensor, raw_als_data );    
      sendAls( AlsSensor, als );      
      
      // Unpower ALS
      sensor_sensorpower( SENSOR_ALS, 0 );
    }
    
    // Switch off Sensor
    //sensor_inactive();
  } else {
      sendAls( AlsSensor, 0 );
  }
}

// --------------------------------------------------------------------------
// One Automatic RH Mist Reading
// --------------------------------------------------------------------------

void auto_rh( int RhSensor ) {
  
  float ftemp        = -273.15;
  float humidity     = 0;
  int raw_temp_data;
  int raw_rh_data;
  

  //sensor_init();
  // Activate Sensor
  //sensor_active();

  // Power TEMP and start conversion
  sensor_sensorpower( SENSOR_TEMP, 1 );
  sensor_temp_setsensor( 10 );
  sensor_sensorstart( SENSOR_TEMP );

  // Read calibration data for Rh
  // - takes about 16 ms
  // - used in RhSetSensor
  sensor_rh_readcalib( RhSensor );
    
  // Wait some time
  //rtk_tsk_sleep( 80 );
  vTaskDelay(8); //each tick is 1 us
  
  // Mist temp should (could) be ready now...
  if ( sensor_waitforconversion( SENSOR_TEMP, 100 ) ) {
  
    sensor_readdata( SENSOR_TEMP, &raw_temp_data );
    ftemp = sensor_temp_convertdatatotemp( raw_temp_data );  
    // sendTemp( ftemp);
    
    
    // Temp ready, to be used for RH measurement
    // Power RH and start conversion: takes 20 ms, so back in (this + 2)
    sensor_sensorpower( SENSOR_RH, 1 );
    sensor_rh_setsensor( RhSensor, ftemp, raw_temp_data );
    sensor_sensorstart( SENSOR_RH );
  
    // Wait some time (20 ms)
    //rtk_tsk_sleep( 20 );
    vTaskDelay(2); //each tick is 1 us
  
    // Wait until RH is ready
    if ( sensor_waitforconversion( SENSOR_RH, 50 ) ) {
      sensor_readdata( SENSOR_RH, &raw_rh_data );
      humidity = sensor_rh_convertdatatohumidity( RhSensor, raw_rh_data );
      sendRh( RhSensor, humidity );
    }
  }

  // Unpower temp and RH
  sensor_sensorpower( SENSOR_TEMP, 0 );
  sensor_sensorpower( SENSOR_RH, 0 );
  
  // Switch off Sensor
  //sensor_inactive();
}

// ============================================================================
// NVM FUNCTION
// ============================================================================

void nvm_dump( void ) {
  uint16_t nvm; 
  int i;
  // Activate Sensor
  //sensor_active();

  // Internal bank
  if ( sensor_testmodeunlocked() ) {
    for (i=0; i<16; i++ ) {
      sensor_readapb( i, &nvm );
      sprintf( buf, "%02x: 0x%04x", i, nvm );
      //modem_send_text( "nvm-i", buf );
      console_uart_sendString("\r\nnvm-i:");
      console_uart_sendString(buf);
    }
  } else {
    //modem_debug( "Internal bank is locked" );
	  console_uart_sendString("Internal bank is locked" );
  }

  // Datasheet bank
  for (i=0; i<5; i++ ) {
    sensor_readnvm( 0x20 + i, &nvm );
    sprintf( buf, "%02x: 0x%04x", 0x20+i, nvm );
    //modem_send_text( "nvm", buf );
    console_uart_sendString("\r\nnvm:");
    console_uart_sendString(buf);
  }
  
  // Calibration bank
  int len = ( mode == MODE_MIST1431 ) ? 56 : 80;
  for (i=0; i<len; i++ ) {
    sensor_readnvm( 0x40 + i, &nvm );
    sprintf( buf, "%02x: 0x%04x", 0x40+i, nvm );
    //modem_send_text( "nvm", buf );
    console_uart_sendString("\r\nnvm:");
    console_uart_sendString(buf);
  }
  
  // Switch off Sensor
  //sensor_inactive();
}

void nvm_read_reg( int i ) {
  uint16_t nvm = -1; 
  
  int len = ( mode == MODE_MIST1431 ) ? 56 : 80;

  // Activate Sensor
  //sensor_active();
  
  if ( i < 16 ) {
    // Internal bank
    if ( sensor_testmodeunlocked() ) {
      sensor_readapb( i, &nvm );
    }
  } else if ( i >= 0x40 && i < ( 0x40 + len ) ) {
    sensor_readnvm( i, &nvm );
  }
  
  sprintf( buf, "%02x: 0x%04x", i, nvm );
  //modem_send_text( "nvm", buf );
  console_uart_sendString("\r\nnvm:");
  console_uart_sendString(buf);
  
  // Switch off Sensor
 // sensor_inactive();
}

void nvm_write_reg( int i, int val ) {
  // Activate Sensor
  //sensor_active();

  int len = ( mode == MODE_MIST1431 ) ? 56 : 80;
  if ( i >= 0x40 && i < ( 0x40 + len ) )
    sensor_writenvm( i, val );
  
  // Switch off Sensor
  //sensor_inactive();
}

// ============================================================================
// SENX00 SPECIFIC FUNCTIONS
// ============================================================================

void sen_prog( void ) {
	int i;
  if ( mode != MODE_MIST1431 ) {
  
    // Activate Sensor
    //sensor_active();
    
    // Temp
    sensor_writenvm( 0x40, 0x000F );
    sensor_writenvm( 0x41, 0x9601 );
    sensor_writenvm( 0x42, 0xFC47 );
    sensor_writenvm( 0x43, 0x4001 );
    
    // RH
    for (i=0; i<4; i++ ) {
      sensor_writenvm( 0x44 + ( i * 4 ), 0xEC17 );
      sensor_writenvm( 0x45 + ( i * 4 ), 0x4F95 );
      sensor_writenvm( 0x46 + ( i * 4 ), 0xC180 );
      sensor_writenvm( 0x47 + ( i * 4 ), 0x0F44 );
    }
  
    // ALS
    for (i=0; i<6; i++ ) {
      sensor_writenvm( 0x54 + ( i * 3 ), 0x00e1 );
      sensor_writenvm( 0x55 + ( i * 3 ), 0x000d );
      sensor_writenvm( 0x56 + ( i * 3 ), 0x009d );
    }
    for (i=0; i<6; i++ ) {
      sensor_writenvm( 0x66 + ( i * 6 ), 0x0004 );
      sensor_writenvm( 0x67 + ( i * 6 ), 0x102e );
      sensor_writenvm( 0x68 + ( i * 6 ), 0x0f34 );
      sensor_writenvm( 0x69 + ( i * 6 ), 0x0001 );
      sensor_writenvm( 0x6A + ( i * 6 ), 0x06ff );
      sensor_writenvm( 0x6B + ( i * 6 ), 0x05ed );
    }
    
    // Switch off Sensor
    //sensor_inactive();
  }
  
  nvm_dump();
}

int default_sensor( int type, int sensor ) {
  int response = 0;
  int defsensor = sensor;
  
  // Activate Sensor
  //sensor_active();
  
  if ( mode != MODE_MIST1431 ) {
  
    switch ( type ) {
    case TYPE_TEMP:     defsensor += 0x10; break;
    case TYPE_HUMIDITY: defsensor += 0x20; break;
    case TYPE_ALS:
	if ( sensor < 6 ) defsensor += 0x38;        // R+B
	else              defsensor += 0x30 - 6;    // RED
	break;
    }
  
    int cmd = ( CMD_SENSOR_MEASURE_DEFAULT << 8 ) | defsensor;
    if ( sensor_sendcommand( cmd, &response ) ) {
      if ( response == RSP_ILLEGAL_SENX00 ) return( 0 );
      sprintf( buf, "Sensor 0x%02x = 0x%x", defsensor, response );
      //modem_debug( buf );
      console_uart_sendString(buf);
    }
#if 0
    // Read calib parameters in case of rh (needed for conversion)
    if ( type == TYPE_HUMIDITY ) {
      sensor_rh_readcalib( sensor );
    }
    
    // Read calib parameters in case of als (needed for conversion)
    if ( type == TYPE_ALS ) {
      sensor_als_readcalib( sensor );       // All cases
    }
#endif
  }
  
  // Switch off Sensor
  //sensor_inactive();

  return( response );
}

void default_temp() {
  if ( mode != MODE_MIST1431 ) {
    raw_temp_data = default_sensor( TYPE_TEMP, 0 );
    ftemp = sensor_temp_convertdatatotemp( raw_temp_data );  
    sendTemp( ftemp);
  }
}

void default_rh( int sensor ) {
  if ( mode != MODE_MIST1431 ) {
    raw_rh_data = default_sensor( TYPE_HUMIDITY, sensor );
    humidity = sensor_rh_convertdatatohumidity( sensor, raw_rh_data );
    sendRh( sensor, humidity );
  }
}

void default_als( int sensor ) {
  if ( mode == MODE_SEN300 ) {
    raw_als_data = default_sensor( TYPE_ALS, sensor );
    als = sensor_als_convertdatatolux( sensor, raw_als_data );    
    sendAls( sensor, als );
  } else {
    sendAls( sensor, 0 );
  }
}

//void auto_sen( void ) {
void vSensorTask( void *pvParameters ){
	  uint32_t i;
	  uint32_t j;
	  int  raw_temp_data ;
	  int  raw_humi_data ;
	  int  raw_lux_data ;
	 uint8_t BuffIn[2];
	 uint8_t BuffOut[2];
	 int Read16 ;
	 int cmd,n,m ;
	 int negative;
	 float temp;
	  float humi;
	 sensor_readings readings ;
	    float lux;
	 console_uart_sendString("\r\nsensor_init\r\n");
	 sensor_init();
	 //console_uart_sendString("\r\nsensor_active");
	 //sensor_active();
#if 1
while(1){

	 // send read T command
	  BuffOut[0] = 0x20;
	  BuffOut[1] = 0x10;
	  //temp =( (((int)BuffIn[0])<<8) + (int)BuffIn[1])>>6 ;
	  cmd = (BuffOut[0] << 8) + BuffOut[1] ;
	  sensor_sendcommand(cmd,&Read16);


	// Convert
	  raw_temp_data = Read16;
	  negative = (raw_temp_data & 0x8000 );
	  raw_temp_data &= 0x7FFF;
	  temp = (float) raw_temp_data / 0x40;
	  if ( negative ) temp = -temp;
	  temp -= 273;

      sprintf( buf, "\r\nTemp = %f ", temp );
      //modem_debug( buf );
      console_uart_sendString(buf);


	  vTaskDelay(1);


	// read humidity
	  BuffOut[0] = 0x20;
	  BuffOut[1] = 0x20;
	  cmd = (BuffOut[0] << 8) + BuffOut[1] ;
	  sensor_sendcommand(cmd,&Read16);


	// Convert
	  raw_humi_data = Read16;


	  humi = (float) raw_humi_data / 512;
      sprintf( buf, "\r\nhumidity = %f ", humi );
      //modem_debug( buf );
      console_uart_sendString(buf);
	  vTaskDelay(1);


	  // read lux
	    BuffOut[0] = 0x20;
	    BuffOut[1] = 0x30;
		  cmd = (BuffOut[0] << 8) + BuffOut[1] ;
		  sensor_sendcommand(cmd,&Read16);
	    //temp =( (((int)BuffIn[0])<<8) + (int)BuffIn[1])>>6 ;
	    raw_lux_data = Read16;


	    // read n param
	    BuffOut[0] = 0x15;
	    BuffOut[1] = 0x69;
		  cmd = (BuffOut[0] << 8) + BuffOut[1] ;
		  sensor_sendcommand(cmd,&Read16);
	    n = Read16;

	    // read m param
	    BuffOut[0] = 0x15;
		BuffOut[1] = 0x6A;
		  cmd = (BuffOut[0] << 8) + BuffOut[1] ;
		  sensor_sendcommand(cmd,&Read16);
		  m = Read16;

		//convert

	     lux  = ( ((float)raw_lux_data * 11997 / (float)m)  - (float)5.9 * n) / 3.4;

	      sprintf( buf, "\r\nlux = %f ", lux );
	      //modem_debug( buf );
	      console_uart_sendString(buf);

	     vTaskDelay(500);
	     extern xQueueHandle queuToLCD ;
	     extern xQueueHandle queuToUSB ;
	     readings.hum = humi ;
	     readings.lux = lux ;
	     readings.temp = temp ;
	    //xQueueSend(queuToLCD,&readings,portMAX_DELAY);
	    //xQueueSend(queuToUSB,&readings,portMAX_DELAY);
	    xQueueSend(queuToLCD,&readings,QUEUEWAIT_MS*portTICK_RATE_MS);
	    xQueueSend(queuToUSB,&readings,QUEUEWAIT_MS*portTICK_RATE_MS);




}
#endif
/*  default_temp();
  for (i=0; i<4; i++ ) {
    default_rh( i );
  }
  for (i=0; i<12; i++ ) {
    default_als( i );
  }
*/
}

#if 0
// --------------------------------------------------------------------------
// IR LED control
// --------------------------------------------------------------------------

void setledpwm(int nr, int intensity)
{
  switch (nr) {
    case 0:
	if (intensity) {
	  IOCON_PIO1_10 = 0x02;
	  TMR16B1MR1 = (100-intensity)*10; 
	} else {
	  IOCON_PIO1_10 = 0; 		
	  GPIO1DIR |= 1 << 10;							
	  GPIO1DATA &= ~(1 << 10); 
	}
        break;

    case 1:
	if (intensity) {
	  IOCON_JTAG_TDI_PIO0_11 = 0x03;		
	  TMR32B0MR3 = (100-intensity)*10; 
	} else {
	  IOCON_JTAG_TDI_PIO0_11 = 0; 		
	  GPIO0DIR |= 1 << 11;							
	  GPIO0DATA &= ~(1 << 11); 
	}
	break;
  }
}

void initledpwm(void)
{
  SYSAHBCLKCTRL |= 1 << 8;
  IOCON_PIO1_10 = 0x02; // select CT16B1MAT1
  TMR16B1TCR = 0x02; // reset timer
  TMR16B1CTCR = 0x00; // timer mode
  TMR16B1TC = 0; 
  TMR16B1PR = 0; 
  TMR16B1MR0 = 1000; 
  TMR16B1MR1 = 1000; 
  TMR16B1MR2 = 1000; 
  TMR16B1MCR = 1 << 7; 
  TMR16B1PWMC = 1 << 1; // enable MR1 as PWM out
  TMR16B1TCR = 0x01; // enable the timer

  SYSAHBCLKCTRL |= 1 << 9;
  IOCON_JTAG_TDI_PIO0_11 = 0x03;		// select CT32B0MAT3
  TMR32B0TCR = 0x02; // reset timer
  TMR32B0CTCR = 0x00; // timer mode
  TMR32B0TC = 0; 
  TMR32B0PR = 0; 
  TMR32B0MR0 = 1000; 
  TMR32B0MR3 = 1000; 
  TMR32B0MCR = 1 << 1; 
  TMR32B0PWMC = 1 << 3; // enable MR3 as PWM out
  TMR32B0TCR = 0x01; // enable the timer

  setledpwm(0,0);
  setledpwm(1,0);
}

// --------------------------------------------------------------------------
// Modem token handler
// --------------------------------------------------------------------------

void modem_control( char * token, char * token_data ) {
  if ( ( strcmp( token, "info" ) == 0 ) || ( strcmp( token, "i" ) == 0 ) ) {
    // Return settings to caller
    modem_name();
    modem_build();
      
    // Activate Sensor
    sensor_active();

    // nvm_detecttest();
  
    modem_send_hval( "type",    nvm_readtype() );
    modem_send_hval( "subtype", nvm_readsubtype() );
    modem_send_text( "12nc",    nvm_read12nc() );
  
    // Switch off Sensor
    sensor_inactive();
  
    modem_send_val( "verbose", verbose );
    modem_send_val( "mode",    mode    );
    modem_send_val( "units",   units   );
    modem_send_val( "ldoovr",  ldo_ovr );
  } else if ( ( strcmp( token, "softreset" ) == 0 )   || ( strcmp( token, "sr" ) == 0 ) ) {
    sensor_command_reset();
    modem_ok();
  } else if ( ( strcmp( token, "tempstart" ) == 0 )   || ( strcmp( token, "ts" ) == 0 ) ) {
    sensor_temp_setsensor( 10 );
    sensor_sensorstart( SENSOR_TEMP );
    modem_ok();
  } else if ( ( strcmp( token, "tempread" ) == 0 )    || ( strcmp( token, "tr" ) == 0 ) ) {
    if ( sensor_conversionready( SENSOR_TEMP ) ) {
      sensor_readdata( SENSOR_TEMP, &raw_temp_data );
      ftemp = sensor_temp_convertdatatotemp( raw_temp_data );
      sendTemp( ftemp );
    } else {
      modem_busy( "TEMP not ready yet" );
    }
  } else if ( ( strcmp( token, "alsread" ) == 0 )     || ( strcmp( token, "ar" ) == 0 ) ) {
    if ( mode != MODE_SEN200 ) {
      if ( sensor_conversionready( SENSOR_ALS ) ) {
        sensor_readdata( SENSOR_ALS, &raw_als_data );
        als = sensor_als_convertdatatolux( CurAlsSensor, raw_als_data );
        sendAls( CurAlsSensor, als );
      } else {
        modem_busy( "ALS not ready yet" );
      }
    } else {
      sendAls( CurAlsSensor, 0 );
    }
  } else if ( ( strcmp( token, "rhread" ) == 0 )      || ( strcmp( token, "rr" ) == 0 ) ) {
    if ( sensor_waitforconversion( SENSOR_RH, 50 ) ) {
      sensor_readdata( SENSOR_RH, &raw_rh_data );
      humidity = sensor_rh_convertdatatohumidity( CurRhSensor, raw_rh_data );
      sendRh( CurRhSensor, humidity );
    } else {
      modem_busy( "RH not ready yet" );
    }

  } else if ( strcmp( token, "auto" ) == 0 ) {
    if ( mode == MODE_MIST1431 ) {
      ldo_ovr = 1; auto_full();
//      ldo_ovr = 0; auto_full();
    } else {
      ldo_ovr = 0; auto_sen();
//      ldo_ovr = 1; auto_sen();
//      ldo_ovr = 0; auto_sen();
//      ldo_ovr = 0; auto_full();
    }
  } else if ( strcmp( token, "temp" ) == 0 ) {
    auto_temp();
  } else if ( strcmp( token, "dt" ) == 0 ) {
    default_temp();
  } else if ( strcmp( token, "prog" ) == 0 ) {
    sen_prog();
  } else if ( strcmp( token, "nvm" ) == 0 ) {
    nvm_dump();
  } else if ( strcmp( token, "nr" ) == 0 ) {
    if ( nvmindex >= 0 ) nvm_read_reg( nvmindex );
    else modem_error( "nvm index" );
  } else {
    // The following tokens have 1 value
    char * endpos;
    int val = (int)strtol( token_data, &endpos, 16 );
    if ( val == 0 && endpos == token_data ) {
      // Conversion failed
      modem_parerror( token );
    } else {
      // General
      if ( ( strcmp( token, "verbose" ) == 0 ) || ( strcmp( token, "v" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { verbose = val; modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "units" ) == 0 ) || ( strcmp( token, "u" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { units = val; modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "mode" ) == 0 ) || ( strcmp( token, "m" ) == 0 ) ) {
        if ( val >= MODE_MIST1431 && val <= MODE_SEN300 ) { mode = val; modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "bug" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { ldo_ovr = val; modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "wait" ) == 0 ) || ( strcmp( token, "w" ) == 0 ) ) {
        rtk_tsk_sleep( val ); modem_ok();

      // NVM
      } else if ( strcmp( token, "index" ) == 0 ) {
        if ( val >= 0 && val <= 255 ) nvmindex = val;
        else modem_error( token );
      } else if ( strcmp( token, "nw" ) == 0 ) {
        if ( nvmindex >= 0 ) nvm_write_reg( nvmindex, val );
        else modem_error( "nvm index" );

      // Auto
      } else if ( strcmp( token, "rh" ) == 0 ) {
        if ( val >= 0 && val <= 3 ) auto_rh( val );
        else modem_error( token );
      } else if ( strcmp( token, "als" ) == 0 ) {
        if ( val >= 0 && val <= 11 ) auto_als( val );
        else modem_error( token );
      } else if ( strcmp( token, "dr" ) == 0 ) {
        if ( val >= 0 && val <= 3 ) default_rh( val );
        else modem_error( token );
      } else if ( strcmp( token, "da" ) == 0 ) {
        if ( val >= 0 && val <= 5 ) default_als( val );
        else modem_error( token );
        
      // Advanced from here onwards
      } else if ( ( strcmp( token, "reset" ) == 0 )       || ( strcmp( token, "r" ) == 0 ) ) {
        sensor_hardreset( val );
        modem_ok();
      } else if ( ( strcmp( token, "analogpower" ) == 0 ) || ( strcmp( token, "pow" ) == 0 ) || ( strcmp( token, "p" ) == 0 ) ) {
        sensor_analogpower( val );
        modem_ok();
        
      // Temp  
      } else if ( ( strcmp( token, "temppower" ) == 0 )   || ( strcmp( token, "tp" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { sensor_sensorpower( SENSOR_TEMP, val ); modem_ok(); }
        else modem_error( token );
        
      // ALS  
      } else if ( ( strcmp( token, "alscalib" ) == 0 )    || ( strcmp( token, "ac" ) == 0 ) ) {
        if ( val >= 0 && val <= 11 ) { sensor_als_readcalib( val ); modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "alspower" ) == 0 )    || ( strcmp( token, "ap" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { sensor_sensorpower( SENSOR_ALS, val ); modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "alsstart" ) == 0 )    || ( strcmp( token, "as" ) == 0 ) ) {
        if ( val >= 0 && val <= 11 ) {
          sensor_als_setsensor( val );
          sensor_sensorstart( SENSOR_ALS );
          CurAlsSensor = val;
          modem_ok();
        } else modem_error( token );

      // RH
      } else if ( ( strcmp( token, "rhcalib" ) == 0 )     || ( strcmp( token, "rc" ) == 0 ) ) {
        if ( val >= 0 && val <= 3 ) { sensor_rh_readcalib( val ); modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "rhpower" ) == 0 )     || ( strcmp( token, "rp" ) == 0 ) ) {
        if ( val >= 0 && val <= 1 ) { sensor_sensorpower( SENSOR_RH, val ); modem_ok(); }
        else modem_error( token );
      } else if ( ( strcmp( token, "rhstart" ) == 0 )     || ( strcmp( token, "rs" ) == 0 ) ) {
        if ( val >= 0 && val <= 3 ) {
          // Temp must be ready, to be used for RH measurement
          // Rh Calib must be done also before this
          sensor_rh_setsensor( val, ftemp, raw_temp_data );
          sensor_sensorstart( SENSOR_RH );
          CurRhSensor = val;
          modem_ok();
        } else modem_error( token );      

      // IR Leds
      } else if ( strcmp( token, "led1" ) == 0 ) {
        if ( val >= 0 && val <= 100 ) { 
		  setledpwm(0,val);
          modem_ok();
        } else modem_error( token );
      } else if ( strcmp( token, "led2" ) == 0 ) {
        if ( val >= 0 && val <= 100 ) {
		  setledpwm(1,val);
          modem_ok();
        } else modem_error( token );
        
      } else {
        modem_error( "unknown token" );
      }    
    }
  }
}

// --------------------------------------------------------------------------
// Start task
// - Performs system init
// - Starts helper tasks for RF handling
// - Then waits (endlessly) for UART commands and handles them
// --------------------------------------------------------------------------

char command[80];
#define MAX_CMD  80

static void app_task_start( void *p_arg ) {
  pcb_init();
  gpio_init();
  led_init();
  vuart_init();
  dio_init();
  verbose = 1;
  sensor_init();
  verbose = 0;
  initledpwm();

  // Print modem name
  modem_name();

  char * cmd = command;
  *cmd = '\0';
  
  while(1) {
    // Read chars (without timeout = wait forever)
    int len = dio_read( DIO_VUART0, uart_message_in, MAX_UARTMESS_LEN, 0, '\n', 0 );
    
    if ( len > 0 ) {
      led_on( LED_0 );
      
      int i = 0;
      while ( i < len ) {
        char ch = uart_message_in[i++];
      
        if( (ch=='\b') || (ch=='\x7F') ) { // Backspace: remove last character
          if( cmd > command ) {
            cmd--;
          }
        } else if( (ch=='\n') || (ch=='\r') ) { // CR or LF: parse and execute
          if ( cmd > command ) {
            *cmd = '\0';
            xml_parse( command, modem_control );

            // New command
            cmd = command;
            }
        } else if( ch=='\x1B' ) { // ESC: erase line (when full), recall line (when empty)
          if ( cmd == command ) {
            // recall previous command
            xml_parse( command, modem_control );
          } else {
            // erase current command
            cmd = command;
           *cmd = '\0';
          }
        } else if( ( cmd - command ) >= MAX_CMD-1 ) { // Too many chars: beep
          // -1 because later \0 is needed
          // buffer overflow
        } else if( ch<32 ) { // unprintable character: beep (don't add)
          // unprintable character
        } else { // Char: add
          *cmd = ch;
          cmd++;
        }
      }
      
      led_off( LED_0 );
    }
  }
}

// --------------------------------------------------------------------------
// STARTUP
// --------------------------------------------------------------------------

void vuart_boothack( void ) {
  // When a USB device is plugged in, it may draw 100mA current for booting. It
  // should boot its USB stack, and then add a pull-up resistor to D+ to signal
  // the host that the device is ready (eg may draw 500mA). This pull-up is an
  // external resistor added using the USB_CONNECT pin (under USB block control).
  // There are two issues on the dongle: the pin with function USB_CONNECT also
  // has the SPI CLK function. On the dongle board the latter is used, so we use
  // an alternative GPIO pin to add the pull-up under software control.
  // Secondly, on the dongle, the hw default for the connect pin upon reset is
  // to have the pull-up connected instead of disconnected. This boot hack
  // function decouples the pull-up; it should be called as first statement
  // in main().
  SYSAHBCLKCTRL_bit.IOCON = 1;    // Enable PCB block
  IOCON_PIO1_11           = 0x10; // Function GPIO for P1.11
  SYSAHBCLKCTRL_bit.GPIO  = 1;    // Enable GPIO block
  GPIO1DIR_bit.P1_11      = 1;    // Set P1.11 as output
  // GPIO1DATA_bit.P1_11  = 0;    // Clears P1.11, ie decouples pull-up from USB D+ (=device absent)
  GPIO1DATA_bit.P1_11     = 1;    // Sets P1.11, ie decouples pull-up from USB D+ (=device absent)
}

void main( void ) {
  vuart_boothack();
  clk_init();
  rtk_tsk_create( app_task_start, "app.start", APP_TASK_START_STKSIZE, NULL, APP_TASK_START_PRIO );
  rtk_sys_start();
  assert(0);
}

#endif
