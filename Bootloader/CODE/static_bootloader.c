/** ########################################################################################################################
**     Filename  : bootloader.C
**     Project   : bootloader
**     Processor : MC56F8006_32_LQFP
**     Version   : Driver 01.10
**     Compiler  : Metrowerks DSP C Compiler
**     Date/Time : 4/13/2006, 2:06 PM
**     Abstract  : Static Serial bootloader for 56F800x/56F802x/3x. 
**     Author:	   William Jiang,Petr Stancik
**     Description:
				This bootloader configures SCI rx interrupt as fast interrupt, so the last vector in the vector table 
				should not contain any JSR/BSR to the fast interrupt service routine, instead, use JMP to
				the fast interrupt service routine as below:
				  JMP  fisr_sci_rx_full                // Interrupt no. 50 (Unused) - ivINT_USER6  
				
				The CPU interrupt must be disabled shadow registers initialization.
				
 	   		    It also requires modification of user's application code; 
 				on _EntryPoint function (after vector table) the folowing sequence must be
 				placed:
				extern unsigned int _vba;
				INTC_VBA = (int)&_vba >>7;  // restore VBA register to point to user defined vector table
 				vba is defined in the linker command file as below:
			        .interrupt_vectors :
			        {
			          # interrupt vectors
			          F_vba = .;
			          * (interrupt_vectors.text)
			        } > .p_Interrupts
			    where interrupt_vectors.text is defined as below
				#pragma define_section interrupt_vectors "interrupt_vectors.text"  RX
				#pragma section interrupt_vectors begin
				static asm void _vect(void) {
				  JMP  0x1B00          # Hardware reset vector must jump to 0x1B00          
				  JMP  0x1B00          # COP reset vector must jump to 0x1B00           
				  JSR  Cpu_Interrupt                   
				  JSR  Cpu_Interrupt                   
				  JSR  Cpu_Interrupt                  
				  JSR  Cpu_Interrupt  
				  ...
					}
				 #pragma section interrupt_vectors end
				                  
			    NOTE: 
			    The above first two reset vectors must replaces those defined by ProcessorExpert or CodeWarrior.   	
    			For 56F802x, the program flash starts from 0x4000, so the .p_Interrupts segment is defined as below:
    		    .p_Interrupts  (RWX) : ORIGIN = 0x00004000, LENGTH = 0x00000080

    			For 56F803x, the program flash starts from 0, so the .p_Interrupts segment is defined as below:
    		    .p_Interrupts  (RWX) : ORIGIN = 0x0000000, LENGTH = 0x00000080
				Users can change the starting address of the vector table to fit their applications.
  				
 				Because the bootloader reads boot delay value
 				from address 0x1BFF,users must define the boot delay value in 0x1BFF by adding a memory segment .xBootCfg 
 				and a memory section .ApplicationConfiguration  in
 				the linker command file  
 				as below:
     			#Serial bootloader configuration section
    			.xBootCfg (RWX) : ORIGIN = 0x00001AFE, LENGTH = 0x2
 				
 
 
 				#Serial bootloader configuration section
		        .ApplicationConfiguration :
		        {
		        	  # Application Entry Point must be written here
		              WRITEH(F_EntryPoint);
		              
		              # Bootloader start delay in seconds
		              WRITEH(10);      # 10 is the boot delay value in seconds before bootloader
		              				   # starts up the application code
		              
		        } > .xBootCfg
 		
**		Change History:	
**			
**			Jan. 24,2008	removed some characters and outputs to reduce the code size in flash
**							so as to fix the issue of overwriting flash configuration area.
**			Nov. 28,2008	Added support for 56F800x
**
** #########################################################################################################################
*/

// Lots o' modifications by Joe Grand (Kingpin) for the DEFCON 18 Badge


/* MODULE bootloader */

/* Including used modules for compiling procedure */
#include "Cpu.h"
#include "Events.h"
#include "SCI1.h"
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* Include custom modules */
#include "bootloader_lib.h"


UWord32  StartAddress;
//UWord16  StartDelay;
char address[4];

char StrCopyright[] = "\n\n\r\
DEFCON 18 Badge by Joe Grand (Kingpin)\n\r\
      [www.grandideastudio.com]\n\r\
--------------------------------------\n\r"; 
char StrWaitingSrec[]   = "Serial bootloader started.\r\nWaiting for application S-Record.";
char StrLoaded[]        = "Download complete.\0";
char StrStarted[]       = "Starting user application.\0"; // NOTE: the last ending byte 0 is added 
														// to workaround the compiler issue with
														// even # of chars in the string that 
														// will not be padded with 0 by the compiler
char StrNewLine[]       = "\r\n\0";
char StrErr[]           = "\r\nError: ";
char StrTimeout[]				= " Timeout!";

