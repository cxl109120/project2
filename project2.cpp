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
#define num_doctor 1
#define num_nurse 1



sem_t sem_receptionist;
sem_t sem_doctor;
sem_t sem_nurse;
sem_t sem_patient;
sem_t sem_register;
sem_t sem_sit;
sem_t mutex1, mutex2;

queue <int> reception_line;
int count;

// define functions
void enter_clinic(int num)
{
    sem_wait(&mutex1);
    cout << "Patient " << num
    << " enters waiting room, waits for receptionist" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void sit_waitingroom(int num)
{
    sem_wait(&mutex1);
    cout << "Patient " << num
    << " leaves receptionist and sits in waiting room" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void patient_register()
{
    sem_wait(&mutex1);
    int patient_num = reception_line.front();
    reception_line.pop();
    cout << "Receptionist register patient " << patient_num << endl;
    sleep(0.5);
    sem_post(&mutex1);
}






// define threads
void* patient_thread(void* num)
{
    int patient_num;
    //int patient_num = *(int*) num;
    sem_wait(&mutex1);
    patient_num = count;
    count++;
    sem_post(&mutex1);
    
    enter_clinic(patient_num);
    // enqueue
    reception_line.push(patient_num);
    
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
        sem_post(&sem_sit);
    }
}

void* nurse_thread(void* arg)
{
    
}


int main(int argc, char* argv[])
{
    count = 0;
    
    // initialize thread
    pthread_t receptionist;
    pthread_t patient[num_patient];
    pthread_t doctor[num_doctor];
    pthread_t nurse[num_doctor];
    
    // initialize semaphores
    sem_init(&sem_receptionist, 0, num_receptionist);
    sem_init(&sem_patient, 0, num_patient);
    sem_init(&sem_doctor, 0, num_doctor);
    sem_init(&sem_nurse, 0, num_nurse);
    
    sem_init(&sem_register, 0, 0);
    sem_init(&sem_sit, 0, 0);
    /*
    sem_init(&sem_register[0], 0, 0);
    sem_init(&sem_register[1], 0, 0);
    sem_init(&sem_register[2], 0, 0);
    */
    // initialize mutex
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    
    
    //int *patient_num;

    for (int i = 0; i < num_patient; i++)
    {
        //patient_num = (int*)malloc(sizeof(int));
        //*patient_num = i;
        //pthread_create(&patient[i], NULL, patient_thread, patient_num);
        pthread_create(&patient[i], NULL, patient_thread, NULL);
    }
    
    pthread_create(&receptionist, NULL, receptionist_thread, NULL);
    
    
    for (int i = 0; i < num_doctor; i++)
    {
        pthread_create(&doctor[i], NULL, doctor_thread, NULL);
        pthread_create(&nurse[i], NULL, nurse_thread, NULL);
    }
    
    
    
    
    
    
    
    for (int i = 0; i < num_patient; i++)
    {
        pthread_join(patient[i], NULL);
    }

    //sem_destroy(&sem_receptionist);

    return 0;
}









