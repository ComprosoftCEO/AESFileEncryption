#ifndef AES_FILE_ENCRYPTION_HEADER
#define AES_FILE_ENCRYPTION_HEADER

#include <stdio.h>
#include <file-io.h>

int encrypt_file(FILE* input, FILE* output, const char* key);
int decrypt_file(FILE* input, FILE* output, const char* key);

char* get_password(const char*);
void hide_output();
void restore_output();





#endif // AES_FILE_ENCRYPTION_HEADER Included
