#ifndef AES_FILE_ENCRYPTION_HEADER
#define AES_FILE_ENCRYPTION_HEADER

#include <stdio.h>
#include <file-io.h>

#define END_BYTE 0x80

int encrypt_file(FILE_t* fp, const char* key);
int decrypt_file(FILE_t* fp, const char* key);

char* get_password(const char*);
void hide_output();
void restore_output();

#endif // AES_FILE_ENCRYPTION_HEADER Included