void bootTimerCheck(void);
void bootExit(void);
void bootTimerInit(void);

// Define page buffer to store the last page containing flash configuration words
#pragma define_section flash_page_section "flash_page.page"  RW
#pragma section flash_page_section begin
word page_buf[FLASH_PAGE_SIZE];
#pragma section flash_page_section end

/*****************************************************************************
* void main(void)
*
* Main application.
*
* In:  n/a
*
* Out: n/a
*****************************************************************************/
void main(void)
{
  UInt16 state;
  char *temp_ptr;
  extern word _vba;
  uint8_t sw0, sw1;
  
  
  INTC_VBA = ((word)&_vba) >> (21-INTC_VBA_BITS);			   /* Set Vector Base Address */
  
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
    
    /*StartDelay = pmem_read((unsigned int *)BOOT_START_DELAY_PLACE) & 0x00FF;
    if(!StartDelay)  bootExit();
    else bootTimerInit();*/ 
   
    /* Write your code here */

   GPIO_A_PER &= ~GPIO_A_PER_PE0_MASK; // set to GPIO mode
   GPIO_A_PER &= ~GPIO_A_PER_PE1_MASK;
   GPIO_A_DDR |= GPIO_A_DDR_DD0_MASK;  // set as output
   GPIO_A_DDR |= GPIO_A_DDR_DD1_MASK;
   GPIO_A_DR &= ~GPIO_A_DR_D0_MASK;  // set low
   GPIO_A_DR &= ~GPIO_A_DR_D1_MASK; 			
			    
    Cpu_Delay100US(5000); // power-up delay
  
   // Initialize R0,R1,M01, and N for circular buffer (including shadows)
   asm {
        swap shadows                    ;// Switch to shadow registers
        moveu.w #(RX_DATA_SIZE-1),M01   ;// Set buffer size
        moveu.w #rx_data,R0             ;// Set starting address
        moveu.w #QSCI_STAT,R1           ;// Use R1 for SCI status register
        moveu.w #0,N                    ;// N is unused
        swap shadows                    ;// Switch back to normal registers
   }

		// with DEFCON 17, we used a timer to keep the badge in bootloader mode on power up for 10 seconds
		// for DEFCON 18, the bootloader mode is only enabled if both buttons are held down on power up
		// otherwise it jumps directly to the user application. no timer necessary, so that code is commented out
		
		// check buttons
   GPIO_C_PUR |= GPIO_C_PUR_PU0_MASK; // enable pull-up
   GPIO_C_PUR |= GPIO_C_PUR_PU1_MASK;   
   GPIO_C_DDR &= ~GPIO_C_DDR_DD0_MASK; // set as input
   GPIO_C_DDR &= ~GPIO_C_DDR_DD1_MASK;
   GPIO_C_PER &= ~GPIO_C_PER_PE0_MASK; // set to GPIO mode
   GPIO_C_PER &= ~GPIO_C_PER_PE1_MASK;
   Cpu_Delay100US(1000); // setup delay
   
   #define SW0_PIN_MASK       ((byte)1) /* Pin mask */
   #define SW1_PIN_MASK       ((byte)2) /* Pin mask */
	 sw0 = !((bool)(getRegBits(GPIO_C_RAWDATA,SW0_PIN_MASK)));
   sw1 = !((bool)(getRegBits(GPIO_C_RAWDATA,SW1_PIN_MASK)));

   if (sw0 || sw1) // one or more buttons has been pressed
	 {
		 Cpu_Delay100US(1000); 	// give the user time to press both buttons (also serves as 100mS debounce)

	 	 sw0 = !((bool)(getRegBits(GPIO_C_RAWDATA,SW0_PIN_MASK)));
     sw1 = !((bool)(getRegBits(GPIO_C_RAWDATA,SW1_PIN_MASK)));
	 }
	
   if (sw0 && sw1) // if both buttons have been pressed, enter bootloader
   {
     // Turn on LEDs so the user knows we're here
     GPIO_A_DR |= GPIO_A_DR_D0_MASK;  // set high
   	 GPIO_A_DR |= GPIO_A_DR_D1_MASK;  
   
	   // Initialize globals
	   mem_init(rx_data, NULL, RX_DATA_SIZE);
	   data_out = data_in = rx_data; 
	   
	   state = INITIAL_STATE;
	   temp_ptr = rx_data;

	   // Disable protection
	   setReg(FM_PROT,BOOT_PROT_VALUE);

	   // Output banner
	   sci_tx(&StrCopyright[0]);
	   
	   // Now it is safe to enable interrupts
	   Cpu_EnableInt();

	   do  {
	      // State: Initial State
	      if (state == INITIAL_STATE) {
	         status = 0;
	         sci_tx(&StrWaitingSrec[0]);
	         sci_tx_char(XON);
	         state = WAIT_FOR_S;
	      }

	      if (data_in != temp_ptr) {
	         //Timer_Disable();

	         // State: Wait for Header "S"
	         if (state == WAIT_FOR_S) {
	            temp_ptr = data_out;
	            if (get_char(&temp_ptr) == 'S') { state = WAIT_FOR_0; }
	            else { get_char(&data_out); }
	         }
	         // State: Wait for Header "0"
	         else if (state == WAIT_FOR_0) {
	            if (get_char(&temp_ptr) == '0') { state = WAIT_FOR_EOL; }
	            else {
	               get_char(&data_out);
	               state = WAIT_FOR_S;
	            }
	         }
	         // State: Wait for EOL
	         else if (state == WAIT_FOR_EOL) {
	            if (get_char(&temp_ptr) == '\r') {
	               if (!(status & TX_XOFF)) {
	                  sci_tx_char(XOFF);
	                  status |= TX_XOFF;
	               }
	               srec_decode();
	               temp_ptr = data_out;
	            }
	         }
	      }
	      else {
	         if (status & TX_XOFF) {
	            sci_tx_char(XON);
	            status &= ~TX_XOFF;
	         }
	      }
	      // State: Error
	      if (status & DOWNLOAD_ERROR) {
	         sci_tx(StrErr);
	         sci_tx(int_to_string(status));
	         state = INITIAL_STATE;
	         sci_tx(StrNewLine);
	      }
	      
	      //bootTimerCheck();
	      
	   } while (status != DOWNLOAD_OK);
	   
	   sci_tx(StrNewLine); 
	   sci_tx(&StrLoaded[0]);
   }
   
   bootExit();
}

