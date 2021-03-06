## ###################################################################
##
##     THIS BEAN MODULE IS GENERATED BY THE TOOL. DO NOT MODIFY IT.
##
##     Filename  : 56F8006_bootloader.cmd
##
##     Project   : bootloader
##
##     Processor : MC56F8006_48_LQFP
##
##     Compiler  : Metrowerks DSP C Compiler
##
##     Date/Time : 11/26/2008, 4:27 PM
##
##     Abstract  :
##
##     This file is used by the linker. It describes files to be linked,
##     memory ranges, stack size, etc. For detailed description about linker
##     command files see CodeWarrior documentation. This file is generated by default.
##     You can switch off generation by setting the property "Generate linker file = no"
##     in the "Build options" tab of the CPU bean and then modify this file as needed.
##
##     (c) Copyright UNIS, a.s. 1997-2008
##     UNIS, a.s.
##     Jundrovska 33
##     624 00 Brno
##     Czech Republic
##     http      : www.processorexpert.com
##     mail      : info@processorexpert.com
##
## ###################################################################

###############################################################################
# The following section determines how Flash and RAM will be allocated for the
# application.  It will require modification in the event the software is 
# modified.  The nature in how each segment is allocated is cumbersome, however
# it is necessary to ensure program flash is contiguous.  A contiguous memory
# footprint for the application is desired, since it facilliates programming
# silicon in the field.
###############################################################################
MEMORY {
    .pIntvectorBoot     (RWX):  ORIGIN=0x0000,  LENGTH=0x0004
    .pFlash             (RWX):  ORIGIN=0x1B00,  LENGTH=0x044F	# do not overlap with flash configuration words
    .xRAM_bss           (RW):   ORIGIN=0x0000,  LENGTH=0x0126   # must align to RX_DATA_SIZE for modulo addressing
    .xRAM_data          (RWX):  ORIGIN=0x0126,  LENGTH=0x008C
    .pRAM               (RWX):  ORIGIN=0x8180,  LENGTH=0x00CC	# must align to 0x80 for vector table
    .xRAM               (RW):   ORIGIN=0x024C,  LENGTH=0x01B4    
    .xPeripherals       (RW):   ORIGIN=0xF000,  LENGTH=0x0000
}


KEEP_SECTION { interrupt_vectorsboot.text, interrupt_vectors.text }

SECTIONS {

    ###########################################################################
    # This following three sections contain program code intended for flash.
    # The reset vector is duplicated by the first two sections to be consistent
    # with other PE templates.
    ###########################################################################
    .interrupt_vectorsboot : {
        F_vector_addr = .;
        * (interrupt_vectorsboot.text)
        Fpflash_index = .;
    } > .pIntvectorBoot


    .ApplicationCode : {

        # Note: The function _EntryPoint should be placed at the beginning
        #       of the code
        OBJECT (F_EntryPoint, Cpu.c)
        
        # Remaining .text sections
        * (rtlib.text)
        * (startup.text)
        * (fp_engine.text)
        * (user.text)
        * (.text)

        # save address where for the data start in pROM
        Fpflash_mirror = .;
        Fpflash_index = .;
    } > .pFlash

    ###########################################################################
    # This section allocates memory in RAM for any zero-initiailized globals 
    # in the application.  Runtime code writes zero to each location of RAM
    # used by this section. 
    ###########################################################################
    .data_in_x_ram_bss : {
        # .bss sections
        F_Xbss_start_addr = .;
        _START_BSS = .;
        OBJECT (Frx_data, bootloader_lib.c)
        * (rtlib.bss.lo)
        * (rtlib.bss)
        * (.bss.char)         # used if "Emit Separate Char Data Section" enabled
        * (.bss)
        _END_BSS   = .;
        F_Xbss_length = _END_BSS - _START_BSS;    
    } > .xRAM_bss
    
    
    ###########################################################################
    # This section allocates memory in RAM for any program code in the 
    # application not intended for flash.  The opcodes are mirrored into Flash
    # and copied into RAM at runtime.
    ###########################################################################
    .prog_in_p_flash_ROM : AT(Fpflash_mirror) {
        Fpram_start = .;
        
        # This line defines the location of the interrupt vector table.  It is
        # used by "Cpu.C" to set the INTC_VBA register.
        F_vba = .;
        
        * (interrupt_vectors.text)						# must align vector table to 7 bits
        OBJECT(Ffisr_sci_rx_full,bootloader_lib.c)		# place FISR after vector table
        * (pram_code.text)
        . = ALIGN(2);
        
        # save data end and calculate data block size
        Fpram_end = .;
        Fpram_size = Fpram_end - Fpram_start;
        Fpflash_mirror2 = Fpflash_mirror + Fpram_size;
        Fpflash_index = Fpflash_mirror + Fpram_size;
    } > .pRAM

    ###########################################################################
    # This section allocates memory in RAM for any constants and non-zero
    # initialized global variables in the application.  The actual data values
    # are mirrored into Flash and copied into RAM at runtime.
    ###########################################################################
    .data_in_p_flash_ROM : AT(Fpflash_mirror2) {
                
        Fxram_start = .;
		
        * (.const.data.char)    # used if "Emit Separate Char Data Section" enabled
        * (.const.data)
        * (fp_state.data)
        * (rtlib.data)
        * (.data.char)          # used if "Emit Separate Char Data Section" enabled
        * (.data)
         . = ALIGN(2);

        # save data end and calculate data block size
        Fxram_end = .;
        Fxram_size = Fxram_end - Fxram_start;
        Fpflash_index = Fpflash_mirror2 + Fxram_size;
    } > .xRAM_data

    ###########################################################################
    # This section defines the stack pointer and heap
    ###########################################################################
    .data_in_x_ram : {

        /* Setup the HEAP address */
        . = ALIGN(4);
        _HEAP_ADDR = .;
        _HEAP_SIZE = 0x0000;
        _HEAP_END = _HEAP_ADDR + _HEAP_SIZE;
        . = _HEAP_END;

        /* SETUP the STACK address */
        _min_stack_size = 0x80;  # 0x01C0;
        _stack_addr = _HEAP_END;
        _stack_end  = _stack_addr + _min_stack_size;
        . = _stack_end;
          
        // store page buffer
        * (flash_page.page)

        __DATA_END=.;
    } > .xRAM

    # Peripheral registers for PESL
    FArchIO = ADDR(.xPeripherals);
    
    # Export HEAP and STACK to runtime libraries
    F_heap_addr    = _HEAP_ADDR;
    F_heap_end     = _HEAP_END;
    F_Lstack_addr  = _HEAP_END;
    F_StackAddr    = _HEAP_END;
    F_StackEndAddr = _stack_end - 1;

    # runtime code __init_sections uses these globals:
    F_Ldata_size     = 0;
    F_Ldata_RAM_addr = 0x0000;
    F_Ldata_ROM_addr = 0;
    F_xROM_to_xRAM   = 0x0000;
    F_pROM_to_xRAM   = 0x0001;
    F_start_bss      = _START_BSS;
    F_end_bss        = _END_BSS;
}
