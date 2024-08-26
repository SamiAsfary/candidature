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

#define CMD_NB 4
#define JSON_PERM (S_IRWXU|S_IRWXG|S_IRWXO)

typedef void (* functionCmd_t)(char * argv[], int argc);

void newCd(char *argv[],int argc);
void statusCd(char *argv[],int argc);
void updateCd(char *argv[],int argc);
void lsCd(char *argv[],int argc);

void candidatureCmd(char *argv[],int argc);