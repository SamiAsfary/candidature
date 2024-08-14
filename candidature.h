#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include  "string.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define CMD_NB 4

typedef uint8_t (* functionCmd_t)(char * argv[], int argc);

void newCd(char *argv[],int argc);
void statusCd(char *argv[],int argc);
void updateCd(char *argv[],int argc);
void lsCd(char *argv[],int argc);

void candidatureCmd(char *argv[],int argc);