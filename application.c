#include "application.h"
#include <jansson.h>


const char* commandHelpShort[CMD_NB] = {
    "Create entry for new application",
    "Display status for each application",
    "Display every company with how many application was done",
    "Init json with pre-existing application",
    "Display this message, helpApp helpApp for more specification"
};

const char* commandHelp[CMD_NB] = {
    "newApp -[OPTIONS] COMPANY_NAME\r\n"
    "\tCreate a new application for COMPANY_NAME with \"Applied to offer\" as a stating reason\r\n"
    "Options :\r\n"
    "\t-s  Change starting reason to \"Spontanious application\"\r\n"
    "\t-r  Change starting reason to \"Received proposition\"\r\n",
    ////////////////////////////////////
    "statusApp -OPTIONS COMPANY_NAME APPLICATION_NUMBER\r\n"
    "\tDisplay current status of APPLICATION_NUMBERth at COMPANY_NAME\r\n"
    "\tIf an option is specified update status of application number APPLICATION_NUMBER at COMPANY_NAME\r\n"
    "Options :\r\n"
    "\t-n  Change ending reason to \"Offer Declined\"\r\n"
    "\t-y  Change ending reason to \"Offer Accepted\"\r\n"
    "\t-r  Change ending reason to \"Rejected\"\r\n"
    "\t-g  Change ending reason to \"Ghosted\"\r\n"
    "\t-h  Change last status to \"HR interview\"\r\n"
    "\t-t  Change last status to \"Technical interview\"\r\n"
    "\t-o  Change last status to \"Offer received\"\r\n",
    ////////////////////////////////////
    "lsApp -[OPTIONS] [COMPANY_NAME]\r\n"
    "\tDisplay a list of every company an application was started on.\r\n"
    "\tIf a [COMPANY_NAME] is specified display each application for said company.\r\n"
    "Options :\r\n"
    "\t-l  Also display application status for each application.\r\n",
    ////////////////////////////////////
    "updateApps -[OPTIONS] \r\n"
    "\tInit Json in every company folder\r\n"
    "Options :\r\n"
    "\t-d  Also check if the json is sync with the pre-existing application.\r\n",
    ////////////////////////////////////
    "helpApp [COMMAND]\r\n"
    "\tDisplay each command and their uses.\r\n"
    "\tIf a [COMMAND] is entered display a longer explanation for it.\r\n"
};

const char* commandStr[CMD_NB] = {
    "newApp",
    "statusApp",
    "lsApp",
    "updateApps",
    "helpApp"
};

const functionCmd_t commandFunc[CMD_NB] = {
    &newApp,
    &statusApp,
    &lsApp,
    &updateApps,
    &helpApp
};

typedef enum{
    newApp_sel = 0,
    statusApp_sel,
    lsApp_sel,
    updateApps_sel,
    helpApp_sel
}cmd_sel_t;

static void strrmch_old(char * str, char c){
    for(int i = 0; str[i] != '\0'; i++){
        if(str[i] == c){
            for(int j = i; str[j-1] != '\0'; j++){
                str[j] = str[j+1];
            }
        }
    }
}

static void strrmch(char* str, char c){
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}


