/******************************************************************************
*
* Filename:		DC18_LCD.c
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
 
#include "DC18_Badge.h"
#include "DC18_LCD.h"  

#include "Cpu.h"
#include "Events.h"

/****** DC18 Badge-specific low-level functions ******/
// Most of the required functions are defined as macros in DC18_LCD.h

// BUSY_DRV monitoring  
void WaitNonBusy(void)			// Stall program execution for as long as BUSY_DRV remains HIGH
{
	while(LCD_BUSY_GetVal())
	{   
			LED0_PutVal(ON);			// Enable LED to show that the LCD is busy
	}
	
	LED0_PutVal(OFF);
}

uint8_t getBUSY_DRV(void)   // Read BUSY_DRV, returning 0 or 1  
{
	if (!LCD_BUSY_GetVal())
  	return (0);
	else
		return (1);
}

 
// Functions to zero outputs to the driver before powering it down.  
void ZeroSPILines(void)   	// SCLK, SIMO, D_nC_DRV = 'L'  
{
	clearSCLK();
	clearSIMO();
	Assert_nC_DRV();
}


/****** DISPLAY.C ******/
  
///////////////////////////////////////////////////////////////////////////////  
//  
// P u b l i c   F u n c t i o n s  
//  
///////////////////////////////////////////////////////////////////////////////  
  
///////////////////////////////////////////////////////////////////////////////  
// Function: Display  
// Purpose:  Updates display with image data in driver RAM using   
//           temperature compensation drive parameters.
// Inputs:   firstRow - first physical display row to update  
//           numRows - number of rows to update  
// Outputs:  None  
///////////////////////////////////////////////////////////////////////////////  
void Display(uint8_t firstRow, uint8_t numRows)  
{  
  uint8_t VAClearPW, VAIdle, AAClearPW, AAIdle, DrivePW, ClearV;  
  uint8_t DriveV, VAClearRpt, VAIdleRpt, AAClearRpt, AAIdleRpt, DriveRpt, Bias;  
  
  // Set parameters  
  // Note: The compensation table has been removed and the temperature has been
	//       hard-coded at 25 degrees C (~room temperature) as recommended by KDI
	DrivePW    = 0x0D; 	// 20mS   
  DriveV     = 0x40; 	// 24V 
  VAClearPW  = 0x15; 	// 100mS  
  VAIdle     = 0x05; 	// Stir time between VA & AA clear (2mS)
  AAClearPW  = 0x11; 	// 40mS 
  AAIdle     = 0x07; 	// 6mS
  ClearV     = DriveV;  
    
  VAClearRpt = 1;  
  VAIdleRpt  = 1;  
  AAClearRpt = 1;  
  AAIdleRpt  = 1;  
  DriveRpt   = 1;  
  Bias       = 0x02;	// 1/7  
  
  // Display the image and break out of the search loop.  
  DriveImage( firstRow, numRows,  
              VAClearPW, VAIdle, AAClearPW, AAIdle,  
              DrivePW, ClearV, DriveV,  
              VAClearRpt, VAIdleRpt,   
              AAClearRpt, AAIdleRpt,  
              DriveRpt, Bias);  
}  

