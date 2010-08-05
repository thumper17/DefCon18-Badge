/******************************************************************************
*
* Filename:		DC18_LCD.h
* Author:		  Kent Displays, Inc.
*
* Description:	Header File for Kent Displays' 128x32 Reflex Graphic Display Module
* Notes:
*
* Source code is provided in C for implementing a full-screen display update. A small 
* number of user-defined functions and parameters specific to the host platform must 
* be implemented independently (done by Joe Grand for use with the DEFCON 18 Badge). 	
*
* Reprinted from data sheet 25138B
*
*******************************************************************************/

/* **************************************************************************** 
   Copyright (c) 2009. Kent Displays, Inc. All Rights Reserved. 
 
   Permission to use, copy, modify, and distribute this software and its 
   documentation for any purpose, without fee, and without a written 
   agreement, is hereby granted, provided that the above copyright notice, 
   this paragraph, and the following two paragraphs appear in all copies, 
   modifications, and distributions and is included with a Kent Displays, 
   Inc. product. 
 
   IN NO EVENT SHALL KENT DISPLAYS, INC. BE LIABLE TO ANY PARTY FOR DIRECT, 
   INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING 
   LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS 
   DOCUMENTATION, EVEN IF KENT DISPLAYS, INC. HAS BEEN ADVISED OF THE 
   POSSIBILITY OF SUCH DAMAGE. 
 
   KENT DISPLAYS, INC. SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT 
   NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
   FOR A PARTICULAR PURPOSE. THE SOFTWARE AND ACCOMPANYING 
   DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS PROVIDED "AS IS". 
   KENT DISPLAYS, INC. HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
   UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 *****************************************************************************/ 
 
#ifndef __DC18_LCD__
#define __DC18_LCD__


/****** LOADDATA.C ******/

#define COL_LSN       0x00  // Set Lower Column Address  
#define COL_MSN       0x10  // Set Higher Column Address  
#define PAGE_ADD      0xB0  // Set Page Address  


/****** DEFINES.H ******/

// Prototypes for functions to abstract I/O and timer hardware.  These control  
// or read individual I/O lines at the display interface.  The functions must  
// be implemented in a user-written file.  
  
// nRESET_DRV manipulation.  
#define Assert_nRESET_DRV()				LCD_RST_PutVal(LOW)			// nRESET_DRV = 'L'
#define Deassert_nRESET_DRV()			LCD_RST_PutVal(HIGH)    // nRESET_DRV = 'H'  
  
// BUSY_DRV monitoring 
void WaitNonBusy(void); 		// Stall program execution for as long as BUSY_DRV remains HIGH
uint8_t getBUSY_DRV(void);	// Read BUSY_DRV, returning 0 or 1  
  
// Display selection/de-selection.  
#define SelectDisplay()						LCD_CS_PutVal(LOW)      // nCS_DRV = 'L'  
#define DeselectDisplay()         LCD_CS_PutVal(HIGH)     // nCS_DRV = 'H'  
  
// D_nC_DRV control.  
#define Assert_D_DRV() 						LCD_DC_PutVal(HIGH)     // D/nC_DRV = 'H'  
#define Assert_nC_DRV()           LCD_DC_PutVal(LOW)      // D/nC_DRV = 'L'  
  
// SPI bit manipulations.  
#define setSIMO()               	LCD_SIMO_PutVal(HIGH)   // SIMO = 'H'  
#define clearSIMO()               LCD_SIMO_PutVal(LOW)    // SIMO = 'L'  
#define setSCLK()                 LCD_SCLK_PutVal(HIGH)   // SCLK = 'H'  
#define clearSCLK()               LCD_SCLK_PutVal(LOW)    // SCLK = 'L'  
  
// Functions to zero outputs to the driver before powering it down.  
void ZeroSPILines(void);                // SCLK, SIMO, D_nC_DRV = 'L'  
  
// Image characteristics (excluding frame).  
#define N_ROWS            32  
#define N_COLS            128  
#define BYTES_PER_IMAGE   (N_ROWS*N_COLS/8)  
#define N_PAGES           (N_ROWS/8)  
#define BYTES_PER_PAGE    (BYTES_PER_IMAGE/N_PAGES) 
#define DUMMY_ROWS				4  
  
// Image remapping:  A normal image's top is at the furthest edge from the   
// flex cable.  A remapped image's top is nearest the flex cable.    
//  
// REMAP_IMAGE    Image       Top of image.   
// ------------   --------    ---------------  
// undefined      Normal      Away from flex.  
// defined        Remapped    Near flex.  
//  
#undef   REMAP_IMAGE      // Define or undefine this token for   
                          // desired image orientation.  
          
//-----------------------------------------------------------------------------  
  
// Function Prototypes  
  
// display.c  
void DriveImage(uint8_t firstRow, uint8_t numRows,  
                uint8_t VAClearPW, uint8_t VAIdle,   
                uint8_t AAClearPW, uint8_t AAIdle,  
                uint8_t DrivePW, uint8_t ClearV, uint8_t DriveV,  
                uint8_t VAClearRpt, uint8_t VAIdleRpt,   
                uint8_t AAClearRpt, uint8_t AAIdleRpt,  
                uint8_t DriveRpt,  
                uint8_t Bias);
void Display(uint8_t firstRow, uint8_t numRows);  
  
// loaddata.c  
void LoadData(uint16_t Index, uint16_t NumBytes,  uint8_t *pData);  
void LoadBorder(uint8_t border);  
  
// system.c  
  
// Tokens for selecting command byte or data byte.  For PutCharSPI().  
#define CMD_MASK          0  
#define DATA_MASK         1  
  
void PutCharSPI(uint8_t data, uint8_t Data_nCmd);    
void HardResetDisplay(void);  


#endif /* __DC18_LCD__ */
