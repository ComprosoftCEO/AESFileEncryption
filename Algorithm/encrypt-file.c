#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int encrypt_file(FILE_t* in_fp, FILE_t* out_fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];

    if (!(in_fp && out_fp)) {return BAD_POINTER;}
    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    //Put the header into the file (encryption of the hash)
    if (write_checksum(out_fp,key) != NO_ERROR) {return WRITE_ERROR;}

    int result = NO_ERROR, write;
    size_t read;
    while (result == NO_ERROR) {

        result = read_next_block(in_fp,NULL,in_fp->len,&read);
        if (result == READ_ERROR) {return READ_ERROR;}
        if (result == END_OF_FILE) {
            if (read < 32) {in_fp->buffer[read] = END_BYTE;}
            else {result = NO_ERROR; /* Do one more loop to add on to next block */}
        }

        AES_CBC_encrypt_buffer(&aes,in_fp->buffer,in_fp->len);

        write = write_next_block(out_fp,in_fp->buffer,in_fp->len);
        if (write != NO_ERROR) {return WRITE_ERROR;}
    }

    return NO_ERROR;
}
