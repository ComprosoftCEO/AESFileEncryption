#include <afe.h>
#include <dynamic-array.h>
#include <readline/readline.h>


char* get_password(const char* prompt) {

    printf("%s",prompt);
    fflush(stdout);

    hide_output();
    char* ret = readline("");
    restore_output();

    printf("\n");
    fflush(stdout);
    return ret;
}


//************** LEGACY CODE BELOW ******************
static void init_terminal() {
    rl_prep_terminal(0);
}

static void close_terminal() {
    rl_deprep_terminal();
}

static void flush_buffer(FILE* fp) {
    fseek(fp,0,SEEK_END);
}

static inline int is_ascii(char c) {
    return (c > 31) && (c < 127);
}


//Legacy way, not used, doesn't work
char* get_password_char(const char* prompt, char c) {
    init_terminal();
    pDynamic_Arr_t arr = new_dynamic_array(sizeof(char));
    if (!arr) {return NULL;}

    char key = 1;
    do {
        key = rl_getc(stdin);
        if (!is_ascii(key)) {
            flush_buffer(stdin);
        } else {
            printf("%c",c);
            fflush(stdout);
            add_array_element(arr,&key);
        }
    } while (key != RETURN);

    close_terminal();

    char* ret = flush_dynamic_array(arr);
    free_dynamic_array(arr,NULL);
    return ret;
}
