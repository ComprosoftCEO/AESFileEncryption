#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int encrypt_file(FILE_t* fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];

    if (!fp) {return BAD_POINTER;}
    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    int result = NO_ERROR, write;
    size_t read;
    while (result == NO_ERROR) {

        result = read_next_block(fp,NULL,fp->len,&read);
        if (result == READ_ERROR) {return READ_ERROR;}
        if (result == END_OF_FILE) {
            if (read < 32) {fp->buffer[read] = END_BYTE;}
            else {result = NO_ERROR; /* Do one more loop to add on to next block */}
        }

        AES_CBC_encrypt_buffer(&aes,fp->buffer,fp->len);

        write = write_next_block(fp,NULL,fp->len);
        if (write != NO_ERROR) {return WRITE_ERROR;}
    }

    return 0;
}