/***************************************************************************
* Functions:   bootTimerCheck()
*
* Description: Check TIMER specified in Timer bean
*
* Returns:     1 - timer is reached defined timevalue
*              0 - count in progress
*
* Notes:
*
**************************************************************************/
/*void bootTimerCheck(void)
{
    if(TMR1_SCTRL & TMR1_SCTRL_TCF_MASK) // is true every 1 second
    {
        TMR1_SCTRL&=~TMR1_SCTRL_TCF_MASK;

        if (--StartDelay == 0) 
        {
        	Timer_Disable();
        	sci_tx(&StrTimeout[0]);
          bootExit();	
        }
    }
}*/

/***************************************************************************
* Functions:   bootExit()
*
* Description: read the new appliaction start address
*              and start user's application

* Returns:     None
*
* Notes:
*
**************************************************************************/

void bootExit(void)
{
    int i=0;
    
    sci_tx(StrNewLine); 
    sci_tx(&StrStarted[0]);
    sci_tx(StrNewLine); 

    while(!getRegBit(SCI_STAT,TIDLE)){}; // wait until all data has been sent

	  // Disable all interrupts
	  Cpu_DisableInt();
    
    // Disable timer
    //Timer_Disable();
    
    // Restore Vector Base Address (VBA)
    INTC_VBA = 0;
    
    // Restore GPIO A register to reset value
    GPIO_A_DDR &= ~GPIO_A_DDR_DD0_MASK; // set low
    GPIO_A_DDR &= ~GPIO_A_DDR_DD1_MASK;  	
    
    
  /****************************************************************************
  / read user start address
  /****************************************************************************/
    StartAddress = pmem_read((unsigned int *)BOOT_START_ADDRESS_PLACE);

    asm(move.l StartAddress, N);
    asm(jmp (N));
}


/*void bootTimerInit(void)
{   
	// Enable timer clock
	SIM_PCE |= (SIM_PCE_TA1_MASK | SIM_PCE_TA0_MASK) ;
	
	// Use 32-bit cascaded timer counter
	TMR0_LOAD = 0;
	TMR0_CNTR = 0;
	TMR0_COMP1 = 31250;		// generate 125ms
	                        	
	TMR1_LOAD = 0;   
	TMR1_CNTR = 0; 
	TMR1_COMP1 = 0; // for 4MHz clock
	
	// Set up the cascaded timer 1 & 0
		
	TMR1_CTRL = 0xE820; 	// enabled  timer 1 as cascaded counter    
		
	TMR0_CTRL = 0x3E20;     // enabled  timer 0      
                            // [12:09]  = 1111 Count Source IPbus clock divide by 128    
							// [8:7]    = 00                                             
							// [6]      = 0 ONCE Count repeatedly                       
							// [5]      = 1 LENGTH Roll over                            
							// [4]      = 0 DIR Count up                                 
							// [3]      = 0                                              
							// [2:0]    = 000 OFLAG asserted      
}*/


/* END bootloader */
/*
** ###################################################################
**
**     This file was created by UNIS Processor Expert 2.98 [03.78]
**     for the Freescale 56800 series of microcontrollers.
**
** ###################################################################
*/
