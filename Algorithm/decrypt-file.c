#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int decrypt_file(FILE_t* in_fp, FILE_t* out_fp, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];
    uint8_t cur_data[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    if (verify_file(in_fp,key) != VERIFY_OK) {return VERIFY_BAD;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    size_t read;
    int result = read_next_block(in_fp,NULL,in_fp->len,&read);
    if (result == READ_ERROR) {return READ_ERROR;}
    while(result == NO_ERROR) {

        memcpy(cur_data,in_fp->buffer,in_fp->len);
        AES_CBC_decrypt_buffer(&aes,cur_data,AES_KEYLEN);

        result = read_next_block(in_fp,NULL,in_fp->len,&read);
        if (result == READ_ERROR) {return READ_ERROR;}
        if (result == NO_ERROR) {
              write_next_block(out_fp,cur_data,AES_KEYLEN);
        }
    }

    //Where is the special terminating byte
    //  May not exist if the block is the exact right length
    int end;
    uint8_t* fin = cur_data;
    if (read > 0) {fin = in_fp->buffer;}
    for (end = in_fp->len - 1; end >= 0; --end) {
        if (fin[end] == 0x80) {break;}
    } if (end < 0) {end = 32;}
    write_next_block(out_fp,fin,end);

    return NO_ERROR;
}
