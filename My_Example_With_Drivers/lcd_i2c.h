/*
 * lcd_i2c.h
 *
 *  Created on: Jan 6, 2011
 *      Author: nlv14991
 */

#ifndef LCD_I2C_H_
#define LCD_I2C_H_

int LCD_Init(void);
int LCD_ClearScreen(void);
int LCD_PrintText(uint8_t *txt);

#endif /* LCD_I2C_H_ */
