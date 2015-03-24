#include <iostream>
#include <sys/timeb.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <fstream>
#include <string>

#include "time_functions.h"

using namespace std;
int readcount, writecount = 0;

void *READER(void *);
void *WRITER(void *);

int milisecond;  // holds milisecond
int second;		  // holds second
pthread_mutex_t monitor;  // to control incrementing and decrementing reader and writer counter
pthread_mutex_t read_write;  // to control to read and write the second and millisecond
pthread_cond_t writer, reader;  // to add and pop from/to queue 
int r; int w; int R; int W;  
// # reader //# writer // delay Reader // delay Writer
int active_reader=0, active_writer=0, waiting_reader=0, waiting_writer=0;
int main(int argc, char *argv[]) {
	if(argc != 5){ // if user didn't enter any input from command
    	cout << "Not argument detected from command, please input" << endl;
        cout << "Number of Readers "; cin >> r; cout <<r;
        cout << "Number of Writers ";   cin >> w; cout << w;
        cout << "Delay for a Reader "; cin >> R; cout << R;
        cout << "Delay for a Writer "; cin >> W; cout << W;
   	}
	else{// assign variable
    	r = atoi((char *)argv[1]);
        w = atoi((char *)argv[2]);
        R = atoi((char *)argv[3]);
        W = atoi((char *)argv[4]);
	}
	// initialize mutexes and condition varibales
	pthread_mutex_init(&monitor, NULL);
    pthread_mutex_init(&read_write, NULL);
    pthread_cond_init(&reader, NULL);
 	pthread_cond_init(&writer, NULL);
    pthread_t *reader_threads = new pthread_t[r]; // allocate for reader threads
    pthread_t *writer_threads = new pthread_t[w]; // allocate for writer threads

    uintptr_t x=0;
    for (int a=0; a<w; a++) {
    	pthread_create(&writer_threads[a], NULL, WRITER, (void *)x); // create thread and call WRITER function with # of thread
        x++;
    }
	uintptr_t y=0;
    for (int a=0; a<r; a++) {
    	pthread_create(&reader_threads[a], NULL, READER, (void *)y);// create thread and call READER function with # of thread
        y++;
    }
// join threads
	for (int a=0; a<w; a++) {
    	pthread_join(writer_threads[a], NULL);
    }
	for (int a=0; a<r; a++) {
    	pthread_join(reader_threads[a],NULL) ;
    }
}
void *READER(void *arg){
	intptr_t read_count = (intptr_t)arg;
	for (int b=0; b<10; b++) {
		pthread_mutex_lock(&monitor); // lock monitor
        while (active_writer + waiting_writer >0) {  // check is there any active or waiting thread
        	waiting_reader++;
        	pthread_cond_wait(&reader, &monitor);
        	waiting_reader--;
     	}
    	active_reader++;   // increment active reader to hold # of active reader
     	pthread_mutex_unlock(&monitor);  // unlock monitor

     	pthread_mutex_lock(&read_write); // lock for read data from DB
     	cout << ">>> DB value read =: " << second<< ":"<< milisecond<< " by reader number "<<  read_count<<endl; //access db
     	pthread_mutex_unlock(&read_write);

     	pthread_mutex_lock(&monitor);
     	active_reader--;  // performed so decrement active reader count
      	if(active_reader ==0 && waiting_writer>0){ // check is there any waiting writer thread in queue
      		pthread_cond_signal(&writer);    // if so, signal the wrter condition variable 
      	}
     	else {
       		pthread_cond_broadcast(&reader);  // if not, broadcast reader condition variable
     	}
       	pthread_mutex_unlock(&monitor);
     	 millisleep(R);  // delay for each reader thread
   	}
	pthread_exit(NULL);
    
	return 0;
}

void *WRITER(void *arg){
	intptr_t write_count = (intptr_t)arg;
 	for (int b=0; b<10; b++) {
     	pthread_mutex_lock(&monitor);
        while (active_reader + active_writer >0) {  // check for is there any waiting or active writer
        	waiting_writer++;
         	pthread_cond_wait(&writer, &monitor);  // if so add them to queue
          	waiting_writer--;
     	}
     	active_writer++;
     	pthread_mutex_unlock(&monitor);

     	pthread_mutex_lock(&read_write);  // lock to write new value to DB
     	ftime(&timestruct);
       	milisecond = timestruct.millitm;
    	second = timestruct.time;
      	cout << "*** DB value set to: "<< second<< ":"<< milisecond<< " by writer number "<< write_count<<endl; // access db
     	pthread_mutex_unlock(&read_write);

      	pthread_mutex_lock(&monitor);
      	active_writer--;
    	if(waiting_writer >0)  // check for is there any waiting thread
        	pthread_cond_signal(&writer); //  if so, perform the writer condition variable
        else if(active_writer == 0 && waiting_writer == 0) { // if not call perform reader
        	pthread_cond_signal(&reader);
       	}
       	pthread_mutex_unlock(&monitor);
        	millisleep(W); // delay for each writer thread
        }
	pthread_exit(NULL);
   	
	return 0;
}

