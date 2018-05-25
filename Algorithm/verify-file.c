#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Prints out whether or not this is the correct password for the file
int verify_file(FILE_t* fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    size_t read;
    int result = read_next_block(fp,NULL,fp->len,&read);
    if (result != READ_ERROR) {

        AES_CBC_decrypt_buffer(&aes,fp->buffer,AES_KEYLEN);

        //Hash of the password is stored in the first byte
        if (!memcmp(fp->buffer,hash,AES_KEYLEN)) {
            return VERIFY_OK;
        } else {
            return VERIFY_BAD;
        }
    } else {
        return READ_ERROR;
    }
}
