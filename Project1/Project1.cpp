#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "time_functions.h"
using namespace std;
void charByChar();
void lineByLine();
char inputFile[100];
char outputFile[100];
int main(int argc, char *argv[])
{
        int mode;
        sscanf(argv[1], "%d", &mode);
        if(mode == 0){
                printf("Please enter the input file name\n");
                scanf("%s",inputFile);
                printf("Please enter the output file name\n");
                scanf("%s", outputFile);
                charByChar();
                printf("the time from 'ftime' for Char-by-Char is %10.3f seconds \n ",get_wall_clock_diff());
                printf("the time from 'getCPUtime' for Char-by-Char is %10.3f seconds\n",get_CPU_time_diff());  // good for seconds of wall clock t$
                printf("\n\n\n");
        }
	else if(mode == 1){
                printf("Please enter the input file name\n");
                scanf("%s",inputFile);
                printf("Please enter the output file name\n");
                scanf("%s", outputFile);
                lineByLine();
                printf("the time from 'ftime' for Line-by-Line is %10.3f seconds \n ",get_wall_clock_diff());
                printf("the time from 'getCPUtime' for Line-by-Line is %10.3f seconds\n",get_CPU_time_diff());  // good for seconds of wall clock t$
                printf("\n\n\n");
        }
	else{
             	printf("you enter wrong number please enter 0 for char by char enter 1 for line by line");
        }
	return 0;
}
void charByChar(){
        FILE *file = fopen(inputFile, "r");
        FILE *outFile = fopen(outputFile, "w");
        int c;
              	if (file == NULL)
                        printf("Unable to open that file\n");
                else{
                     	start_timing();
                        while((c = fgetc(file)) != EOF){ /* read from file char by char*/
                                fputc(c,outFile);
}
                        fclose(file);
                        fclose(outFile);
                        stop_timing();
                }
}
void lineByLine(){
        FILE *file = fopen(inputFile, "r");
        FILE *outFile = fopen(outputFile, "w");
        if(file == NULL){
                        printf("Unable to open that file\n");
                }
                else{
                        char line [80]; /* or other suitable maximum line size */
                        start_timing();
                         while (fgets (line,sizeof line,file ) != NULL ){ /* read a line */
                                 fputs(line, outFile);
                         }
                         fclose(file);
                         fclose(outFile);
                         stop_timing();
                }
}