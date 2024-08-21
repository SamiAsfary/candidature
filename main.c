#include "main.h"
#include "candidature.h"

int cinstr(char * str, char separator){
	int loop = 0, total = 0;	
	while(str[loop] != '\0'){
		if(str[loop] == separator){total++;}
		loop++;
	}
	return total;
}


int main(){
	const char *welcome = "$ ./enseash\nBienvenue dans le Shell ENSEA.\nPour quitter, tapez 'exit'.";
	char newline[50];
	sprintf(newline, "\n enseash %% ");
	char buff[BUFFER_SIZE];
	char *token;
	int pid, argc = 0, loop = 0;
	ssize_t read_len;
	int returnStatus;
	struct timespec start;
	struct timespec end;
	long delay;
	char **argv;
	char * separator = " ";
	int saved_stdout = dup(STDOUT_FILENO), saved_stdin = dup(STDIN_FILENO);
	
	
	write(STDOUT_FILENO, welcome ,strlen(welcome));
	write(STDOUT_FILENO, newline ,strlen(newline));
	

	while(1){
		read_len = read(STDIN_FILENO,buff,BUFFER_SIZE);
		
		//test pour Ctrl+D et exit
		if(strncmp(buff,"exit",strlen("exit")) == 0  || read_len == 0){
			write(STDOUT_FILENO, "\nKTHXBYE\n", strlen("\nKTHXBYE\n"));
			return 0;
		}
		
		//ajout du caractère de fin de chaine de caractère
		buff[read_len-1] = '\0';
		
		
		//allocation de la memoire a la commande
		argc = (cinstr(buff,' ')+1);
		argv = malloc(argc*sizeof(char*));
		char * token = strtok ( buff, separator );
		while ( token != NULL ) {
			argv[loop] = malloc(sizeof(token));
			strcpy(argv[loop], token);
			loop++;
			token = strtok ( NULL, separator );
		}
		//argcbis = loop;
		argv[loop] = NULL;
		
		clock_gettime(CLOCK_MONOTONIC, &start);
		
		pid = fork();
		if(pid == 0){
			
			//analyse de la commande et redirection de la sortie ou de l'entre
			for(loop = 0; argv[loop] != NULL; loop++){
				if(strncmp(argv[loop], "<", strlen("<")) == 0){
					FILE* file = fopen(argv[loop+1],"r");
					dup2(fileno(file),STDIN_FILENO);
					fclose(file);
					
					//rends tous les argument à partir de < NULL
					for(int k=loop;k<argc;k++){
						argv[k]=NULL;
					}
                    argc = loop;
					
				}else if(strncmp(argv[loop], ">", strlen(">")) == 0){
					FILE* file = fopen(argv[loop+1],"w");
					dup2(fileno(file),STDOUT_FILENO);
					fclose(file);
					
					//rends tous les argument à partir de > NULL
					for(int k=loop;k<argc;k++){
						argv[k]=NULL;
					}
                    argc = loop;
				}
				
			}
			candidatureCmd(argv,argc);
			execvp(argv[0], argv);
			exit(EXIT_FAILURE);
		}
		
		//attente de la fin du processus fils et recuperation du status de sortie du processus fils
		wait(&returnStatus);
		
		dup2(saved_stdout, STDOUT_FILENO);
		dup2(saved_stdin, STDIN_FILENO);
		
		//recuperation du temps et calcul de temps d'éxecution de la commande
		clock_gettime(CLOCK_MONOTONIC, &end);
		delay = (end.tv_nsec - start.tv_nsec)/1000000 + ((long)((end.tv_sec-start.tv_sec)*1000));
		
		//liberation de la mèmoire allouer a la commande
		
		for(loop = argc; loop > 0; loop--){
			argv[loop-1]=NULL;
			free(argv[loop-1]);
		}
		free(argv);
		
		//affichage du status de sortie du processus fils
		if(WIFEXITED(returnStatus)){
			if(returnStatus != 0){
			printf("Value of standard error : %d\n", errno);
            perror("Error ");
			}
			sprintf(newline, "enseash [exit:%d|%ldms] %% ",WEXITSTATUS(returnStatus),delay);
		}else if(WIFSIGNALED(returnStatus)){
			sprintf(newline, "enseash [sign:%d|%ldms] %% ",WTERMSIG(returnStatus),delay);
		}
		
		
		write(STDOUT_FILENO, newline ,strlen(newline));
	}
	*token = *token;
	pid = 1;
	return 0;
}




//#include "candidature.h"
/*#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "main.h"

int main(){
    char buff[BUFF_SIZE];
    unsigned char read_len=0;
    printf("Hello World!");
    
    while(1){

        scanf("%s",buff);
        //read_len = read(STDIN_FILENO,buff,BUFF_SIZE);
        printf("2");
        //newCd(buff);
        read_len = read_len;
    }
    return 0;//
}*/