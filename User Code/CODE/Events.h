/** ###################################################################
**     Filename  : Events.H
**     Project   : DC18
**     Processor : MC56F8006_32_LQFP
**     Beantype  : Events
**     Version   : Driver 01.03
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 2/1/2010, 1:25 PM
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         EINT_SW1_OnInterrupt   - void EINT_SW1_OnInterrupt(void);
**         EINT_SW0_OnInterrupt   - void EINT_SW0_OnInterrupt(void);
**         EINT_PWREN_OnInterrupt - void EINT_PWREN_OnInterrupt(void);
**
**     (c) Copyright UNIS, a.s. 1997-2008
**     UNIS, a.s.
**     Jundrovska 33
**     624 00 Brno
**     Czech Republic
**     http      : www.processorexpert.com
**     mail      : info@processorexpert.com
** ###################################################################*/

#ifndef __Events_H
#define __Events_H
/* MODULE Events */

#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
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

void EINT_SW1_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  EINT_SW1_OnInterrupt (module Events)
**
**     From bean   :  EINT_SW1 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void EINT_SW0_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  EINT_SW0_OnInterrupt (module Events)
**
**     From bean   :  EINT_SW0 [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void EINT_PWREN_OnInterrupt(void);
/*
** ===================================================================
**     Event       :  EINT_PWREN_OnInterrupt (module Events)
**
**     From bean   :  EINT_PWREN [ExtInt]
**     Description :
**         This event is called when an active signal edge/level has
**         occurred.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void BTN1_OnButton(void);
/*
** ===================================================================
**     Event       :  BTN1_OnButton (module Events)
**
**     Component   :  BTN1 [Button]
**     Description :
**         This event is called when the button is pressed.
**         If button inactivity feature (advanced view) is enabled,
**         then the next OnButton event is not generated during dead
**         time.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

/* END Events */
#endif /* __Events_H*/

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
