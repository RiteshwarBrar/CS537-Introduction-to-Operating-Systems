#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
//#include "wsh.h"
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>


int jid=0;


struct job {
  int pid,fpid;
  int inuse;
  int bg;
  int nargs;
  int id;
  int pno;
  char *args[50];
};

struct job jobs[50];
//////////////freeeeeeeeeeee
void rem_jobs(struct job *job) {
	for(int i=0;i<job->nargs; i++){
		job->args[i]=0;
	}
	job->inuse=0;
	job->bg=0;	
	memset(job,0,sizeof(*job));
}

void print_job(struct job *job){
	printf("%d: ",(job->id));
	for(int i=0;i<job->nargs; i++){
		if(i==(job->nargs)-1)
			printf("%s",job->args[i]);
		else
			printf("%s ",job->args[i]);
	}
	if(job->bg==1){
		printf(" &");
	}
	printf("\n");
}

void print_jobs(){/////
	for(int x=0;x<50;x++){
		if((&jobs[x])->inuse==0)
			continue;
		print_job(&jobs[x]);
	}
	//print_job(&jobs[0]);
}

void sc_handler(int sig) {
  int pid, status;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
      for (int i = 0; i < 50; i++)
        if (jobs[i].pid == pid)
          rem_jobs(&jobs[i]);
    }
}

