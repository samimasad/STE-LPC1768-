/*----------------------------------------------------------------------------
 * Name:    ADC.c
 * Purpose: MCB1700 low level ADC functions
 * Version: V1.00
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2009 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 * History:
 *          V1.00 Initial Version
 *----------------------------------------------------------------------------*/

#include <LPC17xx.H>                             /* LPC17xx definitions */
#include "ADC.h"

/*----------------------------------------------------------------------------
  initialize ADC Pins
 *----------------------------------------------------------------------------*/
void ADC_init (void) {

  PINCON->PINSEL1 &= ~(3<<18);                   /* P0.25 is GPIO */
  PINCON->PINSEL1 |=  (1<<18);                   /* P0.25 is AD0.2 */

  SC->PCONP       |=  (1<<12);                   /* Enable power to ADC block */

  ADC->ADCR        =  (1<< 2) |                  /* select AD0.2 pin */
                      (1<< 8) |                  /* ADC clock is 18MHz/2 */
                      (1<<21);                   /* enable ADC */ 
}


/*----------------------------------------------------------------------------
  start ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_startCnv (void) {
  ADC->ADCR &= ~(7<<24);                         /* stop conversion  */
  ADC->ADCR |=  (1<<24);                         /* start conversion */
}


/*----------------------------------------------------------------------------
  stop ADC Conversion
 *----------------------------------------------------------------------------*/
void ADC_stopCnv (void) {
  ADC->ADCR &= ~(7<<24);                         /* stop conversion */
}


/*----------------------------------------------------------------------------
  get converted ADC value
 *----------------------------------------------------------------------------*/
uint32_t ADC_getCnv (void) {
  uint32_t adGdr;
//  uint32_t adDr2;

  while (!(ADC->ADGDR & (1UL<<31)));             /* Wait for Conversion end */
  adGdr = ADC->ADGDR;
//  adDr2 = ADC->ADDR2;
  return((adGdr >> 4) & ADC_VALUE_MAX);          /* read converted value */
}
