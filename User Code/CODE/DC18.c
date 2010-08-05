/** ###################################################################
**     Filename  : DC18.C
**     Project   : DC18
**     Processor : MC56F8006_32_LQFP
**     Version   : Driver 01.13
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 2/1/2010, 1:25 PM
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/
/* MODULE DC18 */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "Term.h"
#include "Inhr6.h"
#include "LED0.h"
#include "LED1.h"
#include "SOUT1.h"
#include "PWREN.h"
#include "EINT_SW0.h"
#include "EINT_SW1.h"
#include "LCD_CS.h"
#include "LCD_SIMO.h"
#include "LCD_SCLK.h"
#include "LCD_DC.h"
#include "LCD_RST.h"
#include "LCD_BUSY.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

#include "DC18_Badge.h"

void main(void)
{
  /* Write your local variable definition here */
  extern word _vba;
    
  INTC_VBA = ((word)&_vba) >> 7;	// restore VBA register to point to user defined vector table
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  dc18_badge();
  for(;;) {}
}

/* END DC18 */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
