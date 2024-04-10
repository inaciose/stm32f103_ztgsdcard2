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

