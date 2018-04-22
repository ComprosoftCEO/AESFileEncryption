#include <afe.h>
#include <aes.h>
#include <sha256.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


int encrypt_file(FILE* input, FILE* output, const char* key) {

    SHA256_CTX sha;
    struct AES_ctx aes;
    uint8_t hash[AES_KEYLEN];
    uint8_t filedata[AES_KEYLEN];

    if (!key) {return BAD_KEY;}

    sha256_init(&sha);
    sha256_update(&sha,(const SHA_BYTE*) key,strlen(key));
    sha256_final(&sha,hash);

    AES_init_ctx_iv(&aes,hash,hash);

    size_t read = AES_KEYLEN, write = 0;
    while(read >= AES_KEYLEN) {
        read = fread(filedata,sizeof(uint8_t),AES_KEYLEN,input);
        if (read < AES_KEYLEN) {
            if (ferror(input)) {return READ_ERROR;}

            //We're at the end of the file, so fill in
            //  the first byte with 0x80 and remaining with 0's
            filedata[read++] = 0x80;
            if (read < AES_KEYLEN) {
                memset(((uint8_t*) filedata) + read,0,(AES_KEYLEN - read));
            }
        }


        AES_CBC_encrypt_buffer(&aes,filedata,AES_KEYLEN);

        //Write to the file
        //  If only a few bits are written, then write the remaining data
        while (write < AES_KEYLEN) {
            write += fwrite(((uint8_t*) filedata)+write,sizeof(uint8_t),AES_KEYLEN-write,output);
            if (write < AES_KEYLEN) {
                if (ferror(output)) {return WRITE_ERROR;}
            }
        }
        write = 0;
    }

    return 0;
}
