//
//  project2.cpp
//  
//
//  Created by CHANGSONG LI on 3/12/18.
//

//#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define num_receptionist = 1;
#define num_patient = 3;



int count = 0;



sem_t semaphore;

void* thread(void* arg)
{
    //wait
    sem_wait(&semaphore);
    printf("\nEntered..\n");
    
    //critical section
    sleep(2);
    
    //signal
    printf("\nJust Exiting...\n");
    sem_post(&semaphore);
}


int main()
{
    sem_init(&semaphore, 0, 1);
    pthread_t t1,t2;
    
    pthread_create(&t1,NULL,thread,NULL);
    sleep(1);
    
    pthread_create(&t2,NULL,thread,NULL);
    
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    sem_destroy(&semaphore);
    return 0;
}