static uint8_t testForOptions(char *possibleOptions, cmd_sel_t currentCmd){
    uint8_t selectedOptions = NO_OPTIONS;
    int lenOptions = strlen(possibleOptions);
    if(possibleOptions[0] != '-' || lenOptions < 2){
        return selectedOptions;
    }
    char charCheck[9] = "\0\0\0\0\0\0\0\0\0";
    int lenCheck = 0;
    switch(currentCmd){
        case newApp_sel:
        lenCheck = strlen(strcpy(charCheck,"sr"));
        break;
        case statusApp_sel:
        lenCheck = strlen(strcpy(charCheck,"nyrghtod"));
        break;
        case lsApp_sel:
        lenCheck = strlen(strcpy(charCheck,"l"));
        break;
        case updateApps_sel:
        lenCheck = strlen(strcpy(charCheck,"d"));
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

static void initJSON(char *company, char *start){
    char* buffer;
    buffer = malloc(strlen(company)+8); // company + /App.json
    json_t* application0 = json_pack("{ s: i, s: s, s: [], s: s}",
        "Index", 1,
        "Start", start,
        "Step",
        "End", "Ongoing"
    );
    json_t* jd = json_pack("{s: s, s: i, s: [O]}","company",company,"App Total",1 , "applications",application0);
    sprintf(buffer, "%s/App.json", company);
    int fd = open(buffer, O_RDWR | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error creating JSON file ");
        exit(EXIT_FAILURE);
    }
    json_dumpfd(jd, fd, JSON_INDENT(4));
    
    close(fd);
    free(buffer);
}
/*
static void checkSubfolderJson(char *folder,int nbApp){
    char * buffer;
    int fd;
    json_t * jd;
    json_error_t error;
    json_t *arr1 = json_array(); 
    buffer = malloc(strlen(folder)+strlen("/App.json"));
    sprintf(buffer, "%s/App.json",folder);

    fd = open(buffer, O_RDONLY);
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    json_unpack(jd,"{s: s, s: i, s: o","company",folder,"App Total", nbApp, "applications", &arr1);
    close(fd);
    free(buffer);
}
*/

static cmd_sel_t strToSel(char* cmdStr){
    for(cmd_sel_t cmdSel = newApp_sel; cmdSel < CMD_NB; cmdSel++){
        if(strcmp(commandStr[cmdSel],cmdStr) == 0){
            return cmdSel;
        }
    }
    return newApp_sel;
}

static void addingJSON(char *company, char *start,int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error opening JSON file ");
        exit(EXIT_FAILURE);
    }
    json_t * jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd) {
        free(buffer);
        close(fd);
        perror("Error loading JSON file ");
        exit(EXIT_FAILURE);
    }
    close(fd);

    fd = open(buffer, O_RDWR | O_TRUNC | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error re-opening JSON file ");
        exit(EXIT_FAILURE);
    }
    json_t *arr1 = json_array(); 
    int test;
    json_unpack(jd,"{s: s, s: i, s: o","company",company,"App Total", &test, "applications", &arr1);
    if(index != test+1){}
    json_object_set(jd,"App Total", json_integer(test + 1));
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
    unsigned char nbApp = 0, isJsonHere = 0; 
    if(d == NULL){
        char *errorStr;
        errorStr = malloc(strlen("Error while opening x folder ") + strlen(folder));
        sprintf(errorStr,"Error while opening %s folder ",folder);
        perror(errorStr);
        free(errorStr);
        exit(EXIT_FAILURE);
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && !strncmp("App", dir->d_name,2)){
            nbApp++;
        }else if(dir->d_type == 8 && !strncmp("App.json", dir->d_name,7)){
            isJsonHere = 1;
        }
    }
    closedir(d);
    printf("In %s there is %u App\n", folder, nbApp);
    if(isJsonHere){
        
    }else{
        initJSON(folder, "Applied to offer");
        for(int App = 2; App <= nbApp; App++){
            addingJSON(folder,"Applied to offer",App);
        }
    }
}

static void displayApp(char* company, int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json\0
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error opening JSON file ");
        exit(EXIT_FAILURE);
    }
    json_t *jd, *app_arr, *app, *steps, *string;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd || !json_is_object(jd)) {
        free(buffer);
        close(fd);
        perror("Error loading JSON file ");
        exit(EXIT_FAILURE);
    }
    app_arr = json_object_get(jd,"applications");
    if(!json_is_array(app_arr)) {
        free(buffer);
        close(fd);
        perror("Error loading application array ");
        exit(EXIT_FAILURE);
    }
    app = json_array_get(app_arr,index);
    if(!json_is_object(app)) {
        free(buffer);
        close(fd);
        perror("Error loading application JSON ");
        exit(EXIT_FAILURE);
    }
    steps = json_object_get(app,"Step");
    if(!json_is_array(steps)) {
        free(buffer);
        close(fd);
        perror("Error loading steps array");
        exit(EXIT_FAILURE);
    }
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
    free(buffer);
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

