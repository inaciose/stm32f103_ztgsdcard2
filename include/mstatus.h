// enum state_t {IDLE_S, DIR_S, RFNAME_S, WFNAME_S, DFNAME_S, RFILE_S, WFILE_S, RFILE_E1_S, WFILE_E1_S, DIR_E1_S, DFILE_E1_S};
#define IDLE_S 0

// read file
//myfile.open(filename, O_RDONLY);
// read file name status
#define RFNAME_S 2
// read file contents status
#define RFILE_S 4
// error status
#define RFILE_E1_S 5

// write file (save)
// myfile.open(filename, O_WRITE | O_CREAT);
// read file name status
#define WFNAME_S 6
// write to file status
#define WFILE_S 8
//error status
#define WFILE_E1_S 9

// list / dir status
/// send list/dir bytes (ascii), one per cpu request
#define DIRNAME_S 10
// send list/dir bytes (ascii), one per cpu request
#define DIR_S 12
// error status
#define DIR_E1_S 11
#define DIR_E2_S 13

// delete a file
// read file name status
#define DFNAME_S 14
// error status
#define DFILE_E1_S 15

// rename a file
// read origin file name status
#define RNFNAME1_S 16
// read destination file name status
#define RNFNAME2_S 18
// error status
#define RNFILE_E1_S 17
#define RNFILE_E2_S 19

// copy a file
// read origin file name status
#define CPFNAME1_S 20
// read destination file name status
#define CPFNAME2_S 22
// error status
#define CPFILE_E1_S 21
#define CPFILE_E2_S 23

// check if a file exists
// read file name status
#define EXFNAME_S 24
// send file status (0 = no exist, 1 = file, 2 = dir)
#define EXFREPLY_S 26
// error status
#define EXFNAME_E1_S 25

// make a new directory
// read directory name status
#define MKDNAME_S 28
// error status
#define MKDNAME_E1_S 29

// remove a directory (rmdir)
// read directory name status
#define RMDNAME_S 30
// error status
#define RMDNAME_E1_S 31

// change current directory (cd)
// read directory name status
#define CHDNAME_S 32
// error status
#define CHDNAME_E1_S 33

// get current directory (cd)
// send full path directory name bytes (ascii), one per cpu request
#define GETCWD_S 34
// error status
#define GETCWD_E1_S 35


//
//
//

// open filename
// read file name status
#define OFNAME_S 36
// read file open mode status
#define OFMODE_S 38
// write file handle status
#define OFGHDH_S 40
// open file status
//#define OFOPEN_S 178
// error status
#define OFOPEN_E1_S 41

// close filename
// read file handle
#define CFHDL_S 42
// error status
#define CFHDL_E1_S 43

// write a byte
// read file handle
#define FWRITE_HDL_S 44
// write byte to file status
#define FWRITE_S 46
// operation result status
#define FWRITERES_S 48
// error status
#define FWRITE_HDL_E1_S 45
#define FWRITE_E1_S 47
#define FWRITERES_E1_S 49

// read a byte
// read file handle
#define FREAD_HDL_S 50
// read byte from file status
#define FREAD_S 52
// operation result status
#define FREADRES_S 54
// error status
#define FREAD_HDL_E1_S 51
#define FREAD_E1_S 53

// peak a byte
// read file handle
#define FPEEK_HDL_S 80
// read byte from file status
#define FPEEK_S 82
// operation result status
//#define FREWINDRES_S 236
// error status
#define FPEEK_HDL_E1_S 81
#define FPEEK_E1_S 83

// get file pos
// read file handle
#define FGETPOS_HDL_S 56
// read byte from file status
#define FGETPOS_S 58
// operation result status
//#define FFGETPOSRES_S 204
// error status
#define FGETPOS_HDL_E1_S 57
#define FGETPOS_E1_S 59

// set file pos (absolute start)
// read file handle
#define FSEEKSET_HDL_S 60
// read byte from file status
#define FSEEKSET_S 62
// operation result status
#define FSEEKSETRES_S 64
// error status
#define FSEEKSET_HDL_E1_S 61
#define FSEEKSET_E1_S 63

// set file pos (relative)
// read file handle
#define FSEEKCUR_HDL_S 66
// read byte from file status
#define FSEEKCUR_S 68
// operation result status
#define FSEEKCURRES_S 70
// error status
#define FSEEKCUR_HDL_E1_S 67
#define FSEEKCUR_E1_S 69

// set file pos (absolute end)
// read file handle
#define FSEEKEND_HDL_S 72
// read byte from file status
#define FSEEKEND_S 74
// operation result status
#define FSEEKENDRES_S 76
// error status
#define FSEEKEND_HDL_E1_S 73
#define FSEEKEND_E1_S 75

// set file pos (rewind)
// read file handle
#define FREWIND_HDL_S 78
// error status
#define FREWIND_HDL_E1_S 79
//#define FREWIND_E1_S 235

// write n bytes
// read file handle
#define FWRITEB_HDL_S 84
// write byte to file status
#define FWRITEB_S 86
// operation result status
#define FWRITEBRES_S 88
// error status
#define FWRITEB_HDL_E1_S 85
#define FWRITEB_E1_S 87
#define FWRITEBRES_E1_S 89

// read n bytes
// read file handle
#define FREADB_HDL_S 90
// read byte from file status
#define FREADB_S 92
// operation result status
#define FREADBRES_S 94
// error status
#define FREADB_HDL_E1_S 91
#define FREADB_E1_S 93
#define FREADBRES_E1_S 95

// truncate file to n bytes lenght
// read file handle
#define FTRUNCATE_HDL_S 96
// read bytes to set lenght
#define FTRUNCATE_S 98
// operation result status
#define FTRUNCATERES_S 100
// error status
#define FTRUNCATE_HDL_E1_S 97
#define FTRUNCATE_E1_S 99

// list open files
// write file handle status
#define LSOPEN_READ_S 102
// write file handle status
//#define LSOPEN_END_S 104
// error status
#define LSOPEN_READ_E1_S 103