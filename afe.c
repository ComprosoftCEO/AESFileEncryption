#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <afe.h>

enum ACTION {
    ENCRYPT,
    DECRYPT,
    UNKNOWN
};

enum TOKEN_RETURN {
    SHOW_HELP,
    BAD_FLAG,
    OPEN_FILE_ERROR,
    OKAY
};

//Parsed tokens from the input
typedef struct {
    char* inFile;           //Name of the input file
    char* flag;             //Flag specified
    enum ACTION action;     //Action (encrypt or decrypt)
    enum TOKEN_RETURN ret;  //Return result from the function
    FILE_t* fp;
} Tokens_t;
static Tokens_t parseTokens(int argc, char** argv);

static void showHelp(const char* programName);


int main(int argc, char** argv) {
    Tokens_t tok = parseTokens(argc,argv);
    char* pwd1 = NULL, *pwd2 = NULL;

    switch(tok.ret) {
        case SHOW_HELP:         showHelp(argv[0]); return 0;
        case BAD_FLAG:          printf("Unknown flag '%s'\n",tok.flag); return 0;
        case OPEN_FILE_ERROR:   printf("Unable to open file '%s'\n",tok.inFile); perror("Open File"); return 0;
        default: break;
    }

    int result;
    if (tok.action == ENCRYPT) {   //For encryption, confirm password
        while(1) {
            pwd1 = get_password("Enter Password: ");
            pwd2 = get_password("Confirm Password: ");

            if (!strcmp(pwd1,pwd2)) {break;}

            printf("Error! Passwords do not match.\n");
            goto error;
        }

    } else {
        pwd1 = get_password("Enter Password: ");
    }

    if (tok.action == ENCRYPT) {result = encrypt_file(tok.fp,pwd1);}
    else                       {result = decrypt_file(tok.fp,pwd1);}


    if (result) {
        if (tok.action == ENCRYPT) {printf("Error encrypting file!\n");}
        else                       {printf("Error decrypting file!\n");}
    }

error:
    if (tok.fp) {close_file(tok.fp);}
    if (pwd1) {free(pwd1);}
    if (pwd2) {free(pwd2);}
    return 0;
}


static Tokens_t parseTokens(int argc, char** argv) {

    Tokens_t tok;
    if (argc < 3) {tok.ret = SHOW_HELP; goto end;}

    tok.action = UNKNOWN;
    if (!strcmp(argv[1],"-e")) {tok.action = ENCRYPT;}
    if (!strcmp(argv[1],"-d")) {tok.action = DECRYPT;}

    if (tok.action == UNKNOWN) {
        tok.flag = argv[1];
        tok.ret = BAD_FLAG;
        goto end;
    }

    tok.inFile = argv[2];

    //Try to open the file
    if (open_file(tok.inFile,tok.inFile,".bak",&tok.fp) != NO_ERROR) {
        tok.ret = OPEN_FILE_ERROR;
    }

    tok.ret = OKAY;

end:
    return tok;
}

static void showHelp(const char* programName) {
    printf("\nAES File Encryption (AFE)\n");
    printf("Encrypt or decrypt a file using AES-256\n\n");
    printf("Usage: afe (-e | -d) <file>\n");
    printf("  -e = Encrypt <file>\n");
    printf("  -d = Decrypt <file>\n");
//    printf("  -r = Remove old file from the hard drive\n");
    printf("\nIf -r is not specified, the new file will be named <file>.d or <file>.e\n\n");
}