int
main(int argc, char *argv[]){
	char *in,*toke;
	size_t i_size = 256;
	int i,read,pipes;
	int arrlen;
	char *args[256];
	char *pargs[256];
	//char pipe[256]="";
	int stat;
	int pid,fpid;
	int bg;
	FILE * fp;
	memset(jobs, 0, sizeof(jobs));
	
	signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGINT, SIG_IGN);
        signal(SIGCHLD, sc_handler);
        
	in = (char *)malloc(i_size*sizeof(char));
	//
	if(argc<1 && argc>2){
		exit(-1);
	}
	if(argc==2){
		if((fp = fopen(argv[1], "r")) == NULL){
		      	printf("cannot open %s\n", argv[1]);
			exit(-1);
		}
		
		
	}
		while(1){
			
			for(int x=0;x<50;x++){
				if((&jobs[x])->inuse==0){
					continue;
				}
				if(kill((&jobs[x])->pid,0)!=0){
					//printf("BYE: %d: %s\n",(&jobs[x])->id, (&jobs[x])->args[0]);
					rem_jobs(&jobs[x]);
				}
			}
			if(argc==1){
				printf("wsh> ");
				i=getline(&in, &i_size, stdin);
				//printf("you typed: %s", in);
			}
			else{
			//file
				if((read = getline(&in, &i_size, fp)) == -1)
					break;
			}
			if(strcmp(in,"exit\n")==0 || i==EOF){
				exit(0);
			}
			//tokenize the input
			if(in[strlen(in)-1]=='\n'){
					in[strlen(in)-1]='\0';
			}
			if(in[strlen(in)-1]=='&'){
				bg=1;
				in[strlen(in)-2]='\0';
			}
			else{
				bg=0;
			}
			i=0;
			pipes=0;
			
			//tokenize
			while((toke = strsep(&in," "))!=NULL){
				//if(i==0){
				//	strncpy(cmd,toke,strlen(toke)+1);
				//	i+=1;
				//}
				if(strcmp(toke,"|")==0)
					pipes+=1;
				
				args[i]=toke;
				i+=1;//
				//printf("%s %d",toke, pipes);
				//}
			}
			args[i]=NULL;
			arrlen=i;
			i=0;
			//for(int k=0;k<strlen(args[i-1]);k++){
			//	if(args[i-1][k]=='\n'){
			//		args[i-1][k]='\0';
			//	}
			//	
			//}
			//if(strcmp(args[i-1],"&")==0){
			//	bg=1;
			//	args[i-1]="\0";
			//}
			//else{
			//	bg=0;
			//}
			//we have the args in array format
			//printf("Command: %s",cmd);
			//printf("Arr: \n");
			//while(args[i]!=NULL){
				//printf("%s\n",args[i]);
				//i++;
			//}
			
			
	//built-in commands////////////////////////////////////////////////////////
			
			//CD
			if(strcmp(args[0],"cd")==0){
				if(arrlen!=2){
					printf("arglen cd failed:");
					exit(0);//more than one args
				}
				if(chdir(args[1])!=0){
					printf("chdir failed:");
					exit(0);//error changing directory
				}
				continue;
			}
			
			//JOBS
			else if(strcmp(args[0],"jobs")==0){///
				print_jobs();
				continue;
			}
			
			//FG
			else if(strcmp(args[0],"fg")==0){
				if(arrlen==1){
					for(int k=49;k>=0;k--){
						if((&jobs[k])->inuse == 1){
							tcsetpgrp(STDIN_FILENO, getpgid((&jobs[k])->pid));
							kill((&jobs[k])->pid, SIGCONT);
							if(waitpid((&jobs[k])->pid, &stat, WUNTRACED)>0)
								if (WIFEXITED(stat) || WIFSIGNALED(stat))
					    				rem_jobs(&jobs[k]);
					    		tcsetpgrp(STDIN_FILENO, getpid());
							break;
						}
					}
				}
				else if(arrlen==2){
					for(int k=0;k<50;k++){
						if((&jobs[k])->inuse == 1 && (&jobs[k])->id == atoi(args[1])){
							tcsetpgrp(STDIN_FILENO, getpgid((&jobs[k])->pid));
							kill((&jobs[k])->pid, SIGCONT);
							if(waitpid((&jobs[k])->pid, &stat, WUNTRACED)>0)
								if (WIFEXITED(stat) || WIFSIGNALED(stat))
					    				rem_jobs(&jobs[k]);
					    		tcsetpgrp(STDIN_FILENO, getpid());
							break;
						}
					}
				}
				else
					exit(-1);
				continue;
			}
			else if(strcmp(args[0],"bg")==0){
				if(arrlen==1){
					for(int k=49;k>=0;k--){
						if((&jobs[k])->inuse == 1){
							tcsetpgrp(STDIN_FILENO, getpgid((&jobs[k])->pid));
							kill((&jobs[k])->pid, SIGCONT);
							if(waitpid((&jobs[k])->pid, &stat, WNOHANG)>0)
								if (WIFEXITED(stat) || WIFSIGNALED(stat))
					    				rem_jobs(&jobs[k]);
					    		tcsetpgrp(STDIN_FILENO, getpid());
							break;
						}
					}
				}
				else if(arrlen==2){
					for(int k=0;k<50;k++){
						if((&jobs[k])->inuse == 1 && (&jobs[k])->id == atoi(args[1])){
							tcsetpgrp(STDIN_FILENO, getpgid((&jobs[k])->pid));
							kill((&jobs[k])->pid, SIGCONT);
							if(waitpid((&jobs[k])->pid, &stat, WNOHANG)>0)
								if (WIFEXITED(stat) || WIFSIGNALED(stat))
					    				rem_jobs(&jobs[k]);
					    		tcsetpgrp(STDIN_FILENO, getpid());
							break;
						}
					}
				}
				else
					exit(-1);
				continue;
			}
			/////////fork
			//ppid=getpid();
			if(pipes>0){
				
				//printf("BB%d \n",pipes);
				//printf("%s\n",args[2]);
				//printf("s ");
					int numc=pipes+1;
					int fd[numc][2];
					int i=0;
					int j=0;
					int pin=0;
					//printf("BB%d \n",numc);
					for (pin = 0; pin < numc; pin++) {
						pipe(fd[pin]);
					}
					int fptr=0;
					int fpd[50];
					for(int g=0;g<numc;g++){
					
						//printf("%d\n",arrlen);
						//printf("%d\n",i);
						while(i<arrlen && strcmp(args[i],"|")!=0){
							pargs[j]=args[i];
							//printf("%s\n",pargs[j]);
							//printf("%d\n",i);
							i+=1;
							j+=1;
						}
						
						pargs[j]=NULL;
						
						//printf("%d\n",g);
						
						j=0;//reset index of pargs
						i+=1;//past the slash '|'
						
						//printf("%s\n",pargs[j]);
						fpid=fork();
						if(fpid==0){
							
							signal (SIGINT, SIG_DFL);
							signal (SIGTSTP, SIG_DFL);
							
							if(g>0){
								dup2(fd[g-1][0],STDIN_FILENO);
							}
							if(g+1<numc){
								dup2(fd[g][1],STDOUT_FILENO);
							}
							
							int w = 0;
							
						    for (w = 0; w < numc; w++) {
							close(fd[w][0]);
							close(fd[w][1]);
						    }
							execvp(pargs[0],pargs);
							//exit(-1);
						}
						else{
								
							//printf("%s\n",pargs[0]);
							setpgid(fpid,pid);
							//printf("gc pid: %d\n",fpid);
							//tcsetpgrp(fd[g][0], fpid);//
							fpd[fptr]=fpid;
							fptr+=1;
							
							for(int z=0;z<fptr;z++){//
								//printf("gc pid: %d\n",fpid);
								//printf("above: %s\n",pargs[0]);
								//waitpid(fpd[z], &stat,0);
								if(fpd[z]!=-1 && waitpid(fpd[z], &stat, bg==1?WNOHANG:WUNTRACED)>0) //
									if (WIFEXITED(stat) || WIFSIGNALED(stat))
						    				fpd[z]=-1;
								//printf("below\n");//
							}//
							//tcsetpgrp(fd[g+1][0], pid);
										
						}
												
						//pipe-=1;
					}
					
					//kill(fpid, SIGINT);	
					
					for (int w = 0; w < numc; w++) {
						close(fd[w][0]);
						close(fd[w][1]);
					}
			}
			else{
				if((pid = fork())==0){
				
					pid=getpid();
					setpgid(pid,pid);
					
					//printf("%d   ",pipes);
					signal (SIGINT, SIG_DFL);
					signal (SIGTSTP, SIG_DFL);
						
					if(pipes==0){
						execvp(args[0],args);
						exit(-1);
					//printf("execfail\n");
					//remove job
					}
								
					//setpgid(pid,pid);
					exit(0);//////////
				}
				else{
					for(int x=0;x<50;x++){
						if((&jobs[x])->inuse==0){
							jid=x;
							break;
						}
						
					}
					struct job *job = &jobs[jid];
					jid = jid+1;
					job->pid = pid;
					job->inuse=1; 
					job-> bg=bg;
					job-> nargs = arrlen;
					job->id = jid;
					job->pno=pipes;
					
					for(int k=0;k<arrlen;k++){
						job->args[k]=args[k];
					}
									
					//print_jobs();//
				
					//if(bg == 1){///
					//	tcsetpgrp(STDIN_FILENO, getpid());
					//}
					//else{
					//	tcsetpgrp(STDIN_FILENO, pid);//fg
					//	waitpid(pid, &stat, WUNTRACED);
					//	tcsetpgrp(STDIN_FILENO, getpid());
					//}  
					//setpgid(pid,pid);
					//
					setpgid(pid,pid);
					
					tcsetpgrp(STDIN_FILENO, pid);
					//if(bg == 1){
						//bg
					if(waitpid(-1, &stat, bg==1?WNOHANG:WUNTRACED)>0) //
						if (WIFEXITED(stat) || WIFSIGNALED(stat))
			    				rem_jobs(&jobs[jid-1]);
						
					tcsetpgrp(STDIN_FILENO, getpid());
					
					
					
					
				}
			}	
		}
		fclose(fp);
	
	
	exit(1);
}
