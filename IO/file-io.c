// Handles all file input and output
#include <afe.h>
#include <aes.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    uint8_t* buffer;        // Buffer that stores the file data
    size_t len;             // Total length of the buffer (same as File_Block_Len)
    FILE* in_file;          // If NULL, then there is no out file
    FILE* out_file;         // If NULL, then there is no in file
    char* backup_filename;
} FILE_Obj_t;

static int backup_file(FILE_Obj_t* fp);
static void copy_file(FILE* in, FILE* out) ;


//Automatically allocates the file object in memory
int open_file(const char* file, FILEMODE how, size_t buf_sz, FILE_t** fp) {
    if (!(fp && file)) {return BAD_POINTER;}

    //Allocate data
    FILE_Obj_t* f = (FILE_Obj_t*) calloc(1,sizeof(FILE_Obj_t));
    if (!f) {return ALLOC_ERROR;}
    f->buffer = (uint8_t*) malloc(sizeof(uint8_t) * buf_sz);
    if (!f->buffer) {
        close_file((FILE_t*) f,C_NOTHING);
        return ALLOC_ERROR;
    }
    f->len = buf_sz;


    how &= (F_READ | F_WRITE);  //Create the mask
    if (how & F_READ) {         //Open the input file (if needed)
        f->in_file = fopen(file,"rb");
        if (f->in_file == NULL) {
            close_file((FILE_t*) f,C_NOTHING);
            return IN_OPEN_ERROR;
        }
    }

    //Prepare the backup filename string (if needed)
    if (how == F_READ_WRITE) {
        f->backup_filename = (char*) calloc(strlen(file) + strlen(BAK_EXT)+1,sizeof(char));
        if (!f->backup_filename) {
            close_file((FILE_t*) f,C_NOTHING);
            return ALLOC_ERROR;
        }

        strcpy(f->backup_filename,file);
        strcat(f->backup_filename,BAK_EXT);
        if (backup_file(f) != NO_ERROR) {
            close_file((FILE_t*) f,C_REMOVE);
            return BACKUP_ERROR;
        }
        fclose(f->in_file);

        //Input file should read from backup file
        f->in_file = fopen(f->backup_filename,"rb");
        if (f->in_file == NULL) {
            close_file((FILE_t*) f,C_REMOVE);
            return BACKUP_ERROR;
        }
    }

    if (how & F_WRITE) {
        f->out_file = fopen(file, "wb");
        if (f->out_file == NULL) {
            close_file((FILE_t*) f,C_REMOVE);
            return OUT_OPEN_ERROR;
        }
    }

    *fp = (FILE_t*) f;
    return NO_ERROR;
}


//Automatically deallocates the file object from memory
void close_file(FILE_t* fp, CLOSE_ACTION action) {
    if (!fp) {return;}

    FILE_Obj_t* f = (FILE_Obj_t*) fp;
    if (f->buffer) {free(f->buffer);}
    if (f->backup_filename) {
        switch(action) {
            case C_NOTHING: break;
            case C_UNDO:
                //Copy file before removing backup (we do NOT want a break here)
                copy_file(f->in_file,f->out_file);

            case C_REMOVE:
                //Backup is always the input file
                //  On Windows, you need to close the file before you can remove it
                if (f->in_file) {fclose(f->in_file); f->in_file = NULL;}
                remove(f->backup_filename);
        }
        free(f->backup_filename);
    }
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


//Copy from one file to another
static void copy_file(FILE* in, FILE* out) {

    rewind(in);
    rewind(out);

    // Read contents from file
    int c = fgetc(in);
    while (c != EOF) {
        fputc(c, out);
        c = fgetc(in);
    }
}

//Read a single AES block into the buffer
//
//  IF buf is null, then use fp->buf
//
//  The return is one of the following
//      READ_ERROR (<0) -> Unknown problem
//      NO_ERROR        -> Process this block as normal
//      END_OF_FILE     -> This is the last block in the file, bytes_read is less than total
int read_next_block(FILE_t* fp, uint8_t* buf, size_t to_read, size_t* bytes_read) {
    if (!(fp && bytes_read)) {return BAD_POINTER;}

    FILE_Obj_t* f = (FILE_Obj_t*) fp;
    if (!f->in_file) {return WRONG_MODE;}
    if (!buf) {
        buf = fp->buffer;
        if (to_read > fp->len) {to_read = fp->len;}
    }

    *bytes_read = fread(buf,sizeof(uint8_t),to_read,f->in_file);

    if (*bytes_read < to_read) {
        if (ferror(f->in_file)) {return READ_ERROR;}

        //We're at the end of the file, so fill in
        //  the remaining space with 0's
        memset(((uint8_t*) buf) + *bytes_read,0,(to_read - *bytes_read));
        return END_OF_FILE;
    }

    return NO_ERROR;
}


//Write a block to the output file
//
//  If buf is null, then use fp->buf
//
//  The return is either
//      WRITE_ERROR (<0)-> I don't know
//      NO_ERROR        -> Bytes written successfully
int write_next_block(FILE_t* fp, uint8_t* buf, size_t to_write) {
    if (!fp) {return BAD_POINTER;}

    FILE_Obj_t *f = (FILE_Obj_t*) fp;
    if (!f->out_file) {return WRONG_MODE;}
    if (!buf) {
        buf = fp->buffer;
        if (to_write > fp->len) {to_write = fp->len;}
    }

    //Write to the file
    //  If only a few bits are written, then write the remaining data
    size_t write = 0;
    while (write < to_write) {
        write += fwrite(buf,sizeof(uint8_t),to_write-write,f->out_file);
        if (write < to_write) {
            if (ferror(f->out_file)) {return WRITE_ERROR;}
        }
        buf+=write;
    }

    return NO_ERROR;
}
