#include <sys/wait.h>  /* Needed for the wait function */
#include <unistd.h>    /* needed for the fork function */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <inttypes.h>
#include <unistd.h>

struct thread_arg{
  int first;
  int second;
};

void ReadAndFill();
int MULTIPLY(int, int);
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

  if(argc != 2){
    cout << "Please enter the input file name" << endl;
    return 0;
  }

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

  int *sharedMatrix1;  // shared memory for matrix1
  int *sharedMatrix2;  // shared memory for matrix2

  int shmid;
  size_t shmsize =COLUMN1 *  ROW1 * sizeof(sharedMatrix1[0]); // get size of 2 dimensional matrix
  shmid = shmget(IPC_PRIVATE, shmsize, IPC_CREAT | 0666);  // create shared memory
  sharedMatrix1 = (int *)shmat(shmid, (void *)0, 0);
  int z=0;
  for(int i=0; i<ROW1; i++ ){   // print out first matrix
    for(int y=0; y<COLUMN1; y++){
      sharedMatrix1[z] = matrix1[i][y]; z++;
    }
  }
  int shmid2;
  size_t shmsize2 = COLUMN2 * ROW2 * sizeof(sharedMatrix2[0]); // get size of 2 dimensional matrix
  shmid2 = shmget(IPC_PRIVATE, shmsize2, IPC_CREAT | 0666);  // create shared memory
  sharedMatrix2 = (int *)shmat(shmid2, (void *)0, 0);
  z=0;
  for(int i=0; i<ROW2; i++ ){   // print out first matrix
    for(int y=0; y<COLUMN2; y++){
      sharedMatrix2[z] = matrix2[i][y]; z++;
    }
  }
  int multiplied = 0; // holds the multiplication of the element
  int *result;  // holds the multiplaction of row and column
  int countForNewBuffer=0;
  int *resultMatrix;
  int shmid4;
  size_t shmsize4 = ROW1 * COLUMN2 * sizeof(int); // get size of 2 dimensional matrix
  shmid4 = shmget(IPC_PRIVATE, shmsize4, IPC_CREAT | 0666);  // create shared memory
  resultMatrix = (int *)shmat(shmid4, (void *)0, 0);

  int shmid3;
  size_t shmsize3 =sizeof(int); // get size of 2 dimensional matrix
  shmid3 = shmget(IPC_PRIVATE, shmsize3, IPC_CREAT | 0666);  // create shared memory
  result = (int *)shmat(shmid3, (void *)0, 0);  // to hold multiplied each element
  result[0] = 0;
  int indexForProcess = 0;
  pid_t *pid = new pid_t[ROW1*COLUMN1*COLUMN2]; // allocate pid for each child process
  pid_t *cpid = new pid_t[ROW1*COLUMN1*COLUMN2];

  if(COLUMN1 == ROW2){

    for (int i=0; i<ROW1; i++){
      for (int j=0; j<COLUMN2; j++){
        for (int k=0; k<ROW2; k++){
          args.first = sharedMatrix1[(i*COLUMN1) + k];//k j  // takes element from first matrix
          args.second = sharedMatrix2[(k * COLUMN2) + j]; // takes element from second matrix
          if((pid[indexForProcess] = fork()) == 0){  // create child process and multiply the 2 shared matrices
            multiplied = MULTIPLY(args.first, args.second);
            result[0] += multiplied;
            _exit(EXIT_SUCCESS);
          }
          else{
            int status;
            wait(&status);
          }
          indexForProcess++;
        }
     	resultMatrix[countForNewBuffer] = result[0]; // hold the number in new shared result matrix
        countForNewBuffer++;
        result[0] =0;
      }
    }
// print out the result matrix
    cout << "Result Matrix " << endl;
    outfile << "Result Matrix " << endl;
    int a=0;
    for(int x=0; x< countForNewBuffer; x++){
      cout << resultMatrix[x] << " ";
      outfile << resultMatrix[x] << " ";
      a++;
      if(a == COLUMN2){
        cout << endl;
        outfile << endl;
        a=0;
      }
    }

  }
  else
    cout << "These matrices can not be multiplied" << endl;

  outfile.close();

  return 0;
}
int MULTIPLY(int first, int second){
  int total = first * second;  // takes the variables from struct and multiply them

  return total;  // return the multiplied result
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
        flag = 0;                       }
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
    file.open("input.txt"); // open it again
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

