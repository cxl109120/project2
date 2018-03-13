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



//int patient[3] = {0, 1, 2};


sem_t sem_receptionist;
sem_t sem_doctor;
sem_t sem_patient;
sem_t mutex1, mutex2;
int count = 0;



void* patient_thread(void* num)
{
    //wait
    sem_wait(&sem_receptionist);
    cout << *(int*)num << " entering..." << endl;
    
    //critical section
    //sleep(1);
    
    //signal
    cout << *(int*)num << " exiting..." << endl;
    sem_post(&sem_receptionist);
}

void* receptionist_thread(void* num)
{
    while (true)
    {
        sem_wait(&sem_patient);
        cout << "register patient..." << endl;
        
    }
}


int main(int argc, char* argv[])
{
    // initialize receptionist
    sem_init(&sem_receptionist, 0, 1);
    pthread_t receptionist;
    // initialize patient
    sem_init(&sem_patient, 0, 3);
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
    
    //pthread_create(&receptionist, NULL, receptionist_thread, NULL);
    
    
    
    for (int i = 0; i < 3; i++)
    {
        pthread_join(patient[i], NULL);
    }
    //pthread_join(t1, NULL);
    //pthread_join(t2, NULL);
    sem_destroy(&sem_receptionist);
    return 0;
}