/////////////////////////////////////////////////////////////////////////////// 
//  
// P r i v a t e   F u n c t i o n s  
//  
///////////////////////////////////////////////////////////////////////////////  
///////////////////////////////////////////////////////////////////////////////  
// Function: DriveImage  
// Purpose:  Updates display with image data in driver RAM using   
//           supplied drive parameters.  
// Inputs:   firstRow   - first physical display row to update  
//           numRows    - number of rows to update  
//           VAClearPW  - VA Clear Pulse width (driver encoding)  
//           VAIdle     - VA Idle Duration (driver encoding)  
//           AAClearPW  - AA Clear Pulse width (driver encoding)  
//           AAIdle     - AA Idle Duration (driver encoding)  
//           DrivePW    - Scan Pulse width (driver encoding)   
//           ClearV     - VA/AA Clear Voltage (driver encoding)  
//           DriveV     - Scan Voltage (driver encoding)  
//           VAClearRpt - Number of VA Clear Cycles  
//           VAIdleRpt  - Number of VA Idles  
//           AAClearRpt - Number of AA Clear Cycles  
//           AAIdleRpt  - Number of AA Idles  
//           DriveRpt   - Number of Driver Cycles  
//           Bias       - Drive Voltage Bias (driver encoding)  
// Outputs:  None.  
// Notes:    Full-screen updates include additional dummy rows (DUMMY_ROWS) to  
//           let last image row see some non-select rows.   
//           Driver must already be powered with image loaded.  
///////////////////////////////////////////////////////////////////////////////  
void DriveImage(uint8_t firstRow, uint8_t numRows,  
                uint8_t VAClearPW, uint8_t VAIdle,   
                uint8_t AAClearPW, uint8_t AAIdle,  
                uint8_t DrivePW, uint8_t ClearV, uint8_t DriveV,  
                uint8_t VAClearRpt, uint8_t VAIdleRpt,   
                uint8_t AAClearRpt, uint8_t AAIdleRpt,  
                uint8_t DriveRpt,  
                uint8_t Bias)  
{  
  uint8_t offset, startLineCmd;  
  
  // Map first row to first driver COMMON.  
  if ((firstRow == 0) && (numRows == 32))  // Full screen?  
  {  
    // Include top/bottom active frame in update area.  Thus the image spans  
    // 34 COM lines, COM15 to COM48.  
    firstRow = 15;  
    numRows = 34;       
  }  
  else // Partial screen.  
  {  
    // First image row is on the COM line that is 16 inward from the edge  
    // COM16 or COM47.  
    firstRow += 16;  
  } 
  
  // Compute image location parameters.  
  #ifdef REMAP_IMAGE  
    offset = 0x40 - firstRow;     // Location on display.  
  #else  
    offset = (uint8_t)(firstRow + numRows);  // Location on display.  
    if (firstRow == 15)           // Offset needs special treatment when we   
      offset += DUMMY_ROWS;       // do a full image in non-remapped mode.  
  #endif  
  startLineCmd = (uint8_t)(0x40 + ((firstRow==15) ? 15 : 16)); // Location in display RAM.  
    
  // Initialization code.  
  PutCharSPI( 0xA3, CMD_MASK );   // Enable band gap and other analog control.  
  PutCharSPI( 0x18, CMD_MASK );  
  PutCharSPI( 0xF6, CMD_MASK );   // Enable oscillator.  
  PutCharSPI( 0x40, CMD_MASK );  
  PutCharSPI( 0xAE, CMD_MASK );   // Set auto charge pump threshold value.  
  PutCharSPI( 0x00, CMD_MASK );  
  
  PutCharSPI( 0xA2, CMD_MASK );   // Set Bias level  
  PutCharSPI( Bias, CMD_MASK );  
    
  // Set drive parameters.  
  PutCharSPI(      0x80, CMD_MASK );  
  PutCharSPI(      0x00, CMD_MASK );  
  PutCharSPI( VAClearPW, CMD_MASK );  // VA Clear Duration  
  PutCharSPI(    VAIdle, CMD_MASK );  // VA Idle  Duration.  Stir time.  
  PutCharSPI( AAClearPW, CMD_MASK );  // AA Clear Duration  
  PutCharSPI(    AAIdle, CMD_MASK );  // AA Idle  Duration  
  PutCharSPI(   DrivePW, CMD_MASK );  // Drive    Duration  
  PutCharSPI(    ClearV, CMD_MASK );  // Clear  Voltage  
  PutCharSPI(    DriveV, CMD_MASK );  // Drive  Voltage  
  
  // Set dummy waveform for supply initialization.  
  PutCharSPI( 0x93, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK );  // Skip VA Clear.  
  PutCharSPI( 0x94, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK );  // Skip VA Idle.  
  PutCharSPI( 0x95, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK );  // Skip AA Clear.  
  PutCharSPI( 0x96, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK );  // Skip AA Idle.  
  PutCharSPI( 0x97, CMD_MASK );  
  PutCharSPI( 0x00, CMD_MASK );  // Skip Drive.  
  
  // Dummy update results in future supply initialization to Clear Voltage.  
  PutCharSPI( 0x31, CMD_MASK );  // Dummy update.  
  WaitNonBusy();  
  
  // More driver initialization code.  
  PutCharSPI( 0xA3, CMD_MASK);  // Enable other analog control.  
  PutCharSPI( 0x1A, CMD_MASK);  
  PutCharSPI( 0xE9, CMD_MASK);  // Enable bias driven. 
  PutCharSPI( 0x84, CMD_MASK);  
  PutCharSPI( 0x2F, CMD_MASK);  // Enable booster and high voltage buffer.  
  
  // Delay to allow dc/dc reach voltage.  
  // Rise time to 25V with Vin = 2.4V is 280 msec.  
  // Rise time to 25V with Vin = 3.3V is 160 msec.  
  Delay(240);
  
  // Set up update.  
  PutCharSPI(       0x93, CMD_MASK );  
  PutCharSPI( VAClearRpt, CMD_MASK );     // VA Clear Repeats  
  PutCharSPI(       0x94, CMD_MASK );  
  PutCharSPI(  VAIdleRpt, CMD_MASK );     // VA Idle  Repeats  
  PutCharSPI(       0x95, CMD_MASK );  
  PutCharSPI( AAClearRpt, CMD_MASK );     // AA Clear Repeats  
  PutCharSPI(       0x96, CMD_MASK );  
  PutCharSPI(  AAIdleRpt, CMD_MASK );     // AA Idle  Repeats  
  PutCharSPI(       0x97, CMD_MASK );  
  PutCharSPI(   DriveRpt, CMD_MASK );     // Drive    Repeats  
  
  PutCharSPI( 0x32, CMD_MASK );           // Drive scheme:  
  PutCharSPI( 0x32, CMD_MASK );           // Clear to bright.  
    
  // Configure update area.  
  PutCharSPI( 0xa8, CMD_MASK );  
  if (numRows == 34)  
    PutCharSPI( (uint8_t)(numRows+DUMMY_ROWS), CMD_MASK ); // Mux ratio, full screen.  
  else  
    PutCharSPI( numRows, CMD_MASK );            // Mux ratio, partial screen.  
  
  PutCharSPI( 0xd3, CMD_MASK );  
  PutCharSPI( offset, CMD_MASK );               // Physical location on screen.  
  PutCharSPI( startLineCmd, CMD_MASK );         // Starting location in RAM.  
    
  // Set AA-idle-to-drive delay to 8 msec (coincidentally, 0x08 in driver   
  // encoding).  Shorter values may cause image problems due to chip logic,   
  // especially at high operating frequencies.  The problem manifests  
  // as one or more spurious bright lines during a partial update.   
  PutCharSPI( 0xAA, CMD_MASK );     
  PutCharSPI( 0x08, CMD_MASK );     
  
  // Set COMs to remap or not remap the image.  Segs are remapped during    
  // display RAM loading.  
  #ifdef REMAP_IMAGE  
    PutCharSPI( 0xC0, CMD_MASK );         // Scan COMs normally  
  #else  
    PutCharSPI( 0xC8, CMD_MASK );         // Scan COMs in reverse.  
  #endif  
  
  // Perform Update.  
  PutCharSPI( 0x31, CMD_MASK);  
  WaitNonBusy();  
 
  // Put display to sleep.  
  PutCharSPI( 0x2A, CMD_MASK);  
  PutCharSPI( 0xE9, CMD_MASK);  
  PutCharSPI( 0x04, CMD_MASK);  
  PutCharSPI( 0xF6, CMD_MASK);  
  PutCharSPI( 0x00, CMD_MASK);  
  PutCharSPI( 0xA3, CMD_MASK);  
  PutCharSPI( 0x00, CMD_MASK);  
  PutCharSPI( 0xAB, CMD_MASK);  
  PutCharSPI( 0x00, CMD_MASK);   
} 


