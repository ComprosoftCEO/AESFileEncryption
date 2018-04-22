#ifndef AES_FILE_ENCRYPTION_HEADER
#define AES_FILE_ENCRYPTION_HEADER

#include <stdio.h>

int encrypt_file(FILE* input, FILE* output, const char* key);
int decrypt_file(FILE* input, FILE* output, const char* key);

char* get_password(const char*);
void hide_output();
void restore_output();


//Error codes
#define BAD_KEY      -1
#define READ_ERROR   -2
#define WRITE_ERROR  -3


#endif // AES_FILE_ENCRYPTION_HEADER Included
