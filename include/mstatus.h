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
// open file status
#define OFOPEN_S 178

// error status
#define OFOPEN_E1_S 179
#define OFMODE_E1_S 169

// close filename
// read file handle
#define CFHDL_S 184
// error status
#define CFHDL_E1_S 185

// write a byte
// read file handle
#define FWRITE_HDL_S 192
// write byte to file status
#define FWRITE_S 194
// operation result status
#define FWRITERES_S 196
// error status
#define FWRITE_HDL_E1_S 193
#define FWRITE_E1_S 195
#define FWRITERES_E1_S 197

// read a byte
// read file handle
#define FREAD_HDL_S 200
// read byte from file status
#define FREAD_S 202
// operation result status
#define FREADRES_S 204
// error status
#define FREAD_HDL_E1_S 201
#define FREAD_E1_S 203
//#define FREADRES_E1_S 205

// peak a byte
// read file handle
#define FPEEK_HDL_S 240
// read byte from file status
#define FPEEK_S 242
// operation result status
//#define FREWINDRES_S 236
// error status
#define FPEEK_HDL_E1_S 241
#define FPEEK_E1_S 243

// get file pos
// read file handle
#define FGETPOS_HDL_S 208
// read byte from file status
#define FGETPOS_S 210
// operation result status
//#define FFGETPOSRES_S 204
// error status
#define FGETPOS_HDL_E1_S 209
#define FGETPOS_E1_S 211


// set file pos (absolute start)
// read file handle
#define FSEEKSET_HDL_S 216
// read byte from file status
#define FSEEKSET_S 218
// operation result status
#define FSEEKSETRES_S 220
// error status
#define FSEEKSET_HDL_E1_S 217
#define FSEEKSET_E1_S 219

// set file pos (relative)
// read file handle
#define FSEEKCUR_HDL_S 224
// read byte from file status
#define FSEEKCUR_S 226
// operation result status
#define FSEEKCURRES_S 228
// error status
#define FSEEKCUR_HDL_E1_S 225
#define FSEEKCUR_E1_S 227

// set file pos (absolute end)
// read file handle
#define FSEEKEND_HDL_S 232
// read byte from file status
#define FSEEKEND_S 234
// operation result status
#define FSEEKENDRES_S 236
// error status
#define FSEEKEND_HDL_E1_S 233
#define FSEEKEND_E1_S 235

// set file pos (rewind)
// read file handle
#define FREWIND_HDL_S 238
// read byte from file status
//#define FREWIND_S 234
// operation result status
//#define FREWINDRES_S 236
// error status
#define FREWIND_HDL_E1_S 233
//#define FREWIND_E1_S 235