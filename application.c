/**
 * @file application.c
 * @author SamiAsfary (https://github.com/SamiAsfary)
 * @brief This file provide the 
 * @version 1.0
 * @date 2024-10-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "application.h"
#include <jansson.h>

/**
 * @brief   This array stores a short description of each command.
 * 
 */
const char* commandHelpShort[CMD_NB] = {
    "Create entry for new application",
    "Display status for each application",
    "Display every company with how many application was done",
    "Init json with pre-existing application",
    "Display this message, helpApp helpApp for more specification"
};

/**
 * @brief   This array stores a longer description of each function with 
 *          a description of each options.
 * 
 */
const char* commandHelp[CMD_NB] = {
    COLOUR_CYAN"newApp"COLOUR_GREEN" -[OPTIONS] COMPANY_NAME\r\n"COLOUR_OFF
    "\tCreate a new application for"COLOUR_GREEN" COMPANY_NAME"COLOUR_OFF" with \"Applied to offer\" as a stating reason\r\n"
    "Options :\r\n"
    COLOUR_GREEN"\t-s"COLOUR_OFF"  Change starting reason to \"Spontanious application\"\r\n"
    COLOUR_GREEN"\t-r"COLOUR_OFF"  Change starting reason to \"Received proposition\"\r\n",
    ////////////////////////////////////
    COLOUR_CYAN"statusApp"COLOUR_GREEN" -OPTIONS COMPANY_NAME APPLICATION_NUMBER\r\n"COLOUR_OFF
    "\tDisplay current status of"COLOUR_GREEN" APPLICATION_NUMBER"COLOUR_OFF"th at"COLOUR_GREEN" COMPANY_NAME\r\n"COLOUR_OFF
    "\tIf an option is specified update status of application number"COLOUR_GREEN" APPLICATION_NUMBER"COLOUR_OFF" at"COLOUR_GREEN" COMPANY_NAME"COLOUR_OFF"\r\n"
    "Options :\r\n"
    COLOUR_GREEN"\t-n"COLOUR_OFF"  Change ending reason to \"Offer Declined\"\r\n"
    COLOUR_GREEN"\t-y"COLOUR_OFF"  Change ending reason to \"Offer Accepted\"\r\n"
    COLOUR_GREEN"\t-r"COLOUR_OFF"  Change ending reason to \"Rejected\"\r\n"
    COLOUR_GREEN"\t-g"COLOUR_OFF"  Change ending reason to \"Ghosted\"\r\n"
    COLOUR_GREEN"\t-h"COLOUR_OFF"  Change last status to \"HR interview\"\r\n"
    COLOUR_GREEN"\t-t"COLOUR_OFF"  Change last status to \"Technical interview\"\r\n"
    COLOUR_GREEN"\t-o"COLOUR_OFF"  Change last status to \"Offer received\"\r\n",
    ////////////////////////////////////
    COLOUR_CYAN"lsApp"COLOUR_GREEN" -[OPTIONS] [COMPANY_NAME]\r\n"COLOUR_OFF
    "\tDisplay a list of every company an application was started on.\r\n"
    "\tIf a "COLOUR_GREEN"[COMPANY_NAME]"COLOUR_OFF" is specified display each application for said company.\r\n"
    "Options :\r\n"
    COLOUR_GREEN"\t-l"COLOUR_OFF" Also display application status for each application.\r\n",
    ////////////////////////////////////
    COLOUR_CYAN"updateApps"COLOUR_GREEN" -[OPTIONS] \r\n"COLOUR_OFF
    "\tInit Json in every company folder\r\n"
    "Options :\r\n"
    COLOUR_GREEN"\t-d"COLOUR_OFF" Also check if the json is sync with the pre-existing application.\r\n"
    COLOUR_GREEN"\t-r"COLOUR_OFF" Allow retro compatibility by changing Cd and candidature by App et application.\r\n",
    ////////////////////////////////////
    COLOUR_CYAN"helpApp"COLOUR_GREEN" [COMMAND]\r\n"COLOUR_OFF
    "\tDisplay each command and their uses.\r\n"
    "\tIf a"COLOUR_GREEN" [COMMAND]"COLOUR_OFF" is entered display a longer explanation for it.\r\n"
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


/**
 * @brief remove each instance of a char from a string
 * 
 * @param str pointer to string 
 * @param c character to remove
 * 
 * @retval str 
 */
static void strrmch(char* str, char c){
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

/**
 * @brief convert command string to enum command selection
 * 
 * @param cmdStr string to convert
 * @return cmd_sel_t enum corresponding to cmdStr
 */
static cmd_sel_t strToSel(char* cmdStr){
    for(cmd_sel_t cmdSel = newApp_sel; cmdSel < CMD_NB; cmdSel++){
        if(strcmp(commandStr[cmdSel],cmdStr) == 0){
            return cmdSel;
        }
    }
    return newApp_sel;
}

/**
 * @brief Check if the selected command line include options
 * 
 * @param possibleOptions pointer to the string possibly containing options
 * @param currentCmd enum selecting working command
 * @return uint8_t return options as defined in application.h
 */
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
        lenCheck = strlen(strcpy(charCheck,"dr"));
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
    return selectedOptions;
}

/**
 * @brief   search if there is quote in command line, if so concatenate
 *          every arguments in quote inside one argument.
 * @warning change argv, if argv isn't used with another function in 
 *          this file (application.c) error may occur.
 * @param argv command line received
 * @param argc number of argument
 * @return int return new number of argument
 */
static int testForQuote(char *argv[],int argc){
    if(argc > 1){
        int startingIndex = 1, isFirstApostrophe = 0;
        if(testForOptions(argv[1],strToSel(argv[0])) != NO_OPTIONS){
            startingIndex = 2;
        }
        int i = startingIndex;
        while(i < argc){
            if(isFirstApostrophe == 0 && argv[i][0] == '\"'){
                startingIndex = i;
                isFirstApostrophe = 1;
            }else if(argv[i][strlen(argv[i])-1] == '\"'){
                for(int j = startingIndex+1; j <= i; j++){
                    strcat(argv[startingIndex]," ");
                    strcat(argv[startingIndex],argv[j]);
                }
                for(int j = 1; i + j < argc; j++){
                    argv[startingIndex + j] = argv[i + j];
                }
                argc = argc - (i - startingIndex);
                strrmch(argv[startingIndex],'\"');
                break;
            }
            i++;
        }

    }
    return argc;
}

/**
 * @brief Display the shallow data of one company.
 * 
 * @param company string of selected company
 */
static void lsAppList1line(char* company){
    char* buffer, *outputStr;
    json_error_t error;
    int nbApp = 0, nbAppOpen = 0;
    json_t *jd, *app_arr, *app;
    buffer = malloc(strlen(company)+9); // company + /App.json
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror(COLOUR_RED"Error opening JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd || !json_is_object(jd)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    nbApp = (int) json_integer_value(json_object_get(jd,"App Total"));
    app_arr = json_object_get(jd,"applications");
    if(!json_is_array(app_arr)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading application array "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    for(int index = 0; index < nbApp; index++){
        app = json_array_get(app_arr,index);
        if(!json_is_object(app)) {
            free(buffer);
            close(fd);
            perror(COLOUR_RED"Error loading application JSON "COLOUR_OFF);
            exit(EXIT_FAILURE);
        }
        if(strcmp("Ongoing",json_string_value(json_object_get(app,"End"))) != 0){
            nbAppOpen++;
        }
    }
    outputStr = malloc(strlen(company) + 4 + 6); // company + '\t' + total + '\t' + ongoing + \r\n\0
    sprintf(outputStr,COLOUR_CYAN"%s"COLOUR_OFF"\t\t%d\t%d\r\n",company,nbApp,nbAppOpen);
    write(STDOUT_FILENO,outputStr,strlen(outputStr));
}

/**
 * @brief Initialise JSON of a company with a set Start for the first application.
 * 
 * @param company string of the company name
 * @param start string of the start
 */
static void initJSON(char *company, char *start){
    char* buffer;
    buffer = malloc(strlen(company)+9); // company + /App.json\0
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
        perror(COLOUR_RED"Error creating JSON file "COLOUR_OFF);
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

/**
 * @brief Add a new application with set start and index to JSON of the company
 * @todo Add error if index != (application number + 1)
 * @param company string of the company name
 * @param start string of the start
 * @param index index of the application
 */
static void addingJSON(char *company, char *start,int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror(COLOUR_RED"Error opening JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    json_t * jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    close(fd);

    fd = open(buffer, O_RDWR | O_TRUNC | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror(COLOUR_RED"Error re-opening JSON file "COLOUR_OFF);
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

/**
 * @brief   This function checks if the folder contains the JSON;
 *          And if not init the JSON with the correct number of application;
 *          The default start is "Applied to offer"
 * @todo Add check if number of application in JSON is the same as number of folder
 * @param folder name of the folder to check in
 */
static void checkSubfolder(char *folder){
    DIR *d;
    struct dirent *dir;
    d = opendir(folder);
    unsigned char nbApp = 0, isJsonHere = 0; 
    if(d == NULL){
        char *errorStr;
        errorStr = malloc(strlen("Error while opening x folder ") + strlen(folder));
        sprintf(errorStr,COLOUR_RED"Error while opening"COLOUR_PURPLE" %s"COLOUR_RED" folder "COLOUR_OFF,folder);
        perror(errorStr);
        free(errorStr);
        exit(EXIT_FAILURE);
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && !strncmp("App", dir->d_name,3)){
            nbApp++;
        }else if(dir->d_type == 8 && !strncmp("App.json", dir->d_name,8)){
            isJsonHere = 1;
        }
    }
    closedir(d);
    //printf("In %s there is %u App\n", folder, nbApp);
    if(isJsonHere){
        
    }else{
        initJSON(folder, "Applied to offer");
        for(int App = 2; App <= nbApp; App++){
            addingJSON(folder,"Applied to offer",App);
        }
    }
}

/**
 * @brief This function change Cd.json to App.json if the former exists
 * 
 * @param folder name of the folder to check in
 */
static void checkFolderJSON(char *folder){
    DIR *d;
    struct dirent *dir;
    d = opendir(folder);
    unsigned char nbApp = 0, isCdJsonHere = 0; 
    if(d == NULL){
        char *errorStr;
        errorStr = malloc(strlen("Error while opening x folder ") + strlen(folder));
        sprintf(errorStr,COLOUR_RED"Error while opening"COLOUR_PURPLE" %s"COLOUR_RED" folder "COLOUR_OFF,folder);
        perror(errorStr);
        free(errorStr);
        exit(EXIT_FAILURE);
    }
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && !strncmp("App", dir->d_name,3)){
            nbApp++;
        }else if(dir->d_type == 8 && strncmp("Cd.json", dir->d_name,7) == 0){
            isCdJsonHere = 1;
            write(STDOUT_FILENO,dir->d_name,strlen(dir->d_name));
        }
    }
    closedir(d);
    if(isCdJsonHere == 1){
        char *buffer = malloc(strlen(folder) + 9);// company + "Cd.json\0"
        json_t *app_arr, *jd, *newjd;
        json_error_t error;
        int test;
        sprintf(buffer,"%s/Cd.json",folder);
        int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
        if(fd == -1){
            char *errorStr;
            errorStr = malloc(strlen("Error while opening x file ") + strlen(buffer));
            sprintf(errorStr,COLOUR_RED"Error while opening"COLOUR_PURPLE" %s"COLOUR_RED" file "COLOUR_OFF,buffer);
            free(buffer);
            perror(errorStr);
            exit(EXIT_FAILURE);
        }
        jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
        if(!jd || !json_is_object(jd)) {
            free(buffer);
            close(fd);
            perror(COLOUR_RED"Error loading JSON file "COLOUR_OFF);
            exit(EXIT_FAILURE);
        }
        close(fd);
        app_arr = json_object_get(jd,"applications");
        if(!json_is_array(app_arr)) {
            free(buffer);
            perror(COLOUR_RED"Error loading application array "COLOUR_OFF);
            exit(EXIT_FAILURE);
        }
        test = json_integer_value(json_object_get(jd,"Cd Total"));
        newjd = json_pack("{s: s, s: i, s: O}","company",folder,"App Total", test, "applications",app_arr);
        sprintf(buffer,"%s/App.json",folder);
        fd = open(buffer, O_RDWR | O_CREAT, JSON_PERM);
        if(fd == -1){
            free(buffer);
            perror(COLOUR_RED"Error re-opening JSON file "COLOUR_OFF);
            exit(EXIT_FAILURE);
        }
        json_dumpfd(newjd, fd, JSON_INDENT(4));
        close(fd);
        char *bufferName = malloc(strlen(folder) + 9);// company + "Cd.json\0"
        for(int i = 1;i <= test;i++){
            sprintf(buffer,"%s/Cd1",folder);
            sprintf(bufferName,"%s/App1",folder);
            rename(buffer,bufferName);
        }
        free(buffer);
        free(bufferName);

    }
}

/**
 * @brief Display in terminal the complete status of an application.
 * 
 * @param company company applied to
 * @param index index of the application
 */
static void displayApp(char* company, int index){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json\0
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror(COLOUR_RED"Error opening JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    json_t *jd, *app_arr, *app, *steps, *string;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd || !json_is_object(jd)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    app_arr = json_object_get(jd,"applications");
    if(!json_is_array(app_arr)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading application array "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    app = json_array_get(app_arr,index);
    if(!json_is_object(app)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading application JSON "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    steps = json_object_get(app,"Step");
    if(!json_is_array(steps)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading steps array"COLOUR_OFF);
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

    sprintf(buffer,COLOUR_GREEN"The %d%s application for"COLOUR_PURPLE" %s\r\n"COLOUR_OFF,index+1,order,company);
    write(STDOUT_FILENO,buffer,strlen(buffer));
    free(buffer);
    write(STDOUT_FILENO,COLOUR_CYAN"Started with"COLOUR_OFF" : ",strlen(COLOUR_GREEN"Started with"COLOUR_OFF" : "));
    string = json_object_get(app,"Start");
    write(STDOUT_FILENO,json_string_value(string),json_string_length(string));
    write(STDOUT_FILENO,COLOUR_CYAN"\r\nCurrent Steps"COLOUR_OFF" : [",strlen(COLOUR_CYAN"\r\nCurrent Steps"COLOUR_OFF" : ["));
    for(int i = 0; i < (int)json_array_size(steps);i++){
        write(STDOUT_FILENO,json_string_value(json_array_get(steps,i)),json_string_length(json_array_get(steps,i)));
        if(i != (int)json_array_size(steps)-1){
            write(STDOUT_FILENO,", ",strlen(", "));
        }
    }
    write(STDOUT_FILENO,"]"COLOUR_ORANGE"\r\nCurrent ending Status"COLOUR_OFF" : ",strlen("]"COLOUR_ORANGE"\r\nCurrent ending Status"COLOUR_OFF" : "));
    string = json_object_get(app,"End");
    write(STDOUT_FILENO,json_string_value(string),json_string_length(string));
    write(STDOUT_FILENO,"\n",strlen("\n"));
}

/**
 * @brief Add a new status to an application.
 * 
 * @param company company applied to
 * @param index index of the application
 * @param addon string of the status to add to the application
 * @param isAppEnding 1 if it change ending status, 0 otherwise
 */
static void addStatusApp(char* company, int index, char* addon, uint8_t isAppEnding){
    char* buffer;
    json_error_t error;
    buffer = malloc(strlen(company)+9); // company + /App.json
    sprintf(buffer,"%s/App.json",company);
    int fd = open(buffer, O_RDONLY | O_CREAT, JSON_PERM);
    if(fd == -1){
        free(buffer);
        perror(COLOUR_RED"Error opening JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    json_t *jd, *app_arr, *app, *steps;
    jd = json_loadfd(fd,JSON_DECODE_ANY, &error);
    if(!jd || !json_is_object(jd)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    app_arr = json_object_get(jd,"applications");
    if(!json_is_array(app_arr)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading application array "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    app = json_array_get(app_arr,index);
    if(!json_is_object(app)) {
        free(buffer);
        close(fd);
        perror(COLOUR_RED"Error loading application JSON "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    if(isAppEnding == 0){
        steps = json_object_get(app,"Step");
        if(!json_is_array(steps)) {
            free(buffer);
            close(fd);
            perror(COLOUR_RED"Error loading steps array"COLOUR_OFF);
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
        perror(COLOUR_RED"Error re-opening JSON file "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    json_dumpfd(jd, fd, JSON_INDENT(4));
    close(fd);
    free(buffer);
}
/**
 * @brief This functions check quotation and execute the selected function
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
void applicationCmd(char *argv[],int argc){
    for(int loop = 0; loop < CMD_NB; loop++){
        if(!strncmp(argv[0],commandStr[loop],strlen(commandStr[loop]))){
            argc = testForQuote(argv,argc);
            commandFunc[loop](argv,argc);
        }
    }
}

/**
 * @brief This functions create entry for new application
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
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
    //printf("\npath : %s\n",path);
    addingJSON(company,candStart,i);
    if (mkdir(path, JSON_PERM) == 0){
        write(STDOUT_FILENO,"The folder as been created\n",strlen("The folder as been created\n"));
        free(path);
        exit(EXIT_SUCCESS);
    }
    free(path);
    exit(EXIT_FAILURE);
}

/**
 * @brief This functions display status for each application
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
void statusApp(char *argv[],int argc){
    
    uint8_t options = testForOptions(argv[1],statusApp_sel);

    if(argc < 3 || (options != NO_OPTIONS && argc < 4)){
        perror(COLOUR_RED"Not enough parameter "COLOUR_OFF);
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

/**
 * @brief This functions display every company with how many application was done
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
void updateApps(char *argv[],int argc){
    DIR *d;
    struct dirent *dir;
    uint8_t options = NO_OPTIONS;
    if(argc > 1){
        options = testForOptions(argv[1],updateApps_sel);
    }
    d = opendir(".");
    if(d == NULL){
        perror(COLOUR_RED"Error while opening current folder "COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    if (d) {
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type == 4 && dir->d_name[0] != '.'){
            //printf("%u : %s\n",dir->d_type, dir->d_name);
            if(options == NO_OPTIONS){
                checkSubfolder(dir->d_name);
            }else if((options&UPDATEAPPS_RETRO) == UPDATEAPPS_RETRO){
                checkFolderJSON(dir->d_name);
            }
            
        }
    }
    closedir(d);
    }
    exit(EXIT_SUCCESS);
    argv[argc-1] = argv[argc-1];
    
}

/**
 * @brief This functions init json with pre-existing application
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
void lsApp(char *argv[],int argc){
    uint8_t selectedOptions = NO_OPTIONS;
    if(argc > 1){
        selectedOptions = testForOptions(argv[1],lsApp_sel);
    }
    DIR *dp;
    struct dirent *ep;
    int count = 0;
    dp = opendir ("./");
    if(dp == NULL){
        perror(COLOUR_RED"Couldn't open the directory"COLOUR_OFF);
        exit(EXIT_FAILURE);
    }
    if((selectedOptions&LSAPP_LIST) == LSAPP_LIST){
        write(STDOUT_FILENO,COLOUR_GREEN"company\t\t"COLOUR_BLUE"total\tongoing\n\r"COLOUR_OFF,strlen(COLOUR_GREEN"company\t\t"COLOUR_BLUE"total\tongoing\n\r"COLOUR_OFF));
    }
    while ((ep = readdir (dp)) != NULL){
        if(ep->d_type == 4 && ep->d_name[0] != '.'){
            if((selectedOptions&LSAPP_LIST) == LSAPP_LIST){
                lsAppList1line(ep->d_name);
            }else{
                write(STDOUT_FILENO,ep->d_name,strlen(ep->d_name));
                write(STDOUT_FILENO,"\t",2);
                count++;
                if(count%5 == 0){
                    write(STDOUT_FILENO,"\n",1);
                }
            }
        }
    }
    write(STDOUT_FILENO,"\n",1);
    closedir(dp);
    exit(EXIT_SUCCESS);
}

/**
 * @brief This functions display this message, helpApp helpApp for more specification
 * 
 * @param argv All argument in the command line
 * @param argc Number argument in the command line
 */
void helpApp(char *argv[],int argc){
    char *output;
    int len;
    output = malloc(277);
    if(argc < 2){
        for(int i = 0; i < CMD_NB; i++){

            len = sprintf(output,COLOUR_GREEN"%s"COLOUR_OFF" : "COLOUR_CYAN"%s\r\n"COLOUR_OFF,commandStr[i],commandHelpShort[i]);
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
        perror(COLOUR_RED"Too much argument"COLOUR_OFF);
        write(STDOUT_FILENO,commandHelp[4],strlen(commandHelp[4]));
        exit(EXIT_FAILURE);
    }
    free(output);
    exit(EXIT_SUCCESS);
}