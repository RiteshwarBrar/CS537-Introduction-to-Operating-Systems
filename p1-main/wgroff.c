#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char * argv[]){
	if(argc!=2){
		printf("Improper number of arguments\nUsage: ./wgroff <file>\n");
		exit(0);
	}
	
	char line[256];
	int lptr=0;//current line number
	char cmd[256];
	char section[256];
	char date[256];
	int snum=0;
	char outfile[512];
	FILE *ofp = NULL;
	FILE *fp = fopen(argv[1],"r");
	
	if(fp == NULL){
		printf("File doesn't exist\n");
		exit(0);
	}
	
	while(fgets(line,sizeof(line),fp) != NULL){
		lptr=lptr+1;
		//printf("%d",lptr);
		if(line[0]!='#'){
		
			if(lptr==1){
				if(sscanf(line, ".TH %s %s %s", cmd, section, date)==3){
					snum = atoi(section);
					if(isdigit(snum)!=0 || strlen(section)!=1 || snum<1 || snum>9){
						printf("Improper formatting on line 1\n");
                        			exit(0);
					}
					//YYYY-MM-DD
					if(date==NULL ||strlen(date) != 10){
						printf("Improper formatting on line 1\n");
                        			exit(0);
					}
					else{
						for(int i=0;i<10;i++){
							if(i==4 || i==7){
								if(date[i]!='-'){
									printf("Improper formatting on line 1\n");
                        						exit(0);
								}
							}
							else{
								if(!isdigit(date[i])){
									printf("Improper formatting on line 1\n");
                        						exit(0);
								}
							}
						}
					}
					sprintf(outfile, "%s.%s", cmd,section);
				}
				else{
					printf("Improper formatting on line 1\n");
                			exit(0);
				}
				
				ofp = fopen(outfile,"w");
				if(ofp == NULL){
					printf("cannot open file\n");
					exit(0);
				}
				//printf("%d\n",lptr);
				fprintf(ofp, "%s(%s)%*c%s(%s)\n", cmd, section, 80-(int)(6+(2*strlen(cmd))), ' ', cmd, section);
				continue;
			}
			if(strncmp(line, ".SH ", 4) == 0){
				char header[256];
				strncpy(header, line + 4, sizeof(header));
				header[strlen(header)-1] = '\0';//removing the nextline character
				fprintf(ofp, "\n");
				
				if(strlen(header)!=0 && header[0]!='\n'){
					for(int i=0; header[i];i++){
						header[i] = toupper(header[i]);
					}
					
            				fprintf(ofp, "\033[1m%s\033[0m\n", header);
            				continue;
				}
				else{
					printf("Improper formatting on line %d\n", lptr);
                			exit(0);
				}
				
				
				
								
			}
			else{
				//rest
				struct format{
					char *old;
					char *new;
				};
				
				struct format f1 = {"/fB", "\033[1m"};
				struct format f2 = {"/fI", "\033[3m"};
				struct format f3 = {"/fU", "\033[4m"};
				struct format f4 = {"/fP", "\033[0m"};
				struct format f5 = {"//", "/"};
				
				struct format frmt[] = {f1, f2, f3, f4, f5};
				
				size_t numReplace = sizeof(frmt) / sizeof(frmt[0]);
				for(size_t i = 0; i < numReplace; i++){
					char *find = frmt[i].old;
					char *replace = frmt[i].new;
					char *pos;
					while((pos = strstr(line, find))){
					    size_t flen=strlen(find);
					    size_t rlen=strlen(replace);
					    memmove(pos+rlen, pos+flen, strlen(pos+flen)+1);
					    memcpy(pos, replace, rlen);
					}
    				}
				
				fprintf(ofp, "       %s", line);
			}
		}
	}
	
	if(ofp){
		fprintf(ofp, "%*c%s%*c\n", 35, ' ', date, 35, ' ');
		fclose(ofp);
	}		
	fclose(fp);
	return 0;
}
