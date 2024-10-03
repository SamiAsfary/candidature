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

#define CMD_NB 4
#define JSON_PERM (S_IRWXU|S_IRWXG|S_IRWXO)

#define NO_OPTIONS 0b00000000

#define NEWCD_SPONTANIOUS   0b1 << 0
#define NEWCD_RECEIVED      0b1 << 1

#define STATUSCD_DECLINED   0b1 << 0
#define STATUSCD_ACCEPTED   0b1 << 1
#define STATUSCD_REJECTED   0b1 << 2
#define STATUSCD_GHOSTED    0b1 << 3
#define STATUSCD_HRINTERVIEW    0b1 << 4
#define STATUSCD_TECHINTERVIEW  0b1 << 5
#define STATUSCD_OFFER      0b1 << 6
#define STATUSCD_DISPLAY    0b1 << 7

#define LSCD_LIST   0b1 << 0



typedef void (* functionCmd_t)(char * argv[], int argc);

void newCd(char *argv[],int argc);
void statusCd(char *argv[],int argc);
void updateCds(char *argv[],int argc);
void lsCd(char *argv[],int argc);

void candidatureCmd(char *argv[],int argc);