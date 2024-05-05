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
SDCSWFN:            EQU   0x06 ; write file, send name  
SDCSWFD:            EQU   0x08 ; write file, send data  
SDCSRFN:            EQU   0x02 ; read file, send name  
SDCSRFD:            EQU   0x04 ; read file, read data  
SDCSDIRFN:          EQU   0x0a ; list send name ('\0' is current dir)  
SDCSDIR:            EQU   0x0c ; read list data  
SDCSDFN:            EQU   0x0e ; delete file, send name  
SDCSRENFN1:         EQU   0x10 ; rename, send source  
SDCSRENFN2:         EQU   0x12 ; rename, send dest  
SDCSCPYFN1:         EQU   0x14 ; copy, send source  
SDCSCPYFN2:         EQU   0x16 ; copy, send dest  
SDCSEXISFN:         EQU   0x18 ; exist file?, send name  
SDCSEXIST:          EQU   0x1a ; exist file?, read data  
SDCSMKDFN:          EQU   0x1c ; mkdir, send name  
SDCSRMDFN:          EQU   0x1e ; rmdir, send name  
SDCSCHDFN:          EQU   0x20 ; chdir, send name  
SDCSCWD:            EQU   0x22 ; cwd, read data (full path name)  
SDCSFOFN:           EQU   0x24 ; file open file name  
SDCSFOFM:           EQU   0x26 ; file open file mode  
SDCSCFOGH:          EQU   0x28 ; file open get handle  
SDCSCFHDL:          EQU   0x2a ; file close handle  
SDCSFWHDL:          EQU   0x2c ; file write handle  
SDCSFWRITE:         EQU   0x2e ; file write  
SDCSFWSTAT:         EQU   0x30 ; file write status  
SDCSFRHDL:          EQU   0x32 ; file read handle  
SDCSFREAD:          EQU   0x34 ; file read  
SDCSFRSTAT:         EQU   0x36 ; file read status  
SDCSFGPHDL:         EQU   0x38 ; file get position handle  
SDCSFGPOS:          EQU   0x3a ; file get position  
SDCSFSSHDL:         EQU   0x3c ; file seekset handle  
SDCSSEKSET:         EQU   0x3e ; file seekset  
SDCSFSSSTAT:        EQU   0x40 ; file seekset status  
SDCSFSCHDL:         EQU   0x42 ; file seekcur handle  
SDCSSEKCUR:         EQU   0x44 ; file seekcur  
SDCSFSCSTAT:        EQU   0x46 ; file seekcur statuts  
SDCSFSEHDL:         EQU   0x48 ; file seekend handle  
SDCSSEKEND:         EQU   0x4a ; file seekend  
SDCSFSESTAT:        EQU   0x4c ; file seekend status  
SDCSFRWDHDL:        EQU   0x4e ; file rewind handle  
SDCSFPKHDL:         EQU   0x50 ; file peek handle  
SDCSFPEEK:          EQU   0x52 ; file peek  
SDCSFWBHDL:         EQU   0x54 ; file writebytes handle  
SDCSFWRITEB:        EQU   0x56 ; file writebytes  
SDCSFWBSTAT:        EQU   0x58 ; file writebytes status  
SDCSFRBHDL:         EQU   0x5A ; file readbytes handle  
SDCSFREADB:         EQU   0x5C ; file readbytes  
SDCSFRBSTAT:        EQU   0x5E ; file readbytes status  

; sdcard io commands start  
SDCMDRESET:          EQU   0x0f  
SDCMDLOAD:           EQU   0x0d  
SDCMDSAVE:           EQU   0x0c   
SDCMDRWEND:          EQU   0x0b  
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
SDCMDFWRITEB:        EQU   0x2A  
SDCMDFREADB:         EQU   0x2B  

# operations status and command codes
https://docs.google.com/spreadsheets/d/1EDnzh6c8GuFteZskviRQ0HXl_1hdd2McFDgUcx4P_4A/edit?usp=sharing  
