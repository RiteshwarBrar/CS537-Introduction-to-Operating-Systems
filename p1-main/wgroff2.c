#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <regex.h>

void convertFormattingMarks(char *line) {

    struct {
        char *code;
        char *sequence;
    } formatting[] = {
        {"/fB", "\033[1m"},
        {"/fI", "\033[3m"},
        {"/fU", "\033[4m"},
        {"/fP", "\033[0m"},
        {"//", "/"}
    };
    size_t numRep = sizeof(formatting) / sizeof(formatting[0]);
    for(size_t i = 0; i < numRep; i++){
        char *search = formatting[i].code;
        char *replace = formatting[i].sequence;
        char *pos;

        while((pos = strstr(line, search))){
            size_t searchLen = strlen(search);
            size_t replaceLen = strlen(replace);

            memmove(pos + replaceLen, pos + searchLen, strlen(pos + searchLen) + 1);
            memcpy(pos, replace, replaceLen);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Improper number of arguments\nUsage: ./wgroff <file>\n");
        exit(0);
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        printf("File doesn't exist\n");
        exit(0);
    }

    char command[255];
    char sectionNum[255];
    char date[255];
    char line[255];
    char outputFileName[255];
    FILE *outputFile = NULL;
    int lineNumber = 0;
    int section;

    while (fgets(line, sizeof(line), inputFile) != NULL) {
        lineNumber++;

        // Ignore comments
        if (line[0] == '#') {
            continue;
        }

        if (lineNumber == 1) {
            if (sscanf(line, ".TH %s %s %s", command, sectionNum, date) != 3) {
                printf("Improper formatting on line 1\n");
                exit(0);
            }else{

                // Check for valid section number
                for(int i = 0; sectionNum[i] != '\0'; i++){
                    if(!isdigit(sectionNum[i])){
                        printf("Improper formatting on line 1\n");
                        exit(0);
                    }
                    if(sectionNum[i] == '.'){
                        printf("Improper formatting on line 1\n");
                        exit(0);
                    }else{
                        section = atoi(sectionNum);
                    }
                    if(section < 1 || section > 9){
                        printf("Improper formatting on line 1\n");
                        exit(0);
                    }
                }
                
                // Check for valid date
                if(date != NULL){
                    int count = 0;
                    for(int i = 0; date[i] != '\0'; i++){
                        if(date[i] == '-'){
                            count++;
                        }
                    } 
                    if(count != 2){
                        printf("Improper formatting on line 1\n");
                        exit(0);
                    }
                }else{
                    printf("Improper formatting on line 1\n");
                    exit(0);
                }

                if (strlen(date) != 10) {
                    printf("Improper formatting on line 1\n");
                    exit(0);
                }

                char *pattern = "^[0-9]{4}-[0-9]{2}-[0-9]{2}$";
                regex_t regex;
                int result = regcomp(&regex, pattern, REG_EXTENDED);

                if(result != 0){
                    exit(0);
                }

                result = regexec(&regex, date, 0, NULL, 0);

                if(result != 0){
                    printf("Improper formatting on line 1\n");
                    exit(0);
                }

                regfree(&regex);

                // Naming the output file
                strcpy(outputFileName, command);
                strcat(outputFileName, ".");
                strcat(outputFileName, sectionNum);
            }

            outputFile = fopen(outputFileName, "w");
            if (outputFile == NULL) {
                printf("cannot open file\n");
                exit(0);
            }

            int numSpaces = 80 - (2 * (strlen(command) + strlen(sectionNum) + 2));
            // Format the first line
            fprintf(outputFile, "%s(%s)", command, sectionNum);
            fprintf(outputFile, "%*c", numSpaces, ' ');
            fprintf(outputFile, "%s(%s)\n", command, sectionNum);
            continue;
        }
        // Handle .SH (section header) line
        if (strncmp(line, ".SH ", 4) == 0) {

            fprintf(outputFile, "\n");

            char sectionName[256];
            strncpy(sectionName, line + 4, sizeof(sectionName) - 1);
            sectionName[sizeof(sectionName) - 1] = '\0';

            if(strlen(sectionName) == 0 || sectionName[0] == '\n'){
                printf("Improper formatting on line %d\n", lineNumber);
                exit(0);
            }

            // Convert section name to uppercase and apply bold formatting
            for (int i = 0; sectionName[i]; i++) {
                sectionName[i] = toupper(sectionName[i]);
            }
            int length = strlen(sectionName);
            sectionName[length-1] = '\0';
            fprintf(outputFile, "\033[1m%s\033[0m\n", sectionName);
            continue;
        }
        else{
            convertFormattingMarks(line);
            fprintf(outputFile, "       %s", line);
        }
    }

    if(outputFile){
        fprintf(outputFile, "%*s%s", (int)(40 - strlen(date) / 2), "", date);
        fprintf(outputFile, "%*c\n", 35, ' ');
        fclose(outputFile);
    }
    fclose(inputFile);
    return 0;
}
