#include <Arduino.h>
//#include <SPI.h>
//#include <SD.h>

#include "SdFat.h"
#include "sdios.h"

//    SD card attached to the primary SPI as follows:
//        CS    = PA4;
//        SCK   = PA5;
//        MISO  = PA6;
//        MOSI  = PA7;


// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 3

//#define SDCARD_SS_PIN PA4

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else   // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif  // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
// only works at 16Mhz
#define SPI_CLOCK SD_SCK_MHZ(4)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else  // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif  // HAS_SDIO_CLASS

//------------------------------------------------------------------------------

#if SD_FAT_TYPE == 0
SdFat sd;
File file;
File root;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 file;
File32 root;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile file;
ExFile root;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile myfile;
FsFile root;
#endif  // SD_FAT_TYPE

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


// data pin (0, 7)
#define PD0 PB6
#define PD1 PB7
#define PD2 PB8
#define PD3 PB9
#define PD4 PB12
#define PD5 PB13
#define PD6 PB14
#define PD7 PB15

int dataPins[] = {PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7};

// select from 138 output
// request mode
#define WRD_N PA9     // Y5
#define WRC_N PA10    // Y4
#define RDD_N PA15    // Y3
#define RDS_N PB3     // Y2

// decoded cpu request
// RDS x RDD x WRC x WRD
#define CPUREQ_N PB11

// control wait line
#define WAIT_N PB4

// output enable 245 1=Z
#define OE_N PB10
#define OE_ENABLE LOW
#define OE_DISABLE HIGH

// output dir 245, 0=BA (in), 1=AB (out)
#define DIR PA8
#define DIR_IN 0
#define DIR_OUT 1

//volatile int outputEnable = 0;
//volatile int direction = DIR_IN;
volatile int processing = 0;

// enum state_t {IDLE_S, DIR_S, RFNAME_S, WFNAME_S, DFNAME_S, RFILE_S, WFILE_S, RFILE_E1_S, WFILE_E1_S, DIR_E1_S, DFILE_E1_S};
#define IDLE_S 0
#define RFNAME_S 3
//#define RFARG2_S 12
//#define IFARG3_S 130

#define WFNAME_S 2
//#define WFARG2_S 14
//#define WFARG3_S 15

#define DIR_S 16

#define RFILE_S 5
#define RFILE_E1_S 9

#define WFILE_S 4
#define WFILE_E1_S 8

#define DFNAME_S 32

#define DIR_E1_S 17
#define DFILE_E1_S 33

volatile int state;

char filename[64] = {0};
volatile int filename_count = 0;

// file list process
volatile char dir_lst[2048];
volatile int dir_idx = 0;
volatile int dir_max = 0;

// pin level operations
void setupPin();
void dPinModeInput();
void dPinModeOutput();
void byte2pin(int val);
int pin2byte();

int readFromDataBus();
void writeDataBus(int val);

// interrupt routines
void cpuReadStatusReq();
void cpuReadDataReq();
void cpuWriteCmdReq();
void cpuWriteDataReq();
void cpuEndRequest();

// sd card
void printDirectory(File dir, int numTabs);
void setupSDcard();

// led state
int pstate = 0;

int readFromDataBus() {

  // set stm32 data pins for input
  dPinModeInput();

  // set bus direction
  digitalWrite(DIR, DIR_IN);

  // output enable ( disable tristate )
  digitalWrite(OE_N, OE_ENABLE);

  int read = pin2byte();

  // output enable ( enable tristate )
  digitalWrite(OE_N, OE_DISABLE);

  return read;
}

void writeDataBus(int val) {

  // set stm32 data pins for output
  dPinModeOutput();

  // set bus direction
  digitalWrite(DIR, DIR_OUT);

  // write values to internal part of data bus
  byte2pin(val);

  // output enable ( disable tristate )
  digitalWrite(OE_N, OE_ENABLE);

  // let cpu go
  digitalWrite(WAIT_N, HIGH); 

  // data transfer is occuring.
  // we need to wait a litle before disable OE_N?

  // output enable ( enable tristate )
  digitalWrite(OE_N, OE_DISABLE);

  // set bus direction
  digitalWrite(DIR, DIR_IN);

  // reverse bus (not done in output proccess)
  dPinModeInput();
  
}

