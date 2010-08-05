
; -------------------------------------------------------
;
;       56F80xx_init.asm
;
;	Metrowerks, a Freescale Company
;	sample code
;
;	description:  main entry point to C code.
;                 setup runtime for C and call main
;
; -------------------------------------------------------

;===============================

; OMR mode bits

;===============================

NL_MODE                                                          EQU  $8000
CM_MODE                                                          EQU  $0100
XP_MODE                                                          EQU  $0080
R_MODE                                                           EQU  $0020
SA_MODE                                                          EQU  $0010

;===============================

; init

;===============================

        section startup

        XREF  F_stack_addr
        XREF  F_xROM_to_xRAM
        XREF  F_pROM_to_xRAM
        XREF  F_Ldata_size
        XREF  F_Ldata_ROM_addr
        XREF  F_Ldata_RAM_addr
        org   p:


        GLOBAL Finit_56800_

        SUBROUTINE "Finit_56800_",Finit_56800_,Finit_56800_END-Finit_56800_

Finit_56800_:

;
; setup the OMr with the values required by C
;
        bfset #NL_MODE,omr    ; ensure NL=1  (enables nsted DO loops)
        nop
        nop
        bfclr #(CM_MODE|XP_MODE|R_MODE|SA_MODE),omr               ; ensure CM=0  (optional for C)
                                                                  ; ensure XP=0 to enable harvard architecture
                                                                  ; ensure R=0  (required for C)
                                                                  ; ensure SA=0 (required for C)


; setup the m01 register for linear addressing

        move.w  #-1,x0
        moveu.w x0,m01                  ; set the m register to linear addressing

        moveu.w hws,la                  ; clear the hardware stack
        moveu.w hws,la
        nop
        nop


CALLMAIN:                               ; initialize compiler environment

                                        ; initialize the Stack
        move.l  #>>F_Lstack_addr,r0
        bftsth  #$0001,r0
        bcc     noinc
        adda    #1,r0
noinc:
        tfra    r0,sp                   ; set stack pointer too
        move.w  #0,r1
        nop
        move.w  r1,x:(sp)
        adda    #1,sp


; Stationery default routine
; 56800E LC register doesn't have 8191 element limitation of 56800
; so we can use hardware loop here

; zeroBSS hardware loop utility
        moveu.w #>F_Xbss_length,r2      ; move count
        moveu.w #>F_Xbss_start_addr,r1  ; start address is BSS start
        move.w  #0,x0                   ; zero value
        rep     r2                      ; copy r2 times		
        move.w  x0,x:(r1)+              ; stash value at x address r1

; pROM-to-pRAM utility
        moveu.w #>Fpflash_mirror,r2     ; src address -- pROM mirror
        moveu.w #>Fpram_start,r3        ; dest address -- pRAM data start
        move.w  #>Fpram_size,y0         ; set pRAM size
        do      y0,>LOOP_INIT_PRAM      ;
        move.w  p:(r2)+,x0              ; Copy PFLASH to X0
	    nop
	    nop
	    nop
        move.w  x0,x:(r3)+              ; Copy X0 to URAM
;        nop                             ; Stall (needed for HW loop)
;        nop                             ; Stall (needed for HW loop)
LOOP_INIT_PRAM:

; pROM-to-xRAM utility
        moveu.w #>Fpflash_mirror2,r2    ; src address -- pROM mirror 2
        moveu.w #>Fxram_start,r3        ; dest address -- xRAM data start
        move.w  #>Fxram_size,y0         ; set pRAM size
        do      y0,>LOOP_INIT_XRAM      ;
        move.w  p:(r2)+,x0              ; Copy PFLASH to X0
	    nop
	    nop
	    nop
        move.w  x0,x:(r3)+              ; Copy X0 to URAM
;       nop                             ; Stall (needed for HW loop)
;       nop                             ; Stall (needed for HW loop)
LOOP_INIT_XRAM:

; call main()

        move.w  #0,y0                   ; pass parameters to main()
        move.w  #0,R2
        move.w  #0,R3

        jsr     Fmain                   ; call the user program

Finit_56800_END:
        debughlt
        bra    Finit_56800_END

        endsec
