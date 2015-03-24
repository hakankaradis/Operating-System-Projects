#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef linux
#include <inttypes.h>
#include <unistd.h>
#endif
struct thread_arg{
  int first;
  int second;
};

void ReadAndFill();
void* MULTIPLY(void *arguments);
int ROW1=0;
int ROW2=0;
int COLUMN1=0;
int COLUMN2=0;
using namespace std;
int **matrix1 = 0;
int **matrix2 = 0;
int **resultMatrix = 0;
char* fileName;

int main(int argc, char *argv[]){
  ofstream outfile;
  outfile.open("output.txt");
  fileName = (char *)argv[1];  // take file name from user
  ReadAndFill(); // call ReadAndFill function to fill first and second matrices by reading and spliting line by line
  struct thread_arg args; // create struct to send first and second elements from first and second matrices
  cout << "first matrix" << endl;
  outfile << "first matrix" << endl;
  for(int i=0; i<ROW1; i++ ){   // print out first matrix
    for(int y=0; y<COLUMN1; y++){
      cout << matrix1[i][y] << " ";
	  outfile << matrix1[i][y] << " ";
    }
    cout << endl;
    outfile << endl;
  }
  cout << "second matrix" << endl;
  outfile << "second matrix" << endl;
  for(int i=0; i<ROW2; i++ ){  // print out second matrix
    for(int y=0; y<COLUMN2; y++){
      cout << matrix2[i][y] << " ";
      outfile << matrix2[i][y] << " ";
    }
    cout << endl;
    outfile << endl;
  }
  cout <<"-----" << endl;
  resultMatrix = new int *[ROW1]; // create result matrix that holds the result of multiplied of 2 matrices
  for (int x = 0; x < ROW1; x++){
    resultMatrix[x] = new int[COLUMN2]; // allocate space
  }
   uintptr_t multiplied=0; // holds the multiplication of the element
  int result =0;  // holds the multiplaction of row and column
  int indexForThreads = 0;
  pthread_t *threads = new pthread_t[ROW1*ROW2*COLUMN2];  // allocate spaces for threads
  for (int i=0; i<ROW1 ; i++){
    for (int y=0; y<COLUMN2; y++){
      for (int a=0; a<ROW2; a++){
        args.first = matrix1[i][a];  // takes element from first matrix
        args.second = matrix2[a][y]; // takes element from second matrix
        pthread_create(&threads[indexForThreads], NULL, MULTIPLY, (void *)&args); // create thread and call MULTIPLY function with 2 arguments
        indexForThreads++;
        #ifdef _WIN32
          Sleep(100);
        #endif
	#ifdef linux
          usleep(100);
        #endif
      }
    }
  }
  indexForThreads=0;
  for (int i=0; i<ROW1 ; i++){
    for (int y=0; y<COLUMN2; y++){
      for (int a=0; a<ROW2; a++){
        pthread_join(threads[indexForThreads], (void**)&multiplied);  // join threads and hold the returned value on multiplied variable
        indexForThreads++;
        result += multiplied;
      }
      resultMatrix[i][y] = result;
      result = 0;
    }
  }
  cout<< "--------- RESULT ------" << endl;
  outfile << "--------- RESULT ------" << endl;
  for(int i=0; i<ROW1; i++ ){  // print out the result matrix that holds the result of multiplied 2 matrices
    for(int y=0; y<COLUMN2; y++){
      cout << resultMatrix[i][y] << " ";
      outfile << resultMatrix[i][y] << " ";
    }
    cout << endl;
    outfile << endl;
  }

  outfile.close();
  return 0;
}

void *MULTIPLY(void *arguments){
  struct thread_arg *args = (struct thread_arg *)arguments;
  uintptr_t total = args->first * args->second;  // takes the variables from struct and multiply them

  return (void *)total;  // return the multiplied result
}
void ReadAndFill(){
  int indexForNumbers = 0;
  int row=0;  // holds the number of rows
  int column=0; // holds the number of columns
  string line = "";
  int flag = 0;
  ifstream file;
  char numbers[10];
  file.open(fileName);
  if(!file.is_open()){
    printf("Unable to open that file\n");
    //return 0;
  }
  else{
    while (!file.eof()){ //  go to end of file
      getline(file,line);   // get line from inout file
      int i;
      if(line[0] != '*' && line[0] !='\0'){
        for(i=0; i<line.length(); i++){
          if(line[i] != ' '){  // eleminate the whitespaces
            numbers[indexForNumbers] = line[i];
            indexForNumbers ++;
          }
          if(line[i] == ' ' || i == line.length()-1){
            if ((numbers[0]) != '\0') {
              column++;  // determine how many columns we have
              fill(numbers, numbers+10, 0);
              indexForNumbers=0;
			}
          }
		}
		row++;  // determines how many rows we have
      }
      if(line[0] == '*' && line[0] !='\0'){
        ROW1 = row;
        COLUMN1 = column/row;
        matrix1 = new int *[row]; // allocate spaces for first matrix
        for (int x = 0; x < row; x++){
          matrix1[x] = new int[COLUMN1];
        }
	    row = 0;
        column = 0;
        flag = 0;                       
		}
      }
    }
    ROW2 = row;
    COLUMN2 = column/row;
    matrix2 = new int *[row];  // allocate spaces for second matrix
    for (int x = 0; x < row; x++){
      matrix2[x] = new int[COLUMN2];
	}
	    file.close(); // close file and
	    row = 0;
	    column = 0;
	    file.open(fileName); // open it again
	    if(!file.is_open()){
	      printf("Unable to open that file\n");
	      //return 0;
	    }
	    else{
	      while (!file.eof()){
	      getline(file,line);   // get line from inout file
	      int i;
	      if(line[0] != '*' && line[0] !='\0'){
	        for(i=0; i<line.length(); i++){
	          if(line[i] != ' '){
	            numbers[indexForNumbers] = line[i];
	            indexForNumbers ++;
	          }
	          if(line[i] == ' ' || i == line.length()-1){
	            if ((numbers[0]) != '\0') {
	              if(flag == 0){
	 				matrix1[row][column] = atoi(numbers); // fill the matrix1
	              }
	              else{
	                matrix2[row][column] = atoi(numbers); // fill the matrix2
	              }
	              column++;
	              fill(numbers, numbers+10, 0);
	              indexForNumbers=0;
	            }
	          }
		}
		row++;
		column =0;
	      }
	      if(line[0] == '*' && line[0] !='\0'){
	        row = 0;
	        column = 0;
	        flag = 1;
	      }
	    }
	  }
	}