void setup() {

  setupPin();

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  //while (!Serial) {
  //  ; // wait for serial port to connect. Needed for native USB port only
  //}

  setupSDcard();

  /*
  Serial.println("SD list !");
  
  root.open("/");
  if(root) {
    printDirectory(root, 0);
    state = DIR_S;           
  } else {
    state = DIR_E1_S;
  }

  Serial.println((char *) dir_lst);
  Serial.println("SD list done.");
  */

  // directory list buffer init
  dir_lst[0] = '\0';
  
  state = IDLE_S;
}

void loop() {

  pstate = !pstate;
  digitalWrite(PC13, pstate);
  delay(500);

};


//
//
//


void cpuReadStatusReq() {
  /*
  if(processing) {
    Serial.println("RS on P1");
  }

  if(!digitalRead(RDS_N)) {
    Serial.println("RS");
  } else {
    Serial.println("-RS");
  }
  //Serial.println(" RS");
  //Serial.println(state);
  */
  //Serial.println("RS");

  processing = 1;

  writeDataBus(state);

  // let cpu go
  //digitalWrite(WAIT_N, HIGH);
  //moved to writeDataBus();

  processing = 0;

}

void cpuReadDataReq() {
  /*
  if(processing) {
    Serial.println("RD on P1");
  }

  if(!digitalRead(RDD_N)) {
    Serial.println("RD");
  } else {
    Serial.println("-RD");
  }
  //Serial.println(" RD");
  */
  //Serial.println("RD");

  processing = 1;

  char buf[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

  switch(state) {
    case RFILE_S:
      myfile.read(buf, 1);

      if(!myfile.available()) {
        // file end
        //Serial.println("RD file end");
        state = IDLE_S;
        myfile.close();
      }

      // dbg
      //Serial.println(buf);

      writeDataBus(buf[0]);
    break;

    case DIR_S:
      if(dir_idx < dir_max) { 
        writeDataBus(dir_lst[dir_idx++]); 
        if(dir_idx >= dir_max) {
          // end of dir list
          // reset file list
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;
          state = IDLE_S;         
        }
      } else {
        // we never come here
        //Serial.println("EDIR");
        // reset file list
        dir_lst[0] = '\0';
        dir_idx = 0;
        dir_max = 0;
        state = IDLE_S;
      }
    break;

    default:
      // nothing to do
      // just let cpu go
      digitalWrite(WAIT_N, HIGH);
    break;
  }
  
  // let cpu go
  //digitalWrite(WAIT_N, HIGH); 
  //moved to writeDataBus();

  processing = 0;

}

void cpuWriteCmdReq() {
  /*
  if(processing) {
    Serial.println("WC on P1");
  }

  if(!digitalRead(WRC_N)) {
    Serial.println("WC");
  } else {
    Serial.println("-WC");
  }
  //Serial.println("WC ");
  */
  //Serial.println("WC");

  processing = 1;

  int dataread = readFromDataBus();
  //Serial.println(dataread, HEX);

  switch(state) {
    case IDLE_S:
      switch(dataread) {
        case 0xF: // 15
          // reset
          //Serial.println("WC cmd reset");
          state = IDLE_S;
          filename[0] = '\0';
          filename_count = 0;
          // reset file list
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;
        break;

        case 0xE: // 14
          // list files
          //Serial.println("WC cmd list files");
          
          // file list process
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;

          root.open("/");
          if(root) {
            printDirectory(root, 0);
            state = DIR_S;
          } else {
            state = DIR_E1_S;
          }

        break;

        case 0xD: // 13
          // start load file request
          //Serial.println("WC cmd start load file request");
          filename[0] = '\0';
          filename_count = 0;
          state = RFNAME_S;
        break;

        case 0xC: // 12
          // start write file request (open file)
          //Serial.println("WC cmd start save file request");
          filename[0] = '\0';
          filename_count = 0;
          state = WFNAME_S;
        break;

        case 0xA: // 10
          // delete file request
          //Serial.println("WC cmd delete file request");
          filename[0] = '\0';
          filename_count = 0;
          state = DFNAME_S;
        break;
      }
    break;

    case WFILE_S:
      switch(dataread) {
        case 0xB: // 11
          // end write file request (close file)
          //Serial.println("WC cmd end save file request");          
          myfile.close();          
          filename[0] = '\0';
          filename_count = 0;
          state = IDLE_S;
        break;

        case 0xF: // 15
          // reset
          //Serial.println("WC cmd reset (wfile)");
          myfile.close();
          filename[0] = '\0';
          filename_count = 0;
          state = IDLE_S;          
        break;

      }
    break;

    default:
      switch(dataread) {
        case 0xF: // 15
          // reset
          //Serial.println("WC cmd reset (gbl)");
          state = IDLE_S;
          filename[0] = '\0';
          filename_count = 0;
          if(myfile) {
            myfile.close();
          }
          // reset file list
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;
        break;
      }
    break;
  }

  // let cpu go
  digitalWrite(WAIT_N, HIGH); 

  processing = 0;
}

void cpuWriteDataReq() {
  /*
  if(processing) {
    Serial.println("WD on P1");
  }

  if(!digitalRead(WRD_N)) {
    Serial.println("WD");
  } else {
    Serial.println("-WD");
  }
  //Serial.println("WD ");
  */
  //Serial.println("WD");

  processing = 1;

  int dataread = readFromDataBus();
  //Serial.println(dataread, HEX);

  char buf[8] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

  switch(state) {
    case RFNAME_S:
      //Serial.println("WD RFNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // open file, inform if error, and change state
        myfile.open(filename, O_RDONLY);
        if (myfile) {
          //Serial.println("RFILE_S start");
          state = RFILE_S;
        } else {
          //Serial.println("RFILE_E1_S");
          state = RFILE_E1_S;
        }
      }
      //Serial.println(filename);
    break;

    case WFNAME_S:
      //Serial.println("WD WFNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // open file, inform if error, and change state
        //Serial.println(filename);
        myfile.open(filename, O_WRITE | O_CREAT);
        if (myfile) {
          //Serial.println("WFILE_S start");
          state = WFILE_S;
        } else {
          //Serial.println("WFILE_E1_S");
          state = WFILE_E1_S;
        }
      }
      //Serial.println(filename);
    break;

    case DFNAME_S:
      //Serial.println("WD DFNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // check if file exists
        if (sd.exists(filename)) {
          sd.remove(filename);
          state = IDLE_S;
        } else {
          state = DFILE_E1_S;
        }
      }
      //Serial.println(filename);
    break;

    case WFILE_S:
      //Serial.println("WD WFILE_S");
      Serial.println(dataread);
      buf[0] = dataread;
      myfile.write(buf, 1);
    break;

    default:
      // do nothing
    break;
  }

  // let cpu go
  digitalWrite(WAIT_N, HIGH);
  processing = 0;

}