/****** LOADDATA.C ******/

///////////////////////////////////////////////////////////////////////////////  
// Function:  LoadData  
// Purpose:   Load a portion of display RAM with image data.  
// Inputs:    Index    - starting position within display RAM for data (0 to   
//                       BYTES_PER_IMAGE-1)  
//            NumBytes - number of bytes of image data to load  
//            pData    - pointer to the image data to load  
// Outputs:   None.  
// Notes:     Index is adjusted to properly map into display RAM.  
//            If Vdd is off, this function turns it on and leaves it on,  
//            presuming the display routine will turn it off later.  
///////////////////////////////////////////////////////////////////////////////  
void LoadData(uint16_t Index, uint16_t NumBytes,  uint8_t *pData)  
{  
  uint8_t colAddr, page;  
  uint16_t i;  
  
  // Segment remapping must be done when loading data.  
  #ifdef REMAP_IMAGE  
    PutCharSPI( 0xA1, CMD_MASK );       // Remap segs.  
  #else  
    PutCharSPI( 0xA0, CMD_MASK );       // Don't remap segs.  
  #endif  
      
  // Loop through the data to write.  
  for (i = Index; i < (Index + NumBytes); i++)  
  {    
    colAddr = (uint8_t)((i % N_COLS) + 2); // Add two to skip unused & frame.  
  
    // Set page and column address as needed.  
    if ((colAddr == 2) || (i == Index) )  
    {  
      page = (uint8_t)((i/N_COLS) + 2); // Add two to skip unused & frame.  
      PutCharSPI( (uint8_t)(COL_MSN | (colAddr / 16)) , CMD_MASK );  
      PutCharSPI( (uint8_t)(COL_LSN | (colAddr % 16)) , CMD_MASK );  
      PutCharSPI( (uint8_t)(PAGE_ADD | page) , CMD_MASK );  
    }  
  
    // Write a data byte.  
    PutCharSPI( *pData++, DATA_MASK );  
  }  
  
  // NOP command is required after writing data before chip select goes high.  
  PutCharSPI( 0xE3, CMD_MASK );  
}  
  
  
///////////////////////////////////////////////////////////////////////////////  
// Function:  LoadBorder 
// Purpose:   Sets the display RAM corresponding to the active frame to the  
//            given value.  
// Inputs:    border - 0: dark, 0xFF: previous, else: bright  
// Outputs:   None.    
///////////////////////////////////////////////////////////////////////////////  
void LoadBorder(uint8_t border)  
{  
  uint8_t data, i, colAddr, page;  
  
  // Segment remapping must be done when loading data.  
  #ifdef REMAP_IMAGE  
    PutCharSPI( 0xA1, CMD_MASK );       // Remap segs.  
  #else  
    PutCharSPI( 0xA0, CMD_MASK );       // Don't remap segs.  
  #endif  
      
  // Initialize the image memory data value for the border.  
  if (border == 0)  
    data = 0;  
  else  
    data = 0xff;  
  
  // Write the top border.  
  colAddr = 1;  
  page = 1;  
  PutCharSPI( (uint8_t)(COL_MSN | (colAddr / 16)) , CMD_MASK );  
  PutCharSPI( (uint8_t)(COL_LSN | (colAddr % 16)) , CMD_MASK );  
  PutCharSPI( (uint8_t)(PAGE_ADD | page) , CMD_MASK );  
  for ( i = 0; i < N_COLS+2; i++)  
    PutCharSPI( data, DATA_MASK );  
     
  // Write the bottom border.  
  colAddr = 1;  
  page = N_PAGES + 2;  
  PutCharSPI( (uint8_t)(COL_MSN | (colAddr / 16)) , CMD_MASK );  
  PutCharSPI( (uint8_t)(COL_LSN | (colAddr % 16)) , CMD_MASK );  
  PutCharSPI( (uint8_t)(PAGE_ADD | page) , CMD_MASK );  
  for ( i = 0; i < N_COLS+2; i++)  
    PutCharSPI( data, DATA_MASK ); 
   
  // Write the side borders for the 4 image pages.  
  for (page = 2; page < 2+N_PAGES; page++)  
  {  
    // Write the left side border.    
    colAddr = 1;  
    PutCharSPI( (uint8_t)(COL_MSN | (colAddr / 16)) , CMD_MASK );  
    PutCharSPI( (uint8_t)(COL_LSN | (colAddr % 16)) , CMD_MASK );  
    PutCharSPI( (uint8_t)(PAGE_ADD | page) , CMD_MASK );  
    PutCharSPI( data, DATA_MASK );  
  
    // Write the right side border.    
    colAddr = N_COLS+2;  
    PutCharSPI( (uint8_t)(COL_MSN | (colAddr / 16)) , CMD_MASK );  
    PutCharSPI( (uint8_t)(COL_LSN | (colAddr % 16)) , CMD_MASK );  
    PutCharSPI( data, DATA_MASK );  
  }  
}  


