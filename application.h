/**
 * @file application.h
 * @author SamiAsfary (https://github.com/SamiAsfary)
 * @brief 
 * @version 1.0
 * @date 2024-10-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include  "string.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h> 
#include <stdio.h> 

#define CMD_NB 5
#define JSON_PERM (S_IRWXU|S_IRWXG|S_IRWXO)

#define NO_OPTIONS 0b00000000

#define NEWAPP_SPONTANIOUS   0b1 << 0
#define NEWAPP_RECEIVED      0b1 << 1

#define STATUSAPP_DECLINED   0b1 << 0
#define STATUSAPP_ACCEPTED   0b1 << 1
#define STATUSAPP_REJECTED   0b1 << 2
#define STATUSAPP_GHOSTED    0b1 << 3
#define STATUSAPP_HRINTERVIEW    0b1 << 4
#define STATUSAPP_TECHINTERVIEW  0b1 << 5
#define STATUSAPP_OFFER      0b1 << 6
#define STATUSAPP_DISPLAY    0b1 << 7

#define UPDATEAPPS_DIRECTORY 0b1 << 0
#define UPDATEAPPS_RETRO     0b1 << 1

#define LSAPP_LIST   0b1 << 0



typedef void (* functionCmd_t)(char * argv[], int argc);

typedef struct{
    functionCmd_t commandFunc;
    char commandStr[15];
    char commandHelp[256];
}cmd_t;

void newApp(char *argv[],int argc);
void statusApp(char *argv[],int argc);
void updateApps(char *argv[],int argc);
void lsApp(char *argv[],int argc);
void helpApp(char *argv[],int argc);

void applicationCmd(char *argv[],int argc);