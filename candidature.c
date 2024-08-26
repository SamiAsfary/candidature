#include "candidature.h"
#include <jansson.h>


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

static void initJSON(char *entreprise, char *start){
    char* buffer;
    buffer = malloc(strlen(entreprise)+8); // entreprise + /Cd.json
    json_t* application0 = json_pack("{ s: i, s: s, s: [], s: s}",
        "Index", 1,
        "Start", start,
        "Step",
        "End", "Ongoing"
    );
    json_t* jd = json_pack("{s: s, s: i, s: [O]}","entreprise",entreprise,"Cd Total",1 , "applications",application0);
    sprintf(buffer, "%s/Cd.json", entreprise);
    int fd = open(buffer, O_RDWR | O_CREAT, JSON_PERM);
    if(fd == -1){
        exit(EXIT_FAILURE);
    }
    json_dumpfd(jd, fd, JSON_INDENT(4));
    
    close(fd);
    free(buffer);
}

static void addingJSON(char *entreprise, char *start,int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(entreprise)+9); // entreprise + /Cd.json
    sprintf(buffer,"%s/Cd.json",entreprise);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    json_t * jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd) {
        //TODO : implement error handling
    }
    close(fd);

    fd = open(buffer, O_RDWR | O_TRUNC | O_CREAT, JSON_PERM);
    json_t *arr1 = json_array(); 
    int test;

    

    json_unpack(jd,"{s: s, s: i, s: o","entreprise",entreprise,"Cd Total", &test, "applications", &arr1);
    if(index != test+1){}
    json_object_set(jd,"Cd Total", json_integer(test + 1));
    json_t* application = json_pack("{ s: i, s: s, s: [], s: s}",
        "Index", index,
        "Start", start,
        "Step",
        "End", "Ongoing"
    );
    json_array_append(arr1, application);
    json_dumpfd(jd, fd, JSON_INDENT(4));
    close(fd);
    free(buffer);
    exit(3);
}

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
        if (mkdir(argv[1], JSON_PERM) != 0 || mkdir(path, JSON_PERM) != 0){
            free(path);
            exit(EXIT_FAILURE);
        }
        initJSON(argv[1], "Applied to offer");
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    int i = 0;
    do{
        i++;
        sprintf(path,"%s/Cd%d",argv[1],i);
    }while( stat(path, &st) == 0 && i < 10);
    printf("\npath : %s\n",path);
    addingJSON(argv[1],"Applied to offer",i);
    if (mkdir(path, JSON_PERM) == 0){
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    free(path);
    exit(EXIT_FAILURE);
}

void statusCd(char *argv[],int argc){
    argv[argc-1] = argv[argc-1];
}

void updateCd(char *argv[],int argc){
    argv[argc-1] = argv[argc-1];
}

void lsCd(char *argv[],int argc){
    argv[argc-1] = argv[argc-1];
}