void cpuEndRequest() {
  // run on all relevant 138 outputs go high
  // the cpu go way running
  /*
  if(processing) {
    Serial.println("E on P1");
  }
  */
  //Serial.println("E");

  processing = 1;

  //Serial.println("E  ");
  // prepare next WAIT_L response
  // to put cpu on old then
  digitalWrite(WAIT_N, LOW);
  processing = 0;
}


//
//
//

void setupPin() {

  // data bus
  dPinModeInput();

  // output from 138 (mode)
  pinMode(RDD_N, INPUT);
  pinMode(WRD_N, INPUT);
  pinMode(RDS_N, INPUT);
  pinMode(WRC_N, INPUT);

  attachInterrupt(digitalPinToInterrupt(RDS_N), cpuReadStatusReq, FALLING);
  attachInterrupt(digitalPinToInterrupt(RDD_N), cpuReadDataReq, FALLING);
  attachInterrupt(digitalPinToInterrupt(WRC_N), cpuWriteCmdReq, FALLING);
  attachInterrupt(digitalPinToInterrupt(WRD_N), cpuWriteDataReq, FALLING);

  // decoded cpu request
  pinMode(CPUREQ_N, INPUT);
  attachInterrupt(digitalPinToInterrupt(CPUREQ_N), cpuEndRequest, RISING);

  // output enable
  pinMode(OE_N, OUTPUT);
  digitalWrite(OE_N, OE_DISABLE);

  // output direction
  pinMode(DIR, OUTPUT);
  digitalWrite(DIR, DIR_IN);

  // WAIT_N line control
  pinMode(WAIT_N, OUTPUT);
  digitalWrite(WAIT_N, LOW); // put cpu on old

  // internal led pin
  pinMode(PC13, OUTPUT);

}

