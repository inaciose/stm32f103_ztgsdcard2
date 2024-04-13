//
// v1.01  - firmware status code changed
//        - mininimum driver/cli 1.03
// v1.02  - add rename, copy & file exist operations
//        - add mkdir, rmdir & chdir operations
// v1.03  - correction in end statuses of rename & copy
// v1.04  - solve problem in list dir (bug pointer not reset)
// v1.04a - minor changes in list dir (use globals vars for filenames)
// v1.04b - chdir fix bugs, operations abslolute and relative
// v1.04c - add cwd, get current working directory full path name
// v1.04d - change file exist operation (working)
// v1.04e - change list (add directory as argument)


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
FsFile myfile1;
FsFile root;
//FsFile wdir;
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

#include "mstatus.h"

volatile int state;

// generic buffer
char buffer[256] = {0};

// current directory full path
char directory[256] = {0};
int directory_idx = 0;
int directory_max = 0;

//char *dirnamestart_ptr = directory + 1;

char filename[256] = {0};
volatile int filename_count = 0;

char filename1[64] = {0};
volatile int filename1_count = 0;

// file list process
volatile char dir_lst[2048];
volatile int dir_idx = 0;
volatile int dir_max = 0;
char dir_fname[64] = {0};
char *dir_fname_ptr = dir_fname;


