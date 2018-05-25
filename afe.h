#ifndef AES_FILE_ENCRYPTION_HEADER
#define AES_FILE_ENCRYPTION_HEADER

#include <stdio.h>
#include <file-io.h>

#define END_BYTE 0x80

int encrypt_file(FILE_t* in_fp, FILE_t* out_fp, const char* key);
int decrypt_file(FILE_t* in_fp, FILE_t* out_fp, const char* key);

//File checksum handling
int write_checksum(FILE_t* fp, const char* key);
int verify_file(FILE_t* fp, const char* key);

#define VERIFY_OK   4
#define VERIFY_BAD  5

char* get_password(const char*);
void hide_output();
void restore_output();

#endif // AES_FILE_ENCRYPTION_HEADER Included
