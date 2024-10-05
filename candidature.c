#include "candidature.h"
#include <jansson.h>


const char* commandStr[CMD_NB] = {
    "newCd",
    "status",
    "lsCd",
    "updateCds"
};

const functionCmd_t commandFunc[CMD_NB] = {
    &newCd,
    &statusCd,
    &lsCd,
    &updateCds
};

typedef enum{
    newCd_sel,
    statusCd_sel,
    lsCd_sel,
    updateCds_sel
}cmd_sel_t;

static uint8_t testForOptions(char *possibleOptions, cmd_sel_t currentCmd){
    uint8_t selectedOptions = NO_OPTIONS;
    int lenOptions = strlen(possibleOptions);
    if(possibleOptions[0] != '-' || lenOptions < 2){
        return selectedOptions;
    }
    char charCheck[9] = "\0\0\0\0\0\0\0\0\0";
    int lenCheck = 0;
    switch(currentCmd){
        case newCd_sel:
        lenCheck = strlen(strcpy(charCheck,"sr"));
        break;
        case statusCd_sel:
        lenCheck = strlen(strcpy(charCheck,"nyrghtod"));
        break;
        case lsCd_sel:
        lenCheck = strlen(strcpy(charCheck,"sr"));
        break;
        case updateCds_sel:
        lenCheck = strlen(strcpy(charCheck,""));
        break;
        default:
        break;
    }
    int indexOptions = 1,indexCheck = 0;
    while(indexOptions < lenOptions){
        indexCheck = 0;
        while(indexCheck < lenCheck && charCheck[indexCheck] != '\0'){
            if(charCheck[indexCheck] == possibleOptions[indexOptions]){
                selectedOptions |= 1 << indexCheck;
            }
            indexCheck++;
        }
        indexOptions++;
    }
    sprintf(charCheck,"%#02X",selectedOptions);
    write(STDOUT_FILENO,charCheck,strlen(charCheck));
    return selectedOptions;
}

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
/*
static void checkSubfolderJson(char *folder,int nbCd){
    char * buffer;
    int fd;
    json_t * jd;
    json_error_t error;
    json_t *arr1 = json_array(); 
    buffer = malloc(strlen(folder)+strlen("/Cd.json"));
    sprintf(buffer, "%s/Cd.json",folder);

    fd = open(buffer, O_RDONLY);
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    json_unpack(jd,"{s: s, s: i, s: o","entreprise",folder,"Cd Total", nbCd, "applications", &arr1);
    close(fd);
    free(buffer);
}
*/
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
}

static void checkSubfolder(char *folder){
    DIR *d;
    struct dirent *dir;
    d = opendir(folder);
    unsigned char nbCd = 0, isJsonHere = 0; 
    if(d == NULL){
        exit(EXIT_FAILURE);
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && !strncmp("Cd", dir->d_name,2)){
            nbCd++;
        }else if(dir->d_type == 8 && !strncmp("Cd.json", dir->d_name,7)){
            isJsonHere = 1;
        }
    }
    closedir(d);
    printf("In %s there is %u Cd\n", folder, nbCd);
    if(isJsonHere){
        
    }else{
        initJSON(folder, "Applied to offer");
        for(int cd = 2; cd <= nbCd; cd++){
            addingJSON(folder,"Applied to offer",cd);
        }
    }
    
}

static void displayCd(char* company, int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // entreprise + /Cd.json\0
    sprintf(buffer,"%s/Cd.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        perror("Erreur");
        exit(EXIT_FAILURE);
    }
    json_t *jd, *app_arr, *app, *steps, *string;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    app_arr = json_object_get(jd,"applications");
    app = json_array_get(app_arr,index);
    steps = json_object_get(app,"Step");

    buffer = realloc(buffer,strlen(company)+28); // The XXth application for company\r\n\0
    char order[2];
    switch(index%10+1){
        case 1:
        order[0] = 's';
        order[1] = 't';
        break;
        case 2:
        order[0] = 'n';
        order[1] = 'd';
        break;
        case 3:
        order[0] = 'r';
        order[1] = 'd';
        break;
        default:
        order[0] = 't';
        order[1] = 'h';
        break;
    }

    sprintf(buffer,"The %d%s application for %s\r\n",index+1,order,company);
    write(STDOUT_FILENO,buffer,strlen(buffer));
    write(STDOUT_FILENO,"Started with : ",strlen("Started with : "));
    string = json_object_get(app,"Start");
    write(STDOUT_FILENO,json_string_value(string),json_string_length(string));
    write(STDOUT_FILENO,"\r\nCurrent Steps : [",strlen("\r\nCurrent Steps : ["));
    for(int i = 0; i < (int)json_array_size(steps);i++){
        write(STDOUT_FILENO,json_string_value(json_array_get(steps,i)),json_string_length(json_array_get(steps,i)));
        if(i != (int)json_array_size(steps)-1){
            write(STDOUT_FILENO,", ",strlen(", "));
        }
    }
    write(STDOUT_FILENO,"]\r\nCurrent ending Status : ",strlen("]\r\nCurrent ending Status : "));
    string = json_object_get(app,"End");
    write(STDOUT_FILENO,json_string_value(string),json_string_length(string));
    write(STDOUT_FILENO,"\n",strlen("\n"));
}

