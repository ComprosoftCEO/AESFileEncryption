#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int decrypt_file(FILE_t* fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];
    uint8_t cur_filedata[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    size_t read;
    int result = read_next_block(fp,NULL,fp->len,&read);
    while(result == NO_ERROR) {

        memcpy(cur_filedata,fp->buffer,fp->len);
        AES_CBC_decrypt_buffer(&aes,cur_filedata,AES_KEYLEN);

        result = read_next_block(fp,NULL,fp->len,&read);
        if (result == READ_ERROR) {return READ_ERROR;}
        if (result == NO_ERROR) {
              write_next_block(fp,cur_filedata,fp->len);
        }
    }

    //Where is the special terminating byte
    //  May not exist if the block is the exact right length
    int end;
    uint8_t* fin = cur_filedata;
    if (read > 0) {fin = fp->buffer;}
    for (end = fp->len - 1; end >= 0; --end) {
        if (fin[end] == 0x80) {break;}
    } if (end < 0) {end = 32;}
    write_next_block(fp,fin,end);

    return NO_ERROR;
}
