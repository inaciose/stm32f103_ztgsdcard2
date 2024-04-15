# ztgsdcard2

ztgsdcard2 - Z80 - RC204 - SD card board - STM32 firmware

This repository have old board firmware based on SD library replaced by a new firmware based on SdFat library. 
The old firmware is available at: https://github.com/inaciose/stm32_ard_z80sd

The z80 computer system (SBC) must:
- run a driver (exploration software) available at: https://github.com/inaciose/z80asm/tree/main/rc2014sdcard
- have a bord (ztgsdcard) described at: https://github.com/inaciose/rc2014ss/tree/main/rcsdcard

Some notes about the required board: 
- There are two rc 2014 Z80 compatible boards, the main funcional difference is V2 having an integrated 8KB ROM. 
- The version without rom need to run the cli/driver at 0x8000, the other run at 0x2000. 
- The board v2 also use GALs in order to reduce the IC count. 

# ztgsdcard2 v1.00

it is only a simple rewrite of the previous STM32 firmware for the RC2014 Z80 SD card board (ztgsdcard) in order to use another SD card library.
This version only have the changes required to migrate the firmware from the SD library to the SdFat library. The changes are  Nothing is added or removed.

# ztgsdcard2 v1.0x

The firmware is under development. There is many things changing. See the version on the header of the main.cpp.


# basic info about the ztgsdcard interface 

; sdcard io addresses
SDCRS:              EQU   0x40   
SDCRD:              EQU   0x41   
SDCWC:              EQU   0x40   
SDCWD:              EQU   0x41 

; sdcard io status
SDCSIDL:            EQU   0x00 ;  
SDCSWFN:            EQU   0x10 ; write file, send name 
SDCSWFD:            EQU   0x12 ; write file, send data
SDCSRFN:            EQU   0x08 ; read file, send name 
SDCSRFD:            EQU   0x0a ; read file, read data
SDCSDIRFN:          EQU   0x20 ; list send name ('\0' is current dir)
SDCSDIR:            EQU   0x22 ; read list data
SDCSDFN:            EQU   0x28 ; delete file, send name
SDCSRENFN1:         EQU   0x30 ; rename, send source 
SDCSRENFN2:         EQU   0x38 ; rename, send dest
SDCSCPYFN1:         EQU   0x40 ; copy, send source 
SDCSCPYFN2:         EQU   0x48 ; copy, send dest
SDCSEXISFN:         EQU   0x80 ; exist file?, send name 
SDCSEXIST:          EQU   0x82 ; exist file?, read data 
SDCSMKDFN:          EQU   0x50 ; mkdir, send name
SDCSRMDFN:          EQU   0x58 ; rmdir, send name
SDCSCHDFN:          EQU   0x78 ; chdir, send name
SDCSCWD:            EQU   0x98 ; cwd, read data (full path name)
SDCSFOFN:           EQU   0xa0 ;
SDCSFOFM:           EQU   0xa8 ;
SDCSCFOGH:          EQU   0xb0 ;
SDCSFOIDL:          EQU   0xb2 ;
SDCSCFHDL:          EQU   0xb8 ;
SDCSFWHDL:          EQU   0xc0 ;
SDCSFWRITE:         EQU   0xc2 ;
SDCSFWSTAT:         EQU   0xc4 ;
SDCSFRHDL:          EQU   0xc8 ;
SDCSFREAD:          EQU   0xca ;
SDCSFRSTAT:         EQU   0xcc ;
SDCSFGPHDL:         EQU   0xd0 ;
SDCSFGPOS:          EQU   0xd2 ;
SDCSFSSHDL:         EQU   0xd8 ;
SDCSSEKSET:         EQU   0xda ;
SDCSFSSSTAT:        EQU   0xdc ;
SDCSFSCHDL:         EQU   0xe0 ;
SDCSSEKCUR:         EQU   0xe2 ;
SDCSFSCSTAT:        EQU   0xe4 ;
SDCSFSEHDL:         EQU   0xe8 ;
SDCSSEKEND:         EQU   0xea ;
SDCSFSESTAT:        EQU   0xec ;
SDCSFRWDHDL:        EQU   0xee ;
SDCSFPKHDL:         EQU   0xf0 ;
SDCSFPEEK:          EQU   0xf2 ;

; sdcard io commands start
SDCMDRESET:          EQU   0x0f
SDCMDLOAD:           EQU   0x0d
SDCMDSAVE:           EQU   0x0c
;SDCMDWRITE:          EQU   0x
SDCMDWREND:          EQU   0x0b
SDCMDLIST:           EQU   0x0e
SDCMDDEL:            EQU   0x0a
SDCMDREN:            EQU   0x10
SDCMDCOPY:           EQU   0x11
SDCMDEXIST:          EQU   0x12
SDCMDMKDIR:          EQU   0x13
SDCMDRMDIR:          EQU   0x14
SDCMDCD:             EQU   0x15
SDCMDCWD:            EQU   0x16
SDCMDFOPEN:          EQU   0x20
SDCMDFCLOSE:         EQU   0x21
SDCMDFWRITE:         EQU   0x22
SDCMDFREAD:          EQU   0x23
SDCMDFGPOS:          EQU   0x24
SDCMDFSEKSET:        EQU   0x25
SDCMDFSEKCUR:        EQU   0x26
SDCMDFSEKEND:        EQU   0x27
SDCMDFREWIND:        EQU   0x28
SDCMDFPEEK:          EQU   0x29