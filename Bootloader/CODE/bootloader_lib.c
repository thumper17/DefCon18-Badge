/*****************************************************************************
* File:    bootloader_lib.c
*
* Purpose: Provide utilities for MC56F801x serial bootloader application.
* Dept:    Freescale TSPG MCD DSC Team (Tempe, AZ)
* Email:   support@freescale.com
* Date:    2005.08.15
******************************************************************************
* Notes:
*
*****************************************************************************/

/* Including used modules for compiling procedure */
#include "Cpu.h"
#include "SCI1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include custom modules */
#include "bootloader_lib.h"

#define SEE_INDICATOR


// Global Variables
UInt16 bytesum;
UInt16 status;
char *data_in = 0;
char *data_out;
char hex_string[5];
char rx_data[RX_DATA_SIZE];
UInt16 fisr_cntr;

// Function Prototypes
char get_char(char **ptr);
char int_to_char(UInt16 i);
char *int_to_string(UInt16 i);
void fisr_sci_rx_full(void);
void mem_init(register char *mem, register char init, register UInt16 sz);
void sci_tx(char *msg);
void sci_tx_char(char msg);
void srec_decode(void);
UInt16 char_to_int(char c);
UInt16 get_byte(char **ptr);
UInt16 hfm_command(UInt16 cmd, UInt16 *addr, UInt16 *data, UInt16 cnt);


bool is_scirx_char_ready()
{
	return (data_in != rx_data);
}


/*****************************************************************************
* UInt16 char_to_int(char c)
*
* Convert ASCII hexadecimal character into integer.
*
* In:  c - Character to convert
*
* Out: Returns integer representation of ASCII hexadecimal character.
*****************************************************************************/
UInt16 char_to_int(char c)
{
   UInt16 i = (UInt16) c;

   if ((c >= '1') && (c <= '9'))
      i -= '0';
   else if ((c >= 'A') && (c <= 'F'))
      i -= ('A' - 0xA);
   else
      i = 0;

   return (i);
}



#pragma section pram_code begin

/*****************************************************************************
* UInt16 hfm_command(int flash, int cmd, int *addr, int *data, int cnt)
*
* Write an array of data to flash memory.
* 
* In:  cmd  - HFM command
*      addr - Starting address
*      data - Interger array of data to write
*      cnt  - Number of words in array
*
* Out: Returns executed Flash Command code.
*****************************************************************************/
UInt16 hfm_command(UInt16 cmd, UInt16 *addr, UInt16 *data, UInt16 cnt)
{
   UInt16 fmustat;

       
   // Repeat HFM command process "cnt" times, exit on error.
   for (; (cnt>0 && !(status & DOWNLOAD_ERROR)); cnt--,addr++,data++) 
   {
      // Wait until Flash command buffer is empty before proceeding.
      while (!(FM_USTAT&FM_USTAT_CBEIF_MASK)){}

      // Initiate Flash command
      pmem_write(addr, *data);
      FM_CMD = cmd;
      FM_USTAT |= FM_USTAT_CBEIF_MASK;
      fmustat = FM_USTAT;
      
      if(fmustat & FM_USTAT_ACCERR_MASK)
      FM_USTAT = FM_USTAT_ACCERR_MASK;
      
      if(fmustat & FM_USTAT_PVIOL_MASK)
      FM_USTAT = FM_USTAT_PVIOL_MASK; 
 
      // Wait until Flash command is complete before proceeding.
      while (!(FM_USTAT&FM_USTAT_CCIF_MASK)){}

      // Verify successful programming (only used when programming)
      if (cmd==FLASH_WORD_PROGRAM && (pmem_read(addr) != *data)) 
      {
         status |= ERR_HFM_PROG;
      }
   }
 
   return (cmd);
}

