/** ###################################################################
**     THIS COMPONENT MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
**     Filename  : Term.H
**     Project   : DC18
**     Processor : MC56F8006_32_LQFP
**     Component : Term
**     Version   : Component 02.029, Driver 03.18, CPU db: 3.00.188
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 5/31/2010, 10:09 AM
**     Abstract  :
**
**     Settings  :
**
**     Contents  :
**         SendStr    - void Term_SendStr(uint8_t *str);
**         SendNum    - void Term_SendNum(int32_t number);
**         SendChar   - void Term_SendChar(uint8_t Val);
**         MoveTo     - void Term_MoveTo(uint8_t x, uint8_t y);
**         ReadChar   - void Term_ReadChar(uint8_t *c);
**         KeyPressed - bool Term_KeyPressed(void);
**
**     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
**     
**     http      : www.freescale.com
**     mail      : support@freescale.com
** ###################################################################*/

#ifndef __Term
#define __Term

/*Include shared modules, which are used for whole project*/
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* MODULE Term. */

#include "Cpu.h"


void Term_SendChar(uint8_t Val);
/*
** ===================================================================
**     Method      :  Term_SendChar (component Term)
**
**     Description :
**         Send char to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Val             - Char to send
**     Returns     : Nothing
** ===================================================================
*/

void Term_ReadChar(uint8_t *c);
/*
** ===================================================================
**     Method      :  Term_ReadChar (component Term)
**
**     Description :
**         Read char from terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * c               - Pointer to a char variable
**     Returns     : Nothing
** ===================================================================
*/

bool Term_KeyPressed(void);
/*
** ===================================================================
**     Method      :  Term_KeyPressed (component Term)
**
**     Description :
**         Test if char was received from the terminal.
**     Parameters  : None
**     Returns     :
**         ---             - Return TRUE if user pressed some key
** ===================================================================
*/

void Term_SendStr(void *str);
/*
** ===================================================================
**     Method      :  Term_SendStr (component Term)
**
**     Description :
**         Send string to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * str             - Pointer to a string variable
**     Returns     : Nothing
** ===================================================================
*/

void Term_SendNum(int32_t number);
/*
** ===================================================================
**     Method      :  Term_SendNum (component Term)
**
**     Description :
**         Send number to terminal.
**     Parameters  :
**         NAME            - DESCRIPTION
**         number          - Long number
**     Returns     : Nothing
** ===================================================================
*/

void Term_MoveTo(uint8_t x,uint8_t y);
/*
** ===================================================================
**     Method      :  Term_MoveTo (component Term)
**
**     Description :
**         Move cursor to position x, y. Range of coordinates can be
**         from 1 to the size, which depends on the used terminal.
**         Position [1,1] represents the upper left corner.
**     Parameters  :
**         NAME            - DESCRIPTION
**         x               - Column number
**         y               - Row number
**     Returns     : Nothing
** ===================================================================
*/


/* END Term. */

#endif /* ifndef __Term */
/*
** ###################################################################
**
**     This file was created by Processor Expert 3.00 [04.35]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/