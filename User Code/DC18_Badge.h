/******************************************************************************
*
* DEFCON 18 BADGE
*
* Filename:		DC18_Badge.h
* Author:		  Joe Grand (Kingpin)
* Revision:		1.0
*
* Description:	Header File for the DEFCON 18 Badge (Freescale MC56F8006)
* Notes:
*
* None yet!	
*******************************************************************************/

#ifndef __DC18__
#define __DC18__

#include <stdint.h>


/**************************************************************************
************************** Definitions ************************************
***************************************************************************/

#define LOW   0
#define HIGH  1

#define ON    HIGH 
#define OFF   LOW

#define SW_NONE 	0b00
#define SW_0			0b01
#define	SW_1			0b10
#define SW_BOTH		0b11


/**************************************************************************
************************** Macros *****************************************
***************************************************************************/


/**************************************************************************
************************** Structs ****************************************
***************************************************************************/

typedef enum
{
	DEFCON,
	EIGHTEEN, // * 0xE38 + 16¢
	BADGE,
	BY,
	JOEGRAND,
	AKA,
	KINGPIN,
	USB,
	NINJA
} badge_state_type;

typedef enum
{
	TOP = 0,
	MIDDLE = 1,
	BOTTOM = 2
} tumbler_state_type;

/***********************************************************************
 ************************** Function prototypes ************************
 ***********************************************************************/

void dc18_badge(void);
void dc18_get_buttons(void);
void dc18_change_state(void);

void dc18_clear_fb(void);
void dc18_load_image(uint16_t image_num);
void dc18_load_tumblers(void); 
void dc18_update_lcd(void);
void dc18_usb_send_ack(void);

void dc18_init(void);
void dc18_sleep(void);

int dc18_ninja_validate(unsigned long val); 
uint32_t dc18_encode_tumblers(tumbler_state_type *tumblers); 

void SendChar(uint8_t);
//void SendMsg(uint8_t *msg);
void Delay(uint16_t ms);


/**************************************************************************
************** 000017F0: 0060d0f5bfd62049a77dcfc3a14fad8b *****************
***************************************************************************/

#endif /* __DC18__ */
