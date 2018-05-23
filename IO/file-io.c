// Handles all file input and output
#include <afe.h>
#include <aes.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Public buffer that stores all file data
const size_t File_Block_Len = AES_KEYLEN;

typedef struct {
    uint8_t* buffer;        //Buffer that stores the file data
    size_t len;             //Total length of the buffer (same as File_Block_Len)
    FILE* in_file;
    FILE* out_file;
    char* backup_filename;
} FILE_Obj_t;

static int backup_file(FILE_Obj_t* fp);

//Automatically allocates the file object in memory
int open_file(const char* input_file, const char* output_file, const char* bext, FILE_t** fp) {
    if (!(fp && input_file && output_file && bext)) {return BAD_POINTER;}

    //Allocate data
    FILE_Obj_t* f = (FILE_Obj_t*) calloc(1,sizeof(FILE_Obj_t));
    if (!f) {return ALLOC_ERROR;}
    f->buffer = (uint8_t*) malloc(sizeof(uint8_t) * File_Block_Len);
    f->len = File_Block_Len;
    if (!f->buffer) {close_file((FILE_t*) f); return ALLOC_ERROR;}

    //Prepare the backup filename string
    f->backup_filename = (char*) calloc(strlen(input_file) + strlen(bext)+1,sizeof(char));
    strcpy(f->backup_filename,input_file);
    strcat(f->backup_filename,bext);

    f->in_file = fopen(input_file,"rb");
    if (f->in_file == NULL)         {close_file((FILE_t*) f); return IN_OPEN_ERROR;}
    if (backup_file(f) != NO_ERROR) {close_file((FILE_t*) f); return BACKUP_ERROR;}

    //Configure input and output files
    f->in_file = fopen(f->backup_filename,"rb");
    if (f->in_file == NULL) {close_file((FILE_t*) f); return BACKUP_ERROR;}

    f->out_file = fopen(output_file, "wb");
    if (f->out_file == NULL) {close_file((FILE_t*) f); return OUT_OPEN_ERROR;}

    *fp = (FILE_t*) f;
    return NO_ERROR;
}


//Automatically deallocates the file object from memory
void close_file(FILE_t* fp) {
    if (!fp) {return;}

    FILE_Obj_t* f = (FILE_Obj_t*) fp;
    if (f->buffer) {free(f->buffer);}
    if (f->backup_filename) {free(f->backup_filename);}
    if (f->in_file)  {fclose(f->in_file);}
    if (f->out_file) {fclose(f->out_file);}
    free(fp);
}

//Copy the contents of one file to the internal backup file string
//  The only call does not ever pass a NULL pointer
static int backup_file(FILE_Obj_t* fp) {
    FILE* bfp = fopen(fp->backup_filename,"wb");
    if (bfp == NULL) {return OPEN_ERROR;}

    // Read contents from file
    int c = fgetc(fp->in_file);
    while (c != EOF) {
        fputc(c, bfp);
        c = fgetc(fp->in_file);
    }

    fclose(bfp);
    return NO_ERROR;
}


//Read a single AES block into the buffer
//
//  The return is one of the following
//      READ_ERROR      -> Unknown problem
//      NO_ERROR        -> Process this block as normal
//      END_OF_FILE     -> This is the last block in the file, bytes_read is less than total
int read_next_block(FILE_t* fp, size_t* bytes_read) {
    if (!(fp && bytes_read)) {return BAD_POINTER;}

    FILE_Obj_t* f = (FILE_Obj_t*) fp;
    *bytes_read = fread(f->buffer,sizeof(uint8_t),File_Block_Len,f->in_file);

    if (*bytes_read < File_Block_Len) {
        if (ferror(f->in_file)) {return READ_ERROR;}

        //We're at the end of the file, so fill in
        //  the remaining space with 0's
        memset(((uint8_t*) f->buffer) + *bytes_read,0,(File_Block_Len - *bytes_read));
        return END_OF_FILE;
    }

    return NO_ERROR;
}


//Write a block to the output file
//
//  The return is either
//      WRITE_ERROR     -> I don't know
//      NO_ERROR        -> Bytes written successfully
int write_next_block(FILE_t* fp, size_t to_write) {
    if (!fp) {return BAD_POINTER;}
    if (to_write > File_Block_Len) {to_write = File_Block_Len;}

    FILE_Obj_t *f = (FILE_Obj_t*) fp;

    //Write to the file
    //  If only a few bits are written, then write the remaining data
    size_t write = 0;
    uint8_t* buf = f->buffer;
    while (write < to_write) {
        write += fwrite(buf,sizeof(uint8_t),to_write-write,f->out_file);
        if (write < to_write) {
            if (ferror(f->out_file)) {return WRITE_ERROR;}
        }
        buf+=write;
    }

    return NO_ERROR;
}
