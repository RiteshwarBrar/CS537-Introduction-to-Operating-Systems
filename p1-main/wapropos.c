#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
	if(argc == 1){
		printf("wapropos what?\n");
		exit(0);
	}

	else if(argc >= 2){
		char p[16] = "./man_pages/man";
		char line[256];
		char path[1024];
		char dir[18];
		char filename[256]=" ";
		char section[256]=" ";
		char name[256]= " ";
		char disc[256]= " ";
		int flag=0;
		
		for(int i=1; i<=9; i++){
			sprintf(dir,"%s%d", p, i);
			
			DIR *directory;
			directory = opendir(dir);
			//printf("%s\n",dir);
			struct dirent *entry;
			
			if(directory){
				//printf("1\n");
				while((entry = readdir(directory)) != NULL){
					//filename = file->d_name;
					if(entry->d_type == DT_REG){
						
						strcpy(filename,entry->d_name);
						
						char *dot = strrchr(filename, '.');    
		                                if (dot != NULL) {
		                                	*dot = '\0';
		                                        //section = dot + 1;
		                                        strcpy(section, dot+1);
		                                      	strcpy(name,filename);
						}

						sprintf(path,"%s/%s.%s", dir, filename, section);
						
						char m_name[256]=" ";
		                		FILE *fp = fopen(path,"r");
		                		
		                		if(fp == NULL){
		                        		printf("cannot open file\n");
		                        		exit(1);
		                		}
		                		//printf("%s\n",path);
		                		
		                		while(fgets(line, sizeof(line), fp)){
		                			//printf("%s\n",line);
		                                	//1
		                                	if(strncmp(line,"\033[1mNAME\033[0m\n",256)==0){
		                                		char l[256];
								while(fgets(l,sizeof(l),fp) != NULL){
									if(l[0] == '\n'){
										break;
									}
									else if(strstr(l,argv[1])){
										flag=1;
									}
									strcpy(m_name,l);
									char *dash = strrchr(m_name, '-');    
						                	if (dash != NULL) {
						                		*dash = '\0';
						                        	strcpy(disc,dash + 1);
									}
								}
								
		                                	}
		                                	if(strncmp(line,"\033[1mDESCRIPTION\033[0m\n",256)==0){
		                                		char l[256];
								while(fgets(l,sizeof(l),fp) != NULL){
									if(l[0] == '\n'){
										break;
									}
									else if(strstr(l,argv[1])){
										flag=1;
									}
								}
								
		                                	}
	    							
		                        	}//
		                        	fclose(fp);
						if(flag==1){
							
							printf("%s (%s) -%s", name, section, disc);
							flag=2;
					
						}
		                		
                        		}
                        		       
                        			
				}
				//printf("%d\n",errno);
				closedir(directory);
			}
			else{
				printf("Failed to open directory\n");
				exit(1);
			}
			
		}
		if(flag==0){
		        	printf("nothing appropriate\n");
		                exit(0);
		        }
		 else{
		 	exit(0);
		 }	
	}
	return(1);
}
