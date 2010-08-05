/*****************************************************************************
* File:    bootloader_lib.h
*
* Purpose: Provide utilities for MC56F80xx serial bootloader application.
* Dept:    Freescale TSPG MCD DSC Team (Tempe, AZ)
* Email:   support@freescale.com
* Date:    2005.08.15
******************************************************************************
* Notes:
*
*****************************************************************************/

#ifndef _BOOTLOADER_LIB_H
#define _BOOTLOADER_LIB_H   1


#define BOOT_START_ADDRESS_PLACE 0x1AFE	
//#define BOOT_START_DELAY_PLACE   0x1AFF	
//#define Timer_Disable() TMR0_CTRL &= ~0x2000

// Receiver Buffer Info, Status Flags, and Error Codes
#define ERR_SREC_CKSUM      0x0001
#define ERR_SREC_TYPE       0x0002
#define ERR_HFM_ACCERR      0x0004
#define ERR_HFM_PVIOL       0x0008
#define ERR_HFM_PROG        0x0010
#define ERR_START_ADDR      0x0020
#define ERR_SCI_RX          0x0040
#define TX_XOFF             0x1000
#define RX_HEADER           0x2000
#define RX_END_OF_FILE      0x4000
#define DOWNLOAD_OK         (RX_HEADER|RX_END_OF_FILE)
#define DOWNLOAD_ERROR      0x00FF
#define RX_DATA_SIZE        550

// Flash Parameters and Functions
#define FLASH_PAGE_SIZE     256     // Flash Page Size (in words)
//#define FLASH_LO          0x0000  // Flash Boundary (Low)
//#define FLASH_SECURITY_L  2				// enable flash security
//#define FLASH_SECURITY_H  0x8000	// enable backdoor key access
//#define FLASH_LAST_PAGE	  0x1F00	// Last page address
#define BOOT_PROT_VALUE			0 			//0xC000	// Protect last two sectors starting from 0x1c00
#define FLASH_HI        		0x1AFF  // Flash Boundary (High) for applications 
												    				// since the bootloader occupies the range from 0x1C00 to 0x1FFF

//#define FLASH_SIZE       		0x2000  // Flash Block Size (in words)
//#define HFMCLKD_DIV     	 	0x1B    // HFM Clock Divider Value
//#define FLASH_ERASE_VERIFY  0x05
#define FLASH_WORD_PROGRAM    0x20
#define FLASH_PAGE_ERASE      0x40
//#define FLASH_MASS_ERASE    0x41

#define fmerase(x)          hfm_command(FLASH_MASS_ERASE, (UInt16*)FLASH_LO, 0, 1);
#define fperase(x)          hfm_command(FLASH_PAGE_ERASE, (UInt16*)x, 0, 1);
#define fprogram(x,y,z)     hfm_command(FLASH_WORD_PROGRAM, (UInt16*)x, (UInt16*)y, z);

// Peripheral Access
#define INTC_VBA_REG    0xF041	// INTC_VBA
#define QSCI_RATE       0xF0E0  // Formerly SCIBR SCI_RATE
#define QSCI_CTRL1      (QSCI_RATE+1)  // Formerly SCICR SCI_CTRL1
#define QSCI_CTRL2      (QSCI_RATE+2)  // Formerly SCICR2 SCI_CTRL2
#define QSCI_STAT       (QSCI_RATE+3)  // Formerly SCISR SCI_STAT
#define QSCI_DATA       (QSCI_RATE+4)  // Formerly SCIDR SCI_DATA
//	#define SCI_RX_ERR      (SCI_SCISR_OR|SCI_SCISR_NF|SCI_SCISR_FE|SCI_SCISR_PF)
#define QSCI0_STAT_TDRE_MASK	SCI_STAT_TDRE_MASK
	

// ASCII Character Values
#define XON             17
#define XOFF            19
#define NEWLINE         10
#define RETURN          13

// Application State Machine
#define INITIAL_STATE       ((UInt16)0)
#define WAIT_FOR_S          ((UInt16)1)
#define WAIT_FOR_0          ((UInt16)2)
#define WAIT_FOR_EOL        ((UInt16)3)
#define ERROR_HANDLER       ((UInt16)4)
#define APPLICATION_DONE    ((UInt16)5)
#define PROCESS_DATA        ((UInt16)8)

// Miscellaneous
#define bitset(x,y)     (y |= x)
#define bitclr(x,y)     (y &= ~x)


// Global Variables
extern UInt16 bytesum;
extern UInt16 status;
extern char *data_in;
extern char *data_out;
extern char hex_string[5];
extern char rx_data[RX_DATA_SIZE];

// Linker Command File Variables
extern unsigned int _vba;


// Function Prototypes
extern char get_char(char **ptr);
extern char int_to_char(UInt16 i);
extern char *int_to_string(UInt16 i);
extern void fisr_sci_rx_full(void);
extern void mem_init(register char *mem, register char init, register UInt16 sz);
extern void sci_tx(char *msg);
extern void sci_tx_char(char msg);
extern void srec_decode(void);
extern UInt16 char_to_int(char c);
extern UInt16 get_byte(char **ptr);
extern UInt16 hfm_command(UInt16 cmd, UInt16 *addr, UInt16 *data, UInt16 cnt);
extern bool is_scirx_char_ready();
//extern unsigned int pmem_read(register UInt16 *addr);

/////////////////////////////////////////////////////////////////////////////
// I N L I N E D   F U N C T I O N S
/////////////////////////////////////////////////////////////////////////////
inline unsigned int pmem_read(register UInt16 *addr)
{
   register unsigned int data;

   asm(move.w  p:(addr)+,data);
   return data;
}

inline void pmem_write(register UInt16* addr, register UInt16 data)
{
   asm(move.w  data,p:(addr)+n);
}


#pragma define_section pram_code "pram_code.text" RWX

#endif	BOOTLOADER_LIB_H
