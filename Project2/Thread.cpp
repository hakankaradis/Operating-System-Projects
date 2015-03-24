#include <iostream>
#include <pthread.h>
#include <fstream>
#include <string>
#include <semaphore.h>
#include "time_functions.h"

/* this code is written by Hakan KARADIS CS550-04 */

using namespace std;
void *readFile(void *);
void *readAndWrite(void *fileName);
string buffer[10];
static int count=0;
int in=0, out=0, n=10;
sem_t s;   // semaphore
sem_t n1;  // semaphore equals the number of buffer
sem_t e; // keep track of number of empty space

int main(int argc, char *argv[]) {
  sem_init(&s,0,1);   // initialize the semaphore 1
  sem_init(&n1,0,0);  // initialize the semaphore 0 that start first element of buffer so it is 0.
  sem_init(&e,0,10);  // initialize the semaphore 10 that is size of buffer

  char *fileNameIn = (char *)argv[1];
  char *fileNameOut = "output.txt";
  pthread_t thread1;
  pthread_t thread2;
  start_timing(); // start timing to measure the time of all program
  pthread_create( &thread1, NULL, readFile, fileNameIn); // create a thread running readFile
  pthread_create( &thread2, NULL, readAndWrite, fileNameOut); // create athread running readAndWrite
  pthread_join(thread1,NULL) ;
  pthread_join(thread2,NULL) ;

  return 0;
}

void *readFile(void *fileName){
  char *name = (char*)fileName;
  string line = "";
  ifstream file;
  file.open(name);
  if(file == NULL){
    printf("Unable to open that file\n");

    return 0;
  }
  else{
    while (!file.eof()){ //  go to end of file
      while((in+1)%n == out){
      }
      getline(file,line);   // get line from inout file
      buffer[in] = line;    // assign this line to the buffer
	  getline(file,line);   // get line from inout file
	  buffer[in] = line;    // assign this line to the buffer
	  sem_wait(&e);   // to control size of buffer, lock it
	  sem_wait(&s);     // lock semaphore
	  in = (in+1) %n;   /* critical section */ // increment in to point next element in buffer
	  sem_post(&s);   // unlock semaphore
	  sem_post(&n1);
	}
	buffer[in] ="#";  // add # as a special character
	sem_wait(&e);
	sem_wait(&s); // this is for last line, lock thread
	in = (in+1) %n;  // critial section, increment in to point next element in buffer
	sem_post(&s); // unlock thread
	sem_post(&n1);
	file.close();
   }
    pthread_exit(NULL);
	  
	return 0;
}

void *readAndWrite(void *fileName){
  char *name = (char*)fileName;
  ofstream myfile (name);
  if (myfile.is_open()){
    while(1){  // infinite loop it is going to end when found the special character in buffer.
      while(in==out){}
      if(buffer[out] == "#"){  // search for special character
        myfile << buffer[out]<<endl;
        sem_wait(&n1);  // lock the semaphore that for checking buffer
        sem_wait(&s);   // lock the semaphore
        out = (out+1) %n; /* critical section */ // increment out to point next element in buffer
        sem_post(&s);   // unlock the semaphore
        sem_post(&e);
        stop_timing();  // program end. stop timing and calculate time.
        printf("the time from 'ftime' is %10.3f seconds \n ",get_wall_clock_diff());   // print calculated time
        printf("the time from 'getCPUtime' is %10.3f seconds\n",get_CPU_time_diff());  // print calculated cpu time

        return 0;
      }
      if(!buffer[out].empty()){
        myfile << buffer[out]<<endl;
      }
      sem_wait(&n1);  // lock the semaphore that for checking buffer
      sem_wait(&s);   // lock the semaphore
      out = (out+1) %n; /* critical section */ // increment out to point next element in buffer
      sem_post(&s);   // unlock the semaphore
      sem_post(&e);
    }
    myfile.close();
   }
   else
     return 0;
   pthread_exit(NULL);

   return 0;
}
