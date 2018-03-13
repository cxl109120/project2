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
#include <queue>

using namespace std;

#define num_receptionist 1
#define num_patient 3



sem_t sem_receptionist;
sem_t sem_doctor;
sem_t sem_patient;
sem_t sem_register;
sem_t sem_sit;
sem_t mutex1, mutex2;
queue <int> register_line;
int count;

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

void patient_register()
{
    int patient_num = patient_line.front();
    patient_line.pop();
    cout << "Receptionist register patient " << patient_num << endl;
    sleep(1);
}






// define threads
void* patient_thread(void* num)
{
    int patient_num = *(int*) num;
    
    sem_wait(&mutex1);
    enter_clinic(patient_num);
    register_line.push(patient_num);
    sem_post(&mutex1);
    
    sem_wait(&sem_receptionist);
    sem_post(&sem_register);

    sem_wait(&sem_sit);
    sit_waitingroom(patient_num);
    
    sem_post(&sem_receptionist);
    
}


void* receptionist_thread(void* num)
{
    while (true)
    {
        sem_wait(&sem_register);
        patient_register();
        //cout << "Receptionist register patient " << endl;
        sem_post(&sem_sit);
    }
}


int main(int argc, char* argv[])
{
    count = 0;
    
    // initialize thread
    pthread_t receptionist;
    pthread_t patient[num_patient];
    
    // initialize semaphores
    sem_init(&sem_receptionist, 0, num_receptionist);
    sem_init(&sem_patient, 0, num_patient);
    
    sem_init(&sem_register, 0, 0);
    /*
    sem_init(&sem_register[0], 0, 0);
    sem_init(&sem_register[1], 0, 0);
    sem_init(&sem_register[2], 0, 0);
    */
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









