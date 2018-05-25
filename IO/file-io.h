//Handles all of the file input and output
#ifndef FILE_IO_HEADER
#define FILE_IO_HEADER

#include <stdint.h>

//File Input and Output
typedef struct {
    uint8_t* buffer;    // Where to write the file data
    size_t len;         // Just there for reference
} FILE_t;

extern const size_t File_Block_Len;

//When input file is the same as output file, then bytes are overwritten
//  starting from the start of the main file
//
//  The backup file = input_file + backup_extension
int open_file(const char* input_file, const char* output_file, const char* backup_ext, FILE_t** fp);
void close_file(FILE_t* fp);


//Reads the next block from the input file
//
//  If buf is null, then use fp->buf
//
//  The return is one of the following
//      READ_ERROR      -> Unknown problem
//      NO_ERROR        -> Process this block as normal
//      END_OF_FILE     -> This is the last block in the file, bytes_read is less than total
int read_next_block(FILE_t* fp, uint8_t* buf, size_t to_read, size_t* bytes_read);

//Write a block to the output file
//
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

#endif // FILE_IO_HEADER Included
