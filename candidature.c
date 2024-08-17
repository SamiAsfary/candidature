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
            commandFunc[loop](argv,argc);
        }
    }
}

void newCd(char *argv[], int argc){
    for(int loop = 0; loop < argc; loop++){
        write(STDOUT_FILENO, argv[loop],strlen(argv[loop]));
        write(STDOUT_FILENO, "\n",1);
    }
    if(argc < 2){
        write(STDOUT_FILENO,"Too few argument\n",strlen("Too few argument\n"));
        exit(EXIT_FAILURE);
    }
    char * path = malloc(strlen(argv[1])+4); // argv[1] + \Cd1
    struct stat st = {0};
    if(stat(argv[1], &st) == -1) {
        sprintf(path,"%s/Cd1",argv[1]);
        if (mkdir(argv[1], 0777) != 0 || mkdir(path, 0777) != 0){
            free(path);
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    int i = 0;
    do{
        i++;
        sprintf(path,"%s/Cd%d",argv[1],i);
    }while( stat(path, &st) != 0 && i < 10);
    printf("\n%s\n",path);
    if (mkdir(path, 0777) == 0){
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    free(path);
    exit(EXIT_FAILURE);
}

void statusCd(char *argv[],int argc){

}

void updateCd(char *argv[],int argc){

}

void lsCd(char *argv[],int argc){

}
