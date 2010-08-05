/** ###################################################################
**     Filename  : Events.C
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
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#include "DC18_Badge.h"

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
//#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void EINT_SW1_OnInterrupt(void)
{
  /* place your EINT_SW1 interrupt procedure body here */
}


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
//#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void EINT_SW0_OnInterrupt(void)
{
  /* place your EINT_SW0 interrupt procedure body here */
}


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
//#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void EINT_PWREN_OnInterrupt(void)
{
  /* place your EINT_PWREN interrupt procedure body here */
}


/*
** ===================================================================
**     Event       :  Inhr6_OnRxChar (module Events)
**
**     From bean   :  Inhr6 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is
**         received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the
**         <Receiver> property is enabled or the <SCI output mode>
**         property (if supported) is set to Single-wire mode.
**         Version specific information for Freescale 56800
**         derivatives ] 
**         DMA mode:
**         If DMA controller is available on the selected CPU and
**         the receiver is configured to use DMA controller then
**         this event is disabled. Only OnFullRxBuf method can be
**         used in DMA mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
/*
** ===================================================================
**     Event       :  Inhr6_OnRxChar (module Events)
**
**     From bean   :  Inhr6 [AsynchroSerial]
**     Description :
**         This event is called after a correct character is
**         received.
**         The event is available only when the <Interrupt
**         service/event> property is enabled and either the
**         <Receiver> property is enabled or the <SCI output mode>
**         property (if supported) is set to Single-wire mode.
**         Version specific information for Freescale 56800
**         derivatives ] 
**         DMA mode:
**         If DMA controller is available on the selected CPU and
**         the receiver is configured to use DMA controller then
**         this event is disabled. Only OnFullRxBuf method can be
**         used in DMA mode.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
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
#pragma interrupt called /* Comment this line if the appropriate 'Interrupt preserve registers' property */
                         /* is set to 'yes' (#pragma interrupt saveall is generated before the ISR)      */
void BTN1_OnButton(void)
{
  /* Write your code here ... */
}

/* END Events */

/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.99 [04.17]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