/////////////////////////////////////////////////////////////////////////////
// A S S E M B L Y   R O U T I N E S
/////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
* void fisr_sci_rx(void)
*
* Fast Interrupt Service Routine for SCI Receive Data Full Interrupts
*
* In:  n/a
*
* Out: n/a
*****************************************************************************/
asm void fisr_sci_rx_full(void)
{
        move.w  x:(r1),y1           ;// SCISR must be read to clear RDRF flag
        move.w  x:(QSCI_DATA),y0    ;// Read SCIDR to get RX data
        bftstl  #0x0f00,y1          ;// Check SCISR for RX errors
        bcs     NO_RX_ERRORS        ;// Skip next 2 lines if no RX errors
        move.w  y1,x:(r1)           ;// Clear SCI RX error flags (OR,NF,PF,FE)
        bfset   #ERR_SCI_RX,status  ;// Track errors in "status"
        bra     EXIT_FISR           ;// Exit if RX errors detected
NO_RX_ERRORS:
        move.bp y0,x:(r0)+          ;// Write SCI RX data to circ buffer
        move.w  r0,data_in          ;// Put circ buffer index in "data_in"
EXIT_FISR:
        frtid                       ;// Delayed return from FISR
        nop                         ;// Stall
        nop                         ;// Stall
}

#pragma section pram_code end


/*****************************************************************************
* char int_to_char(UInt16 i)
* 
* Get ASCII character representation of a 4-bit integer value.
*
* In:  i - Integer to convert (4-bit)
* 
* Out: Returns ASCII value used to display 4-bit integer.
*
* Note: This function only uses i[3:0] in translation!
*****************************************************************************/
char int_to_char(UInt16 i)
{
   char j;

   i &= 0x000F;
   j = (char) ((i < 10) ? (i + '0'): (i - 10 + 'A'));

   return (j);
}


/*****************************************************************************
* char *int_to_string (UInt16 i)
*
* Convert integer into 4-character ASCII Hex format.
*
* Input:  i - Integer to convert.
*
* Output: Returns string containing converted value.
*****************************************************************************/
char *int_to_string (UInt16 i)
{
   char *s = hex_string;
   int j;

   for (j=12; j>=0; j-=4)
      *s++ = int_to_char(0xF & (i >> j));
   *s++ = '\0';
   
   return (hex_string);
}


/*****************************************************************************
* void sci_tx(char *msg)
*
* Transmit string characters until NULL character is reached.
*
* In:  *msg - Transmit Buffer
*
* Out: n/a
*****************************************************************************/
void sci_tx(char *msg)
{
   do {
      while (!(SCI_STAT & SCI_STAT_TDRE_MASK)){}
      SCI_DATA = *msg++; 
   } while (*msg);
}


/*****************************************************************************
* void sci_tx_char(char msg)
*
* Transmit single character over SCI port.
*
* In:  msg - Transmit Character
*
* Out: n/a
*****************************************************************************/
asm void sci_tx_char(register char msg)
{
WAIT_TDRE:
        brclr   #QSCI0_STAT_TDRE_MASK,x:>QSCI_STAT,>WAIT_TDRE
        move.w  msg,x:>QSCI_DATA
        rts
}