static void addStatusApp(char* company, int index, char* addon, uint8_t isAppEnding){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error opening JSON file ");
        exit(EXIT_FAILURE);
    }
    json_t *jd, *app_arr, *app, *steps;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd || !json_is_object(jd)) {
        free(buffer);
        close(fd);
        perror("Error loading JSON file ");
        exit(EXIT_FAILURE);
    }
    app_arr = json_object_get(jd,"applications");
    if(!json_is_array(app_arr)) {
        free(buffer);
        close(fd);
        perror("Error loading application array ");
        exit(EXIT_FAILURE);
    }
    app = json_array_get(app_arr,index);
    if(!json_is_object(app)) {
        free(buffer);
        close(fd);
        perror("Error loading application JSON ");
        exit(EXIT_FAILURE);
    }
    if(isAppEnding == 0){
        steps = json_object_get(app,"Step");
        if(!json_is_array(steps)) {
            free(buffer);
            close(fd);
            perror("Error loading steps array");
            exit(EXIT_FAILURE);
        }
        json_array_append(steps,json_string(addon));
        json_object_set(app,"Step",steps);
    }else{
        json_object_set(app,"End",json_string(addon));
    }
    json_array_set(app_arr,index,app);
    json_object_set(jd,"applications",app_arr);

    fd = open(buffer, O_RDWR | O_TRUNC | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror("Error re-opening JSON file ");
        exit(EXIT_FAILURE);
    }
    json_dumpfd(jd, fd, JSON_INDENT(4));
    close(fd);
    free(buffer);
}

void applicationCmd(char *argv[],int argc){
    
    if(argc > 1){
        int startingIndex = 1, isFirstApostrophe = 0;
        if(testForOptions(argv[1],strToSel(argv[0])) != NO_OPTIONS){
            startingIndex = 2;
        }
        write(STDOUT_FILENO,"len", 4);
        int i = startingIndex;
        while(i < argc){
            if(isFirstApostrophe == 0 && argv[i][0] == '\"'){
                startingIndex = i;
                isFirstApostrophe = 1;
                write(STDOUT_FILENO,"found", 6);
            }else if(argv[i][strlen(argv[i])-1] == '\"'){
                for(int j = startingIndex+1; j <= i; j++){
                    strcat(argv[startingIndex]," ");
                    strcat(argv[startingIndex],argv[j]);
                }
                write(STDOUT_FILENO,argv[startingIndex],strlen(argv[startingIndex]));
                for(int j = 1; i + j < argc; j++){
                    argv[startingIndex + j] = argv[i + j];
                }
                write(STDOUT_FILENO,"found2", 7);
                argc = argc - (i - startingIndex)-1;
                argc = startingIndex + (argc - i) + 2;
                strrmch(argv[startingIndex],'\"');
                break;
            }
            i++;
            write(STDOUT_FILENO,"loop", 5);
        }

    }
    write(STDOUT_FILENO,"disp\n", 6);
    write(STDOUT_FILENO,argv[argc-1],strlen(argv[argc-1]));
    write(STDOUT_FILENO,"disp\n", 6);
    for(int loop = 0; loop < CMD_NB; loop++){
        if(!strncmp(argv[0],commandStr[loop],strlen(commandStr[loop]))){
            commandFunc[loop](argv,argc);
        }
    }
}

