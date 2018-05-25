//Handles all of the file input and output
#ifndef FILE_IO_HEADER
#define FILE_IO_HEADER

#include <stdint.h>


//Modes of opening the file:
typedef enum {
    F_READ       = 1,   // Open a single file for reading
    F_WRITE      = 2,   // Open a single file for writing
    F_READ_WRITE = 3    // Open a file for reading and overwriting (READ | WRITE)
} FILEMODE;

//What to do with the backup file
typedef enum {
    C_NOTHING = 0,      // Do nothing to the backup file
    C_REMOVE  = 1,      // Remove the backup file
    C_UNDO    = 2       // Replace the old file with the backup file
} CLOSE_ACTION;

//File Input and Output
typedef struct {
    uint8_t* buffer;    // Where to write the file data
    size_t len;         // How big is the buffer (DO NOT MODIFY)
    FILEMODE mode;      // Current mode of this file object (DO NOT MODIFY)
} FILE_t;

//Open a file for reading, writing, or both
//
//When a file is open for both reading and writing, the bytes are copies to a
//  temporary file (<file>.bak), then the old file is overwritten
//
int open_file(const char* file, FILEMODE how, size_t buf_sz, FILE_t** fp);
void close_file(FILE_t* fp, CLOSE_ACTION removeBackup);
#define BAK_EXT ".bak"


//===========================================
//Reads the next block from the input file
//===========================================
//
//  If buf is null, then use fp->buf
//
//  The return is one of the following
//      READ_ERROR      -> Unknown problem
//      NO_ERROR        -> Process this block as normal
//      END_OF_FILE     -> This is the last block in the file, bytes_read is less than total
int read_next_block(FILE_t* fp, uint8_t* buf, size_t to_read, size_t* bytes_read);

//====================================
//Write a block to the output file
//====================================
//  Writes bytes until all bytes or written or an error occurs
//  If buf is null, then use fp->buf
//
//  The return is either
//      WRITE_ERROR     -> I don't know
//      NO_ERROR        -> Bytes written successfully
int write_next_block(FILE_t* fp, uint8_t* buf, size_t to_write);


//Error codes
#define END_OF_FILE     1
#define NO_ERROR        0
#define BAD_KEY        -1
#define READ_ERROR     -2
#define WRITE_ERROR    -3
#define OPEN_ERROR     -4     //Error opening file (if input and output file are the same)
#define CLOSE_ERROR    -5
#define IN_OPEN_ERROR  -6     //Error opening input file
#define OUT_OPEN_ERROR -7     //Error opening output file
#define ALLOC_ERROR    -8     //Unable to allocate data in memory
#define BACKUP_ERROR   -9     //Problem backing up the file for reading and writing
#define BAD_POINTER    -10    //Don't try to give me a null pointer
#define WRONG_MODE     -11    //Trying to read from a write file or write to a read file

#endif // FILE_IO_HEADER Included
