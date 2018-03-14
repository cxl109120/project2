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
#define num_patient 4
#define num_doctor 3



int count;
queue <int> reception_line;
queue <int> doctor_line;
int patient_doctor[num_patient] = {-1};
int doctor_patient[num_doctor] = {-1};

// declare semaphores
sem_t sem_receptionist;
sem_t sem_patient;
sem_t sem_doctor;
sem_t sem_nurse;

sem_t sem_register;
sem_t sem_sit;
sem_t sem_take_office;
sem_t sem_enter_office;
sem_t sem_patient_ready;
sem_t sem_listen_symptom;
sem_t sem_receive_advice;

sem_t sem_doctor_ready[num_doctor];
sem_t mutex1, mutex2;




// define functions
void patient_enter_clinic(int num)
{
    sem_wait(&mutex1);
    cout << "Patient " << num
    << " enters waiting room, waits for receptionist" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void patient_sit(int num)
{
    sem_wait(&mutex1);
    cout << "Patient " << num
    << " leaves receptionist and sits in waiting room" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void receptionist_register()
{
    sem_wait(&mutex1);
    int patient_num = reception_line.front();
    // dequeue reception_line
    reception_line.pop();
    cout << "Receptionist register patient " << patient_num << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void nurse_take_office(int num)
{
    sem_wait(&mutex1);
    int patient_num = doctor_line.front();
    // dequeue doctor_line
    doctor_line.pop();
    cout << "Nurse " << num
    << " takes patient " << patient_num << " to doctor's office" << endl;
    
    patient_doctor[patient_num] = num;
    doctor_patient[num] = patient_num;
    sleep(0.5);
    sem_post(&mutex1);
}

void patient_enter_office(int num)
{
    sem_wait(&mutex1);
    int doctor_num = patient_doctor[num];
    cout << "Patient " << num
    << " enters doctor "<< doctor_num << "'s office" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void doctor_listen(int num)
{
    sem_wait(&mutex1);
    int patient_num = doctor_patient[num];
    cout << "Doctor " << num
    << " listens to symptoms from patient " << patient_num << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void patient_receive(int num)
{
    sem_wait(&mutex1);
    int doctor_num = patient_doctor[num];
    cout << "Patient " << num
    << " receives advice from doctor " << doctor_num << endl;
    sleep(0.5);
    sem_post(&mutex1);
}

void patient_leave(int num)
{
    sem_wait(&mutex1);
    cout << "Patient " << num
    << " leave" << endl;
    sleep(0.5);
    sem_post(&mutex1);
}



// define threads
void* patient_thread(void* arg)
{
    int patient_num;
    //int patient_num = *(int*) num;
    sem_wait(&mutex1);
    patient_num = count;
    count++;
    sem_post(&mutex1);
    
    patient_enter_clinic(patient_num);
    // enqueue reception_line
    reception_line.push(patient_num);
    sem_wait(&sem_receptionist);
    sem_post(&sem_register);
    sem_wait(&sem_sit);
    sem_post(&sem_receptionist);
    patient_sit(patient_num);
    
    // enqueue doctor_line
    doctor_line.push(patient_num);
    sem_post(&sem_take_office);
    sem_wait(&sem_enter_office);
    patient_enter_office(patient_num);
    sem_post(&sem_patient_ready);
    sem_wait(&sem_listen_symptom);
    patient_receive(patient_num);
    sem_post(&sem_receive_advice);
    
    patient_leave(patient_num);
}

void* receptionist_thread(void* arg)
{
    while (true)
    {
        sem_wait(&sem_register);
        receptionist_register(); // dequeue reception_line
        sem_post(&sem_sit);
        //sem_post(&sem_take_office);
    }
}

void* nurse_thread(void* num)
{
    int nurse_num = *(int*) num;
    while (true)
    {
        sem_wait(&sem_take_office);
        nurse_take_office(nurse_num); // dequeue doctor_line
        sem_post(&sem_enter_office);
        //sem_post(&sem_patient_ready);
    }
}

void* doctor_thread(void* num)
{
    int doctor_num = *(int*) num;
    while (true)
    {
        sem_wait(&sem_patient_ready);
        doctor_listen(doctor_num);
        sem_post(&sem_listen_symptom);
        sem_wait(&sem_receive_advice);
        //sem_post(
    }
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
    sem_init(&sem_nurse, 0, num_doctor);
    sem_init(&sem_register, 0, 0);
    sem_init(&sem_sit, 0, 0);
    sem_init(&sem_take_office, 0, 0);
    sem_init(&sem_enter_office, 0, 0);
    sem_init(&sem_patient_ready, 0, 0);
    sem_init(&sem_listen_symptom, 0, 0);
    sem_init(&sem_receive_advice, 0, 0);
    
    for(int i = 0; i < num_doctor; i++)
    {
        sem_init(&sem_doctor[i], 0, 0);
    }
    
    
    // initialize mutex
    sem_init(&mutex1, 0, 1);
    //sem_init(&mutex2, 0, 1);
    
    
    //int *patient_num;

    // patient thread
    for (int i = 0; i < num_patient; i++)
    {
        //patient_num = (int*)malloc(sizeof(int));
        //*patient_num = i;
        //pthread_create(&patient[i], NULL, patient_thread, patient_num);
        pthread_create(&patient[i], NULL, patient_thread, NULL);
    }
    
    // receptionist thread
    pthread_create(&receptionist, NULL, receptionist_thread, NULL);
    
    
    // doctor and nurse thread
    int *doctor_num;
    //int doctor_num;
    for (int i = 0; i < num_doctor; i++)
    {
        sem_wait(&mutex1);
        doctor_num = (int*)malloc(sizeof(int));
        *doctor_num = i;
        pthread_create(&doctor[i], NULL, doctor_thread, doctor_num);
        pthread_create(&nurse[i], NULL, nurse_thread, doctor_num);
        sem_post(&mutex1);
    }
    
    
    
    
    
    
    
    for (int i = 0; i < num_patient; i++)
    {
        pthread_join(patient[i], NULL);
    }

    //sem_destroy(&sem_receptionist);

    return 0;
}









