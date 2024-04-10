# stm32f103_ztgsdcard2
ztgsdcard2 - Z80 - RC204 - SD card board - STM32 firmware

This repository have old board firmware based on SD library replaced by a new firmware based on SdFat library.
The old firmware is available at: https://github.com/inaciose/z80asm/tree/main/rc2014sdcard

The z80 computer system (SBC) must:
- run a driver (exploration software) available at: https://github.com/inaciose/z80asm/tree/main/rc2014sdcard
- have a bord (ztgsdcard) described at: https://github.com/inaciose/rc2014ss/tree/main/rcsdcard

 There are two boards, the main funcional difference is V2 having an integrated 8KB ROM. 
 It also use GALs in order to reduce the IC count.

# ztgsdcard2 v1.00
Is a rewrite of the previous STM32 firmware for the RC2014 Z80 SD card board (ztgsdcard).
The rewrite its to convert it from the SD library to the SdFat library. The changes are  Nothing is added or removed.

