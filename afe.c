#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <afe.h>
#include <aes.h>

enum ACTION {
    ENCRYPT,
    DECRYPT,
    VERIFY,
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
    enum TOKEN_RETURN ret;  // Return result from the function
    char* inFile;           // Name of the input file
    char* outFile;          // Name of the output file (if specified)
    char* flag;             // Flag specified
    enum ACTION action;     // Action (encrypt or decrypt)
    int removeBak;          // If set, removes the backup
    FILE_t* in_fp;
    FILE_t* out_fp;
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

    if      (tok.action == ENCRYPT) {result = encrypt_file(tok.in_fp,tok.out_fp,pwd1);}
    else if (tok.action == DECRYPT) {result = decrypt_file(tok.in_fp,tok.out_fp,pwd1);}
    else                            {result = verify_file(tok.in_fp,pwd1);}

    if (result < NO_ERROR) {
        if (tok.action == ENCRYPT)   {printf("Error encrypting file!\n");}
        else                         {printf("Error decrypting file!\n");}
    } else if (result == VERIFY_OK)  {printf("Password is correct!\n");}
      else if (result == VERIFY_BAD) {printf("Password is NOT correct!\n"); tok.removeBak = C_UNDO;}

error:
    if (tok.in_fp) {close_file(tok.in_fp,tok.removeBak);}
    if (tok.out_fp && (tok.in_fp != tok.out_fp)) {
        close_file(tok.out_fp,tok.removeBak);
    }

    if (pwd1) {free(pwd1);}
    if (pwd2) {free(pwd2);}
    return 0;
}








//Nice function to parse all tokens
static Tokens_t parseTokens(int argc, char** argv) {

    Tokens_t tok = {OKAY};
    if (argc < 3) {tok.ret = SHOW_HELP; goto end;}

    //Get encrypt/decrypt flag
    tok.action = UNKNOWN;
    if (!strcmp(argv[1],"-e")) {tok.action = ENCRYPT;}
    if (!strcmp(argv[1],"-d")) {tok.action = DECRYPT;}
    if (!strcmp(argv[1],"-v")) {tok.action = VERIFY;}

    if (tok.action == UNKNOWN) {
        tok.flag = argv[1];
        tok.ret = BAD_FLAG;
        goto end;
    }

    if (tok.action != VERIFY) {
        if (argc > 3) {
            //Test for remove backup flag
            if (!strcmp(argv[2],"-r")) {tok.removeBak = C_REMOVE;}

            if (tok.removeBak) {
                tok.inFile = argv[3];
                if (argc > 4) {tok.outFile = argv[4];}
            } else {
                tok.inFile = argv[2];
                tok.outFile = argv[3];
            }
        } else {
            tok.inFile = argv[2];
        }

        //Try to open the files needed
        if (!tok.outFile) {
            //Same file for reading and writing
            if (open_file(tok.inFile,F_READ_WRITE,AES_KEYLEN,&tok.in_fp) != NO_ERROR) {
                tok.ret = OPEN_FILE_ERROR;
            }
            tok.out_fp = tok.in_fp;
        } else {
            if ((open_file(tok.inFile,F_READ,AES_KEYLEN,&tok.in_fp) != NO_ERROR)
                || (open_file(tok.outFile,F_WRITE,AES_KEYLEN,&tok.out_fp) != NO_ERROR)) {
                tok.ret = OPEN_FILE_ERROR;
            }
        }
    } else {
        tok.inFile = argv[2];
        if (open_file(tok.inFile,F_READ,AES_KEYLEN,&tok.in_fp) != NO_ERROR) {
            tok.ret = OPEN_FILE_ERROR;
        }
    }

end:
    return tok;
}




static void showHelp(const char* programName) {
    printf("\nAES File Encryption (AFE)\n");
    printf("Encrypt or decrypt a file using AES-256\n\n");
    printf("Usage: afe (-e | -d | -v) [-r] <in-file> [out-file]\n");
    printf("  -e = Encrypt <in-file>\n");
    printf("  -d = Decrypt <in-file>\n");
    printf("  -v = Verify encryption of <in-file>\n");
    printf("  -r = Remove backup file from the hard drive (when [out-file] isn't specified)\n");
    printf("\nIf [out-file] isn't specified, then encryption or decryption overwrites <in-file>.\n"
            "However, a backup is created named '<in-file>.bak.'\n\n");
}
