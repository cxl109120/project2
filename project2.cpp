//
//  project2.cpp
//  
//
//  Created by CHANGSONG LI on 3/12/18.
//

//#include <sys/types.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

#define num_receptionist = 1;
#define num_patient = 3;



int count = 0;
//int patient[3] = {0, 1, 2};


sem_t semaphore;

void* patient_thread(void* num)
{
    //wait
    sem_wait(&semaphore);
    cout << *(int*)num << " entering..." << endl;
    //printf("%d Entered...\n", *(int*)num);
    
    //critical section
    sleep(1);
    
    //signal
    cout << *(int*)num << " exiting..." << endl;
    //printf("%d Just Exiting...\n", *(int*)num);
    sem_post(&semaphore);
}


int main(int argc, char* argv[])
{
    sem_init(&semaphore, 0, 1);
    pthread_t patient[3];
    int *patient_num;
    
    
    for (int i = 0; i < 3; i++)
    {
        patient_num = (int*)malloc(sizeof(int));
        *patient_num = i;
        pthread_create(&patient[i], NULL, patient_thread, patient_num);
    }
    //pthread_create(&t1, NULL, thread, &patient[0]);
    //pthread_create(&t2, NULL, thread, &patient[1]);
    
    
    for (int i = 0; i < 3; i++)
    {
        pthread_join(patient[i], NULL);
    }
    //pthread_join(t1, NULL);
    //pthread_join(t2, NULL);
    sem_destroy(&semaphore);
    return 0;
}