// change data pins to input mode
void dPinModeInput() {

  // data bus
  pinMode(PD0, INPUT);
  pinMode(PD1, INPUT);
  pinMode(PD2, INPUT);
  pinMode(PD3, INPUT);
  pinMode(PD4, INPUT);
  pinMode(PD5, INPUT);
  pinMode(PD6, INPUT);
  pinMode(PD7, INPUT);

}

// change data pins to output mode
void dPinModeOutput() {

  // data bus
  pinMode(PD0, OUTPUT);
  pinMode(PD1, OUTPUT);
  pinMode(PD2, OUTPUT);
  pinMode(PD3, OUTPUT);
  pinMode(PD4, OUTPUT);
  pinMode(PD5, OUTPUT);
  pinMode(PD6, OUTPUT);
  pinMode(PD7, OUTPUT);

}

// put byte bits to pins
void byte2pin(int val) {
  for(int f = 0; f < 8; f++) {
    digitalWrite(dataPins[f], bitRead(val, f));
  }
}

// get byte bits from pins
int pin2byte() {
  int val = 0;
  for(int f = 0; f < 8; f++) {
    if(digitalRead(dataPins[f])) {
      bitSet(val, f);
    //} else {
    //  bitClear(val, f);
    }
  }
  return val;
}


//
//
//

void setupSDcard() {
  //delay(1000);

  // Initialize the SD card.
  if (!sd.begin(SD_CONFIG)) {
    //sd.initErrorHalt(&Serial);
    while(1){
      Serial.println("SD initialization failed!");
    }
  } else {
      Serial.println("SD initialization done!");
  }

}

/*
void setupSDcard() {
  Serial.print("Initializing SD card...");
  if (!SD.begin(PA4)) {
    Serial.println("initialization failed!");
    while (1)
    {
      Serial.println("initialization failed!");
    };
  }
  Serial.println("initialization done.");
}
*/

/*
void SDCardListFiles() {
  root = SD.open("/");
  printDirectory(root, 0);
  //Serial.println("done!");
}
*/

void printDirectory(File dir, int numTabs) {

  char fname[64] = {0};
  char *name = fname;

  dir.rewind();
  
  while (myfile.openNext(&dir, O_READ)) {
    
    for (uint8_t i = 0; i < numTabs; i++) {
      dir_lst[dir_max++] = '\t';
      //dir_max++;
      //Serial.print('\t');
    }

    myfile.getName(name, 63);
    while(*name != '\0') {    
      dir_lst[dir_max++] = *name;
      name++;
    }

    if (myfile.isDirectory()) {
      dir_lst[dir_max++] = '/';
      //Serial.println("/");
      //printDirectory(myfile, numTabs + 1);
    } else {
      // files have sizes, directories do not

      //dir_lst[dir_max++] = '\t';
      //dir_lst[dir_max++] = '\t';

      //Serial.print("\t\t");
      //Serial.println(entry.size(), DEC);
      //Serial.println("");
    }

    dir_lst[dir_max++] ='\n';
    dir_lst[dir_max++] ='\r';
    //dir_lst[dir_max] = '\0';

    myfile.close();
  }
  dir_lst[dir_max++] = '\0';
}