/****** SYSTEM.C ******/

///////////////////////////////////////////////////////////////////////////////  
// Function:  PutCharSPI  
// Purpose:   Sends a byte to the SPI bus.  
// Inputs:    data - byte to send  
//            Data_nCmd - command (0) or data (else) attribute of byte to send  
// Outputs:   None.  
// Note: The MC56F8006 has internal SPI hardware, but we will bit-bang it instead
///////////////////////////////////////////////////////////////////////////////  
void PutCharSPI(uint8_t data, uint8_t Data_nCmd)  
{  
  uint8_t mask, i;  
  
  // Assert chip select to display.  
  // The driver requires a new chip select to start each byte.  
  SelectDisplay();  
  
  // Signal byte type to driver.  
  if (Data_nCmd)  
    Assert_D_DRV();  
  else  
    Assert_nC_DRV();  
  
  // Bit-bang data out, MSB first.  Data changes on falling edge and  
  // is latched on rising.  Clock is inactive low.  
  for (mask = 0x80; mask != 0; mask >>= 1)  
  {  
    if (data & mask)  
      setSIMO();  
    else  
      clearSIMO();        
  
    setSCLK();
    for (i = 0; i < 10; ++i){}; // add ~30uS delay to meet timing specification
    clearSCLK();
    for (i = 0; i < 3; ++i){}; // add ~10uS delay to meet timing specification    
  }  
  
  // De-assert chip select to display. This places the LCD in low-power standby mode. 
  DeselectDisplay();
}  

  
///////////////////////////////////////////////////////////////////////////////  
// Function:  HardResetDisplay  
// Purpose:   Asserts the reset line to the display.  Systems which leave  
//            the driver powered between updates should perform a hard reset  
//            before a data load / update cycle.  
// Inputs:    None.  
// Outputs:   None.  
///////////////////////////////////////////////////////////////////////////////  
void HardResetDisplay(void)  
{  
  // Assert reset line.  
  Assert_nRESET_DRV();  
    
  // Delay ~2 msec (only 20 uS req'd).
  Delay(2);  
  
  // De-assert reset line.  
  Deassert_nRESET_DRV();  
}  