// file existe response
int fexist_reply = 0;

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

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  setupSDcard();

  
  /*
  Serial.println("SD list !");
  
  root.open("/");
  if(root) {
    printDirectory(root, 0);
  }
  root.close();

  Serial.println((char *) dir_lst);
  Serial.println("SD list done.");

  // reset file list
  dir_lst[0] = '\0';
  dir_idx = 0;
  dir_max = 0;
  */

  // full directory variable init
  directory[0] = '/';
  directory[1] = '\0'; 
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

    case GETCWD_S:
      //Serial.println("CWDD");
      directory_max = strlen(directory) + 1;
      if(directory_idx < directory_max) { 
        writeDataBus(directory[directory_idx++]); 
        if(directory_idx >= directory_max) {
          // end of the full path directory name
          // reset directory send control
          directory_idx = 0;
          directory_max = 0;
          state = IDLE_S;         
        }
      } else {
        // we never come here
        // reset directory send control
        directory_idx = 0;
        directory_max = 0;
        state = IDLE_S;
      }
    break;

    case EXFREPLY_S:
      writeDataBus(fexist_reply);
      state = IDLE_S;
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
          fexist_reply = 0;
          filename[0] = '\0';
          filename_count = 0;
          filename1[0] = '\0';
          filename1_count = 0;
          // current directory send control
          directory_idx = 0;
          directory_max = 0;
          // reset file list
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;
          state = IDLE_S;
        break;

        case 0xE: // 14
          // list files
          //Serial.println("WC cmd list files");
          filename[0] = '\0';
          filename_count = 0;
          state = DIRNAME_S;          

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

        case 0x10: // 16
          // start file rename request
          //Serial.println("WC cmd start rename file request");
          filename[0] = '\0';
          filename_count = 0;
          filename1[0] = '\0';
          filename1_count = 0;
          state = RNFNAME1_S;
        break;

        case 0x11: // 17
          // start file copy request
          //Serial.println("WC cmd start copy file request");
          filename[0] = '\0';
          filename_count = 0;
          filename1[0] = '\0';
          filename1_count = 0;
          state = CPFNAME1_S;
        break;

        case 0x12: // 18
          // start file exist request
          //Serial.println("WC cmd start exist file request");
          filename[0] = '\0';
          filename_count = 0;
          state = EXFNAME_S;
        break;

        case 0x13: // 19
          // start mkdir request
          //Serial.println("WC cmd start mkdir request");
          filename[0] = '\0';
          filename_count = 0;
          state = MKDNAME_S;
        break;

        case 0x14: // 20
          // start rmdir request
          //Serial.println("WC cmd start rmdir request");
          filename[0] = '\0';
          filename_count = 0;
          state = RMDNAME_S;
        break;

        case 0x15: // 21
          // chdir request
          //Serial.println("WC cmd start chdir request");
          filename[0] = '\0';
          filename_count = 0;
          state = CHDNAME_S;
        break;

        case 0x16: // 22
          // return current directory full path
          //Serial.println("WC cmd get cwd");
          state = GETCWD_S;
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
          filename1[0] = '\0';
          filename1_count = 0;
          state = IDLE_S;          
        break;

      }
    break;

    default:
      switch(dataread) {
        case 0xF: // 15
          // reset
          //Serial.println("WC cmd reset (gbl)");
          fexist_reply = 0;
          filename[0] = '\0';
          filename_count = 0;
          filename1[0] = '\0';
          filename1_count = 0;
          // close files
          if(myfile) {
            myfile.close();
          }
          if(myfile1) {
            myfile1.close();
          }

          // current directory send control
          directory_idx = 0;
          directory_max = 0;

          // reset file list
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;

          state = IDLE_S;
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

    case DIRNAME_S:
      //Serial.println("WD DIRNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
          // get file list directory
          // to a buffer and get ready
          // to send it byte by byte
          dir_lst[0] = '\0';
          dir_idx = 0;
          dir_max = 0;
          //Serial.println(filename);
          if(filename[0] == '\0') {
            strcpy(filename, directory);
          }
          root.open(filename);
          if(root) {
            printDirectory(root, 0);
            //Serial.println("list files");
            root.close();
            state = DIR_S;
          } else {
            state = DIR_E1_S;
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

    case RNFNAME1_S:
      //Serial.println("WD RN_FNAME1_S");
      if(dataread) {
        // get source file name
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // already get source file name
        // now get destination file name
        // we should check now if file exist
        // and return error? 
        // lets go get destination
        state = RNFNAME2_S;
      }
      //Serial.println(filename);
    break;

    case RNFNAME2_S:
      //Serial.println("WD RN_FNAME2_S");
      if(dataread) {
        // get destination file name
        filename1[filename1_count] = dataread;
        filename1[filename1_count + 1 ] = '\0';
        filename1_count++;
      } else {
        // already get source and destionation file name

        // open file, inform if error, and change state
        //Serial.println(filename);
        myfile.open(filename, O_WRITE);
        if (myfile) {
          if (myfile.rename(filename1)) {
            // renamed
            state = IDLE_S;
          } else {
            // error state
            state = RNFILE_E2_S;
          }
          myfile.close();

        } else {
          //Serial.println("RNFILE_E1_S");
          // error state
          state = RNFILE_E1_S;
        }
        
        filename[0] = '\0';
        filename_count = 0;
        filename1[0] = '\0';
        filename1_count = 0;

      }
      //Serial.println(filename);
      //Serial.println(filename1);
    break;

    case CPFNAME1_S:
      //Serial.println("WD CP_FNAME1_S");
      if(dataread) {
        // get source file name
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // already get source file name
        // now get destination file name
        // we should check now if file exist
        // and return error? 
        // lets go get destination
        state = CPFNAME2_S;
      }
      //Serial.println(filename);
    break;

    case CPFNAME2_S:
      //Serial.println("WD RN_FNAME2_S");
      if(dataread) {
        // get destination file name
        filename1[filename1_count] = dataread;
        filename1[filename1_count + 1 ] = '\0';
        filename1_count++;
      } else {
        // already get source and destionation file name

        if (myfile.open(filename, FILE_READ)) {
          // source file opeened
          if (myfile1.open(filename1, O_WRITE | O_CREAT)) {
            // destination file opeened
            // copy from src to dst

            //int data;
            //while ((data = myfile.read()) >= 0) {
            //  myfile1.write(data);
            //}

            size_t n;  
            uint8_t buf[64];
            while ((n = myfile.read(buf, sizeof(buf))) > 0) {
              myfile1.write(buf, n);
            }

            myfile1.close();
            state = IDLE_S;

          } else {
            // error opening destination
            state = CPFILE_E2_S;
          }
          
          myfile.close();
            
        } else {
          // error opening source
          state = CPFILE_E1_S;
        }
         
        filename[0] = '\0';
        filename_count = 0;
        filename1[0] = '\0';
        filename1_count = 0;

     }
      //Serial.println(filename);
      //Serial.println(filename1);
    break;

    case EXFNAME_S:
      //Serial.println("WD EXFNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        if (sd.exists(filename)) {   
          // file exists. what type       
          myfile.open(filename, O_READ);
          if (myfile) {
            if (myfile.isDirectory()) {
              fexist_reply = 2;
            } else {
              fexist_reply = 1;
            }
            myfile.close();
            state = EXFREPLY_S;
          } else {
            // error opening the file
            // cannot get type
            // error state
            state = GETCWD_E1_S;
          }
        // file do not exists
        } else {
          fexist_reply = 0;
          state = EXFREPLY_S;
        }

        // reset filename
        filename[0] = '\0';
        filename_count = 0;
      }
      //Serial.println(filename);
    break;

    case MKDNAME_S:
      //Serial.println("WD MKDNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // we have the name, now
        // try to create the directory
        if (sd.mkdir(filename)) {
          state = IDLE_S;
        } else {
          state = MKDNAME_E1_S;
        }
        // clean up
        filename[0] = '\0';
        filename_count = 0;
      }
      //Serial.println(filename);
    break;

    case RMDNAME_S:
      //Serial.println("WD RMDNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // we have the name, now
        // try to remove the directory
        if (sd.rmdir(filename)) {
          state = IDLE_S;
        } else {
          state = RMDNAME_E1_S;
        }
        // clean up
        filename[0] = '\0';
        filename_count = 0;
      }
      //Serial.println(filename);
    break;

    case CHDNAME_S:
      //Serial.println("WD CHDNAME_S");
      if(dataread) {
        filename[filename_count] = dataread;
        filename[filename_count + 1 ] = '\0';
        filename_count++;
      } else {
        // we have the name, now
        // may be ".." process it
        if(filename[0]=='.' && filename[0]=='.') {
          strcpy(buffer, directory);
          char *ptr = strrchr(buffer, '/');
          *ptr = '\0';

          // store original filename
          strcpy(filename1, filename);

          // copy parent directory full path to filename
          strcpy(filename, buffer);

          // add the remaning chars after the ".."
          char *ptr1 = filename1 + 2;
          strcat(filename, ptr1);

          // if filename is an empty string add '/'
          if(!strlen(filename)) {
            filename[0] = '/';
            filename[1] = '\0';
          }

          //Serial.println(filename);

        }
        // try to change to the directory on filename
        if (sd.chdir(filename)) {
          // update directory variable
          if(filename[0]=='/') {
            // is an absolute path
            strcpy(directory, filename);

          } else {
            // is a relative path
            // test if whe are on root
            if(strcmp(directory, "/")) {
              // return != 0, are diferent, not root
              // add separator
              strcat(directory, "/");
            }
            // add the directory name
            strcat(directory, filename);
          }
          state = IDLE_S;
        } else {
          state = CHDNAME_E1_S;
        }
        // clean up
        filename[0] = '\0';
        filename_count = 0;
      }
      //Serial.println(filename);
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


void printDirectory(File dir, int numTabs) {

  dir.rewind();
  
  while (myfile.openNext(&dir, O_READ)) {
    
    for (uint8_t i = 0; i < numTabs; i++) {
      dir_lst[dir_max++] = '\t';
      //dir_max++;
    }

    dir_fname_ptr = dir_fname;
    myfile.getName(dir_fname_ptr, 63);

    while(*dir_fname_ptr != '\0') {    
      dir_lst[dir_max++] = *dir_fname_ptr;
      dir_fname_ptr++;
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

    myfile.close();
  }

  dir_lst[dir_max++] = '\0';
}