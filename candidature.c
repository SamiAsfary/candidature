#include "candidature.h"

const char* commandStr[] = {
    "newCd",
    "status",
    "lsCd",
    "updateCd"
};

const functionCmd_t commandFunc[] = {
    &newCd,
    &statusCd,
    &lsCd,
    &updateCd
};

void candidatureCmd(char *argv[],int argc){

    for(int loop = 0; loop < CMD_NB; loop++){
        if(!strncmp(argv[0],commandStr[loop],strlen(commandStr[loop]))){
            write(STDOUT_FILENO, "\nKTHXBYE\n", strlen("\nKTHXBYE\n"));
        }
    }
}

void newCd(char *argv[], int argc){

    unsigned char charnb = 0;
    while(argv[1][charnb] =! "\0"){
        charnb++;
    }///
    printf("%c",argv[1]);
}

void statusCd(char *argv[],int argc){

}

void updateCd(char *argv[],int argc){

}

void lsCd(char *argv[],int argc){

}
