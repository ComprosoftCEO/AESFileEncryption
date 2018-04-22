//Cross-Platform code to hide stdout
#include <stdio.h>

#ifdef _WIN32
    #include <io.h>
    char * const nulFileName = "NUL";
    #define CROSS_DUP(fd) _dup(fd)
    #define CROSS_DUP2(fd, newfd) _dup2(fd, newfd)
#else
    #include <unistd.h>
    char * const nulFileName = "/dev/null";
    #define CROSS_DUP(fd) dup(fd)
    #define CROSS_DUP2(fd, newfd) dup2(fd, newfd)
#endif


static int stdoutBackupFd;  //Holds the legit version of stdout

void hide_output() {

    FILE *nullOut;
    /* duplicate stdout */
    stdoutBackupFd = CROSS_DUP(STDOUT_FILENO);

    fflush(stdout);
    nullOut = fopen(nulFileName, "w");
    CROSS_DUP2(fileno(nullOut), STDOUT_FILENO);

    fflush(stdout);
    fclose(nullOut);
}

void restore_output() {
    CROSS_DUP2(stdoutBackupFd, STDOUT_FILENO);
    close(stdoutBackupFd);
}