static void addStatusCd(char* company, int index, char* addon, uint8_t isCdEnding){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // entreprise + /Cd.json
    sprintf(buffer,"%s/Cd.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        perror("Erreur");
        exit(EXIT_FAILURE);
    }

    json_t *jd, *app_arr, *app, *steps;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    app_arr = json_object_get(jd,"applications");
    app = json_array_get(app_arr,index);
    if(isCdEnding == 0){
        steps = json_object_get(app,"Step");
        json_array_append(steps,json_string(addon));
        json_object_set(app,"Step",steps);
    }else{
        json_object_set(app,"End",json_string(addon));
    }
    json_array_set(app_arr,index,app);
    json_object_set(jd,"applications",app_arr);

    fd = open(buffer, O_RDWR | O_TRUNC | O_CREAT, JSON_PERM);
    json_dumpfd(jd, fd, JSON_INDENT(4));
    close(fd);
    free(buffer);
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

    char * company = malloc(strlen(argv[argc-1]));
    sprintf(company,"%s",argv[argc-1]);
    
    char * path = malloc(strlen(company)+5); // argv[1] + \Cd1
    
    struct stat st = {0};
    char candStart[24] = "Applied to offer";
    uint8_t selectedOptions = testForOptions(argv[1],newCd_sel);
    if((selectedOptions & NEWCD_SPONTANIOUS) == NEWCD_SPONTANIOUS){
        sprintf(candStart,"Spontanious candidature");
    }else if((selectedOptions & NEWCD_RECEIVED) == NEWCD_RECEIVED){
        sprintf(candStart,"Received proposition");
    }

    if(stat(company, &st) == -1) {
        sprintf(path,"%s/Cd1",company);
        if (mkdir(company, JSON_PERM) != 0 || mkdir(path, JSON_PERM) != 0){
            free(path);
            exit(EXIT_FAILURE);
        }
        initJSON(company, candStart);
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    int i = 0;
    do{
        i++;
        sprintf(path,"%s/Cd%d",company,i);
    }while( stat(path, &st) == 0 && i < 9);
    printf("\npath : %s\n",path);
    addingJSON(company,candStart,i);
    if (mkdir(path, JSON_PERM) == 0){
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    free(path);
    exit(EXIT_FAILURE);
}

void statusCd(char *argv[],int argc){
    
    uint8_t options = testForOptions(argv[1],statusCd_sel);

    if(argc < 3 || (options != NO_OPTIONS && argc < 4)){
        exit(EXIT_FAILURE);
    }

    if(options == NO_OPTIONS){
        displayCd(argv[argc-2],atoi(argv[argc-1])-1);
        exit(EXIT_SUCCESS);
        
    }

    char* addon;
    uint8_t isCdEnding = 0;
    if((options&STATUSCD_DECLINED) == STATUSCD_DECLINED){
        addon = malloc(strlen("Offer Declined")+1);
        strcpy(addon,"Offer Declined");
        isCdEnding = 1;
    }else if((options&STATUSCD_ACCEPTED) == STATUSCD_ACCEPTED){
        addon = malloc(strlen("Offer Accepted")+1);
        strcpy(addon,"Offer Accepted");
        isCdEnding = 1;
    }else if((options&STATUSCD_REJECTED) == STATUSCD_REJECTED){
        addon = malloc(strlen("Rejected")+1);
        strcpy(addon,"Rejected");
        isCdEnding = 1;
    }else if((options&STATUSCD_GHOSTED) == STATUSCD_GHOSTED){
        addon = malloc(strlen("Ghosted")+1);
        strcpy(addon,"Ghosted");
        isCdEnding = 1;
    }else if((options&STATUSCD_HRINTERVIEW) == STATUSCD_HRINTERVIEW){
        addon = malloc(strlen("HR interview")+1);
        strcpy(addon,"HR interview");
    }else if((options&STATUSCD_TECHINTERVIEW) == STATUSCD_TECHINTERVIEW){
        addon = malloc(strlen("Technical interview")+1);
        strcpy(addon,"Technical interview");
    }else if((options&STATUSCD_OFFER) == STATUSCD_OFFER){
        addon = malloc(strlen("Offer received")+1);
        strcpy(addon,"Offer received");
    }

    addStatusCd(argv[argc-2],atoi(argv[argc-1])-1,addon,isCdEnding);
    exit(EXIT_SUCCESS);

}

void updateCds(char *argv[],int argc){
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && dir->d_name[0] != '.'){
            printf("%u : %s\n",dir->d_type, dir->d_name);
            checkSubfolder(dir->d_name);
        }
    }
    closedir(d);
    }
    argv[argc-1] = argv[argc-1];
    exit(EXIT_SUCCESS);
}

void lsCd(char *argv[],int argc){
    argv[argc-1] = argv[argc-1];
}

