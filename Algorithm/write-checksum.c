#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//Write the checksum placed at the beginning of the file (for password checking)
int write_checksum(FILE_t* fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);


    memcpy(fp->buffer,hash,fp->len);
    AES_CBC_encrypt_buffer(&aes,fp->buffer,fp->len);
    if (write_next_block(fp,NULL,fp->len) != NO_ERROR) {
        return WRITE_ERROR;
    }

    return NO_ERROR;
}
