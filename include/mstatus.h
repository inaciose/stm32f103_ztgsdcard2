// enum state_t {IDLE_S, DIR_S, RFNAME_S, WFNAME_S, DFNAME_S, RFILE_S, WFILE_S, RFILE_E1_S, WFILE_E1_S, DIR_E1_S, DFILE_E1_S};
#define IDLE_S 0

// read file
//myfile.open(filename, O_RDONLY);
// read file name status
#define RFNAME_S 8
// read file contents status
#define RFILE_S 10
// error status
#define RFILE_E1_S 11
//#define RFNAME_S 3
//#define RFILE_S 5
//#define RFILE_E1_S 9

// write file (save)
// myfile.open(filename, O_WRITE | O_CREAT);
// read file name status
#define WFNAME_S 16
// write to file status
#define WFILE_S 18
//error status
#define WFILE_E1_S 19
//#define WFNAME_S 2
//#define WFILE_S 4
//#define WFILE_E1_S 8


// list / dir status
/// send list/dir bytes (ascii), one per cpu request
#define DIRNAME_S 32
// send list/dir bytes (ascii), one per cpu request
#define DIR_S 34
// error status
#define DIR_E1_S 33
#define DIR_E2_S 35
//#define DIR_S 16
//#define DIR_E1_S 17

// delete a file
// read file name status
#define DFNAME_S 40
// error status
#define DFILE_E1_S 41
//#define DFNAME_S 32
//#define DFILE_E1_S 33

// rename a file
// read origin file name status
#define RNFNAME1_S 48
// read destination file name status
#define RNFNAME2_S 56
// error status
#define RNFILE_E1_S 49
#define RNFILE_E2_S 57

// copy a file
// read origin file name status
#define CPFNAME1_S 64
// read destination file name status
#define CPFNAME2_S 72
// error status
#define CPFILE_E1_S 65
#define CPFILE_E2_S 73

// check if a file exists
// read file name status
#define EXFNAME_S 128
// send file status (0 = no exist, 1 = file, 2 = dir)
#define EXFREPLY_S 130
// error status
#define EXFNAME_E1_S 129

// make a new directory
// read directory name status
#define MKDNAME_S 80
// error status
#define MKDNAME_E1_S 81

// remove a directory (rmdir)
// read directory name status
#define RMDNAME_S 88
// error status
#define RMDNAME_E1_S 89

// change current directory (cd)
// read directory name status
#define CHDNAME_S 120
// error status
#define CHDNAME_E1_S 121

// get current directory (cd)
// send full path directory name bytes (ascii), one per cpu request
#define GETCWD_S 152
// error status
#define GETCWD_E1_S 153


//
//
//

// open filename
// read file name status
#define OFNAME_S 160
// read file open mode status
#define OFMODE_S 168
// write file handle status
#define OFGHDH_S 176
// open open status
#define OFOPEN_S 178

// error status
#define OFOPEN_E1_S 179
#define OFMODE_E1_S 169

// close filename
// read file handle
#define CFHDL_S 184
// error status
#define CFHDL_E1_S 185