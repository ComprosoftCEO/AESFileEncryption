#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <afe.h>

enum ACTION {
    ENCRYPT,
    DECRYPT,
    UNKNOWN
};

int main(int argc, char** argv)
{

    if (argc < 3) {
        printf("\nAES File Encryption (AFE)\n");
        printf("Encrypt or decrypt a file using AES-256\n\n");
        printf("Usage: afe (-e | -d) [-r] <file>\n");
        printf("  -e = Encrypt <file>\n");
        printf("  -d = Decrypt <file>\n");
        printf("  -r = Remove old file from the hard drive\n");
        printf("\nIf -r is not specified, the new file will be named <file>.d or <file>.e\n\n");

        return 0;
    }


    enum ACTION act = UNKNOWN;
    int doRemove = 0;
    char* in_filename, *out_filename;
    FILE* input = NULL, *output = NULL;
    char* pwd1 = NULL, *pwd2 = NULL;

    //Parse the action
    if (!strcmp(argv[1],"-e")) {act = ENCRYPT;}
    if (!strcmp(argv[1],"-d")) {act = DECRYPT;}
    if (act == UNKNOWN) {
        printf("Unknown flag %s\n",argv[1]);
        return 0;
    }

    //Parse remove old file???
    if (argc > 3) {
        if (!strcmp(argv[2],"-r")) {doRemove = 1;}
    }

    //Now, open the input file
    if (doRemove) {in_filename = argv[3];}
    else          {in_filename = argv[2];}
    input = fopen(in_filename,"rb");
    if (input == NULL) {perror("Input File"); goto error;}



    int result;
    if (act == ENCRYPT) {   //For encryption, confirm password
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


    //Okay, now create the output file
    //Build the output file (<in file> + (.d|.e) + null)
    out_filename = calloc(sizeof(char),strlen(in_filename)+2+1);
    strcpy(out_filename,in_filename);
    if (act == ENCRYPT) {strcat(out_filename,".e");}
    else                {strcat(out_filename,".d");}
    output = fopen(out_filename,"wb");
    if (output == NULL) {perror("Output File"); goto error;}


    if (act == ENCRYPT) {result = encrypt_file(input,output,pwd1);}
    else                {result = decrypt_file(input,output,pwd1);}


    if (result) {
        if (act == ENCRYPT) {printf("Error encrypting file!\n");}
        else                {printf("Error decrypting file!\n");}
        remove(out_filename);
    } else {
        if (doRemove) {
            remove(in_filename);
            rename(out_filename,in_filename);
        }
    }

error:
    if (input)  {fclose(input);}
    if (output) {fclose(output);}
    if (pwd1) {free(pwd1);}
    if (pwd2) {free(pwd2);}
    return 0;
}
