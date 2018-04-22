#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Read a single AES block into the buffer
//
//  Returns:
//      -1 => Error
//       0 => Do normal
//       1 => Last block
//       2 => Don't do this block
static int read_aes_block(uint8_t* buffer,FILE* fp) {
    size_t read = fread(buffer,sizeof(uint8_t),AES_KEYLEN,fp);
    if (read < AES_KEYLEN) {
        if (read == 0) {return 2;}
        if (ferror(fp)) {return -1;}

        //We're at the end of the file, so fill in
        //  the remaining space with 0's
        memset(((uint8_t*) buffer) + read,0,(32 - read));
        return 1;
    }

    return 0;
}

//Returns 0 on failure
static int write_aes_block(uint8_t* buffer,FILE* fp, size_t cnt) {

    //Write to the file
    //  If only a few bits are written, then write the remaining data
    size_t write = 0;
    while (write < cnt) {
        write += fwrite(((uint8_t*) buffer)+write,1,cnt-write,fp);
        if (write < cnt) {
            if (ferror(fp)) {return 0;}
        }
    }
    return 1;
}



int decrypt_file(FILE* input, FILE* output, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];
    uint8_t cur_filedata[AES_KEYLEN];
    uint8_t next_filedata[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    int result;
    result = read_aes_block(next_filedata,input);

    while(result == 0) {

        memcpy(cur_filedata,next_filedata, sizeof(uint8_t) * AES_KEYLEN);

        AES_CBC_decrypt_buffer(&aes,cur_filedata,AES_KEYLEN);

        result = read_aes_block(next_filedata,input);
        if (result == 0) {
            write_aes_block(cur_filedata,output,AES_KEYLEN);
        }
    }

    if (result < 0) {return READ_ERROR;}
    if (result == 1) {
        //Do one final decryption
        memcpy(cur_filedata,next_filedata, sizeof(uint8_t) * AES_KEYLEN);
        AES_CBC_decrypt_buffer(&aes,cur_filedata,AES_KEYLEN);
    }

    //Where is the special terminating byte
    //  May not exist if the block is the exact right length
    int end;
    for (end = AES_KEYLEN - 1; end >= 0; --end) {
        if (cur_filedata[end] == 0x80) {break;}
    } if (end < 0) {end = 32;}
    write_aes_block(cur_filedata,output,end);


    return 0;
}
