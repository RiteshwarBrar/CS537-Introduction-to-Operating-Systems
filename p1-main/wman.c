#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
	if(argc == 1){
		printf("What manual page do you want?\nFor example, try 'wman wman'\n");
		exit(0);
	}

	else if(argc == 2){
		char p[16] = "./man_pages/man";
		char line[256];
		char path[256];
		for(int i=1; i<=9; i++){
		       	sprintf(path,"%s%d/%s.%d", p, i, argv[1], i);
			FILE *fp = fopen(path,"r");
			if(errno==2){
				printf("No manual entry for %s\n",argv[1]);
				exit(0);
			}
			else if(fp == NULL){
				printf("cannot open file\n");
				exit(1);
			}
			else if(fp != NULL){
				while(fgets(line, sizeof(line), fp)){
					printf("%s",line);
				}
				fclose(fp);
			exit(0);	
			}
			fclose(fp);	
		}	
	}

	else if(argc >= 3){

		int num = atoi(argv[1]);
		
		if(isdigit(num)==0 && strlen(argv[1])==1 && num>=1 && num<=9){
			char p[16] = "./man_pages/man";
			char path[256];
		       	sprintf(path,"%s%d/%s.%d", p, num, argv[2], num); 
		       	FILE *fp = fopen(path,"r");
			if(errno==2){
                                printf("No manual entry for %s in section %d\n",argv[2],num);
                                exit(0);
                        }
			else if(fp == NULL && errno != 2){
				printf("cannot open file\n");
                                exit(1);
			}
			else if(fp != NULL){
                                char line[256];
                                while(fgets(line, sizeof(line), fp)){   
                                        printf("%s",line);
                                }
			fclose(fp);	
                        exit(0);
                        }
			fclose(fp);
		}
		else{
			printf("invalid section\n");
			exit(1);
		}

	}
	return(1);
}
