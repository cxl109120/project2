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

#define num_receptionist 1
#define num_patient 3



//int patient[3] = {0, 1, 2};


sem_t sem_receptionist;
sem_t sem_doctor;
sem_t sem_patient;
sem_t sem_register[3];
sem_t sem_sit;
sem_t mutex1, mutex2;
int count = 0;

// define functions
void enter_clinic(int num)
{
    cout << "Patient " << num
    << " enters waiting room, waits for receptionist" << endl;
    sleep(1);
}

void sit_waitingroom(int num)
{
    cout << "Patient " << num
    << " leaves receptionist and sits in waiting room" << endl;
    sleep(1);
}






// define threads
void* patient_thread(void* num)
{
    int patientnr = *(int*) num;
    
    sem_wait(&mutex1);
    enter_clinic(patientnr);
    sem_post(&mutex1);
    
    sem_wait(&sem_receptionist);
    sem_post(&sem_register[patientnr]);

    sem_wait(&sem_sit);
    sit_waitingroom(patientnr);
    
    sem_post(&sem_receptionist);
    
}


void* receptionist_thread(void* num)
{
    while (true)
    {
        int patientnr = *(int*) num;
        
        sem_wait(&sem_register[patientnr]);
        cout << "Receptionist register patient " << patientnr << endl;
        sem_post(&sem_sit);
    }
}


int main(int argc, char* argv[])
{
    // initialize thread
    pthread_t receptionist;
    pthread_t patient[num_patient];
    
    // initialize semaphores
    sem_init(&sem_receptionist, 0, num_receptionist);
    sem_init(&sem_patient, 0, num_patient);
    
    //sem_init(&sem_register, 0, 0);
    
    sem_init(&sem_register[0], 0, 0);
    sem_init(&sem_register[1], 0, 0);
    sem_init(&sem_register[2], 0, 0);
    
    // initialize mutex
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    
    
    int *patient_num;
    

    
    for (int i = 0; i < num_patient; i++)
    {
        patient_num = (int*)malloc(sizeof(int));
        *patient_num = i;
        pthread_create(&patient[i], NULL, patient_thread, patient_num);
    }
    
    pthread_create(&receptionist, NULL, receptionist_thread, NULL);
    
    
    
    for (int i = 0; i < num_patient; i++)
    {
        pthread_join(patient[i], NULL);
    }

    //sem_destroy(&sem_receptionist);

    return 0;
}