void newApp(char *argv[], int argc){
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
    
    char * path = malloc(strlen(company)+5); // argv[1] + \App1
    
    struct stat st = {0};
    char candStart[24] = "Applied to offer";
    uint8_t selectedOptions = testForOptions(argv[1],newApp_sel);
    if((selectedOptions & NEWAPP_SPONTANIOUS) == NEWAPP_SPONTANIOUS){
        sprintf(candStart,"Spontanious application");
    }else if((selectedOptions & NEWAPP_RECEIVED) == NEWAPP_RECEIVED){
        sprintf(candStart,"Received proposition");
    }

    if(stat(company, &st) == -1) {
        sprintf(path,"%s/App1",company);
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
        sprintf(path,"%s/App%d",company,i);
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

void statusApp(char *argv[],int argc){
    
    uint8_t options = testForOptions(argv[1],statusApp_sel);

    if(argc < 3 || (options != NO_OPTIONS && argc < 4)){
        perror("Not enough parameter ");
        exit(EXIT_FAILURE);
    }

    if(options == NO_OPTIONS){
        displayApp(argv[argc-2],atoi(argv[argc-1])-1);
        exit(EXIT_SUCCESS);
        
    }

    char* addon;
    uint8_t isAppEnding = 0;
    if((options&STATUSAPP_DECLINED) == STATUSAPP_DECLINED){
        addon = malloc(strlen("Offer Declined")+1);
        strcpy(addon,"Offer Declined");
        isAppEnding = 1;
    }else if((options&STATUSAPP_ACCEPTED) == STATUSAPP_ACCEPTED){
        addon = malloc(strlen("Offer Accepted")+1);
        strcpy(addon,"Offer Accepted");
        isAppEnding = 1;
    }else if((options&STATUSAPP_REJECTED) == STATUSAPP_REJECTED){
        addon = malloc(strlen("Rejected")+1);
        strcpy(addon,"Rejected");
        isAppEnding = 1;
    }else if((options&STATUSAPP_GHOSTED) == STATUSAPP_GHOSTED){
        addon = malloc(strlen("Ghosted")+1);
        strcpy(addon,"Ghosted");
        isAppEnding = 1;
    }else if((options&STATUSAPP_HRINTERVIEW) == STATUSAPP_HRINTERVIEW){
        addon = malloc(strlen("HR interview")+1);
        strcpy(addon,"HR interview");
    }else if((options&STATUSAPP_TECHINTERVIEW) == STATUSAPP_TECHINTERVIEW){
        addon = malloc(strlen("Technical interview")+1);
        strcpy(addon,"Technical interview");
    }else if((options&STATUSAPP_OFFER) == STATUSAPP_OFFER){
        addon = malloc(strlen("Offer received")+1);
        strcpy(addon,"Offer received");
    }

    addStatusApp(argv[argc-2],atoi(argv[argc-1])-1,addon,isAppEnding);
    exit(EXIT_SUCCESS);

}

void updateApps(char *argv[],int argc){
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if(d == NULL){
        char *errorStr;
        errorStr = malloc(strlen("Error while current folder ")+1);
        sprintf(errorStr,"Error while opening current folder ");
        perror(errorStr);
        free(errorStr);
        exit(EXIT_FAILURE);
    }
    if (d) {
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && dir->d_name[0] != '.'){
            printf("%u : %s\n",dir->d_type, dir->d_name);
            checkSubfolder(dir->d_name);
        }
    }
    closedir(d);
    }
    exit(EXIT_SUCCESS);
    argv[argc-1] = argv[argc-1];
    
}

void lsApp(char *argv[],int argc){
    exit(EXIT_SUCCESS);
    argv[argc-1] = argv[argc-1];
}

void helpApp(char *argv[],int argc){
    char *output;
    int len;
    output = malloc(277);
    if(argc < 2){
        for(int i = 0; i < CMD_NB; i++){

            len = sprintf(output,"%s : %s\r\n",commandStr[i],commandHelpShort[i]);
            write(STDOUT_FILENO,output,len);
        }
    }else if(argc == 2){
        int i = 0;
        for(i = 0; i < CMD_NB; i++){
            if(strcmp(commandStr[i],argv[argc-1]) == 0){
                break;
            }
        }
        write(STDOUT_FILENO,commandHelp[i],strlen(commandHelp[i]));
    }else{
        free(output);
        perror("Too much argument");
        write(STDOUT_FILENO,commandHelp[4],strlen(commandHelp[4]));
        exit(EXIT_FAILURE);
    }
    free(output);
    exit(EXIT_SUCCESS);
}