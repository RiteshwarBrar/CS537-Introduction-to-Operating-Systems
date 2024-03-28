#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
//#include "wsh.h"
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>

struct job {
  int pid;
  int inuse;
  int bg;
  char *args[MAX_ARGS];
  int nargs;
};

struct job jobs[MAX_JOBS];


int
main(int argc, char *argv[]){
	char *in,*toke;
	size_t i_size = 256;
	int i;
	int arrlen;
	char *args[256];
	//char cmd[256]="";
	int stat;
	int pid;
	signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGINT, SIG_IGN);
	in = (char *)malloc(i_size*sizeof(char));
	//
	if(argc<1 && argc>2){
		return 0;
	}
	else if(argc==1){
		while(1){
			printf("wsh> ");
			i=getline(&in, &i_size, stdin);
			//printf("you typed: %s", in);
			
			if(strcmp(in,"exit\n")==0 || i==EOF){
				exit(0);
			}
			i=0;
			while((toke = strsep(&in," "))!=NULL){
				//if(i==0){
				//	strncpy(cmd,toke,strlen(toke)+1);
				//	i+=1;
				//}
				//else{
					args[i]=toke;
					i+=1;//sprintf(args,"%s %s",args,toke);
				//}
			}
			args[i]=NULL;
			arrlen=i;
			for(int k=0;k<strlen(args[i-1]);k++){
				if(args[i-1][k]=='\n'){
					args[i-1][k]='\0';
				}
			}
			i=0;                          //we have the args in array format
			//printf("Command: %s",cmd);
			//printf("Arr: \n");
			//while(args[i]!=NULL){
				//printf("%s\n",args[i]);
				//i++;
			//}
	//built-in commands
			if(strcmp(args[0],"cd")==0){
				if(arrlen!=2){
					printf("arglen cd failed:");
					exit(0);//more than one args
				}
				if(chdir(args[1])!=0){
					printf("chdir failed:");
					exit(0);//error changing directory
				}
			}
			
			else if((pid = fork())==0){
				signal (SIGINT, SIG_DFL);
				signal (SIGTSTP, SIG_DFL);
				if(execvp(args[0],args)==-1){
					printf("execfail\n");
				}
				setpgid(pid,pid);
				exit(0);
			}
			else{
				setpgid(pid,pid);
				tcsetpgrp(STDIN_FILENO, pid);//fg
				waitpid(pid, &stat, WUNTRACED);
				tcsetpgrp(STDIN_FILENO, getpid());
			}
			
			
		}
	}
	//printf(1,"XV6_TEST_OUTPUT Last catted filename: %s\n", out);
	exit(1);
}