/*****************************************************************************
* void srec_decode(void)
*
* Parse S-record data contained in buffer.
*
* In:  n/a
*
* Out: n/a
*****************************************************************************/
void srec_decode(void)
{
   char type;
   int i;
   UInt16 bytes, data, checksum;
   UInt32 addr;
   
   // Process S-record "type", "byte count", and "address" fields
   get_char(&data_out);
   type = get_char(&data_out);
   bytesum = 0;
   bytes = get_byte(&data_out);
   for (addr=0,i=4; i>0; i--) {
      addr <<= 8;
      addr += (UInt32)get_byte(&data_out);
   }

   // Process S-record "data" field (content depends on "type" field)
   if (type == '0') {
   
      sci_tx("\r\n\0"); 
      for (i=((int)bytes)-4-1; i>0; i--) {
         sci_tx_char((char)get_byte(&data_out));
      }
                
     status |= RX_HEADER;

    	// For 56F800x, user space contains 27 pages of flash, 256 words/page starting from 0      
      for (i=0; i<NO_FLASH_PAGES_ERASE;i++)
      {
      	hfm_command(FLASH_PAGE_ERASE, (UInt16*)(i*FLASH_PAGE_SIZE+PFLASH_START_ADDR), 0, 1);
      }      
   }
   else if (type == '3') 
   {
      for (i=((int)bytes)-4-1; i>0; i-=2) 
      {
         data = get_byte(&data_out);
         data += (i>1) ? (get_byte(&data_out) << 8): 0xFF00u;
         if (addr <= FLASH_HI) 
         { 
	         fprogram(addr++, &data, 1);  
       	 }
        if (status & DOWNLOAD_ERROR) break;
      }
   }
   else if (type == '7') 
   { 
   
  	status |= RX_END_OF_FILE; 
  	fprogram(BOOT_START_ADDRESS_PLACE, &addr, 1);	// save new application start address read from S7 record into 
  																								// BOOT_START_ADDRESS_PLACE
  }
   else  { status |= ERR_SREC_TYPE; }

   // Process S-record "checksum" field (compare with calculated value).
   if (!(status & DOWNLOAD_ERROR)) {
      checksum = (~bytesum) & 0xFF;
      if (checksum != get_byte(&data_out)) { 
         status |= ERR_SREC_CKSUM; 
      }

      // Discard EOL characters
      get_char(&data_out);
   }

   // Use "*" to indicate progress
#ifdef SEE_INDICATOR   
   sci_tx_char('*');
#endif   
}



/*****************************************************************************
* UInt16 get_byte(char **ptr)
*
* Get two characters from circular buffer and returns the ASCII hex value.
* Post increments character pointer using modulo arithmetic.  Accumulates 
* ASCII hex values in "bytesum" global.
* 
* In:   ptr - Pointer to location in circular buffer
*
* Out:  Returns ASCII hexadecimal value of two characters in buffer.
*****************************************************************************/
asm UInt16 get_byte(register char **ptr)
{
        adda    #2,sp                   ;//
        move.w  m01,x:(sp-1)            ;// Save M01 to stack
        moveu.w #(RX_DATA_SIZE-1),M01   ;// Set buffer size
        moveu.w x:(ptr),r0              ;// Get circular buffer pointer
        move.bp x:(r0)+,y0              ;// Get 1st character (high nibble)
        jsr     char_to_int             ;// Convert into integer
        asll.w  #<4,y0                  ;// Shift to account for high nibble
        move.w  y0,y1                   ;// Accumulate result into Y1
        move.bp x:(r0)+,y0              ;// Get 2nd character (low nibble)
        jsr     char_to_int             ;// Convert into integer
        add     y1,y0                   ;// Accumulate result into Y0 (return value)
        add.w   y0,bytesum              ;// Accumulate result into global
        move.w  r0,x:(ptr)              ;// Update circular buffer pointer
        move.w  #bytesum+2,x0
        moveu.w x:(sp-1),m01            ;// Restore M01
        suba    #2,sp                   ;// Restore stack pointer
        rts
}


/*****************************************************************************
* char get_char(char **ptr)
*
* Get a character from circular buffer.  Post increments character pointer 
* using modulo arithmetic.
*
* In:   ptr - Pointer to location in circular buffer
*
* Out:  Returns character in circular buffer.
*****************************************************************************/
asm char get_char(register char **ptr)
{
        adda    #2,sp                   ;//
        move.w  m01,x:(sp-1)            ;// Save M01 to stack
        moveu.w #(RX_DATA_SIZE-1),M01   ;// Set buffer size
        moveu.w x:(ptr),r0              ;// Get circular buffer pointer
        move.bp x:(r0)+,y0              ;// Get 1st character
        move.w  r0,x:(ptr)              ;// Save updated pointer
        moveu.w x:(sp-1),m01            ;// Restore M01
        suba    #2,sp                   ;// Restore stack pointer
        rts
}

/*****************************************************************************
* void mem_init(register char *mem, register char init, register UInt16 sz)
*
* Initialize a string with desired character.
*
* In:  mem - String to initialize
* 
* Out: n/a
*****************************************************************************/
asm void mem_init(register char *mem, register char init, register UInt16 sz)
{
        rep     sz
        move.bp init,x:(mem)+
        rts
}


