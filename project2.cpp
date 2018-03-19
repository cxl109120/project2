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
#include <vector>
#include <queue>

using namespace std;

//#define num_patient 4
//#define num_doctor 3



int count;
int num_patient;
int num_doctor;
queue <int> reception_line;
queue <int> doctor_line;
//int patient_doctor[num_patient] = {-1};
//int doctor_patient[num_doctor] = {-1};
vector <int> patient_doctor(num_patient, -1);
vector <int> doctor_patient(num_doctor, -1);


// declare semaphores
sem_t sem_receptionist;
sem_t sem_doctor;
sem_t sem_nurse;

sem_t sem_receptionist_register;
sem_t sem_patient_sit;
sem_t sem_take_office;

/*
sem_t sem_assignment[num_patient];

sem_t sem_doctor_ready[num_doctor];
sem_t sem_enter_office[num_doctor];
sem_t sem_patient_ready[num_doctor];
sem_t sem_listen_symptom[num_doctor];
sem_t sem_receive_advice[num_doctor];
*/

vector <sem_t> sem_assignment(num_patient);

vector <sem_t> sem_doctor_ready(num_doctor);
vector <sem_t> sem_enter_office(num_doctor);
vector <sem_t> sem_patient_ready(num_doctor);
vector <sem_t> sem_listen_symptom(num_doctor);
vector <sem_t> sem_receive_advice(num_doctor);


sem_t mutex;




// define functions
void patient_enter_clinic(int num)
{
    sem_wait(&mutex);
    cout << "Patient " << num
    << " enters waiting room, waits for receptionist" << endl;
    sleep(0.5);
    sem_post(&mutex);
}

void patient_sit(int num)
{
    sem_wait(&mutex);
    cout << "Patient " << num
    << " leaves receptionist and sits in waiting room" << endl;
    sleep(0.5);
    sem_post(&mutex);
}

void receptionist_register()
{
    sem_wait(&mutex);
    int patient_num = reception_line.front();
    // dequeue reception_line
    reception_line.pop();
    cout << "Receptionist register patient " << patient_num << endl;
    sleep(0.5);
    sem_post(&mutex);
}

int nurse_take_office(int num)
{
    sem_wait(&mutex);
    int patient_num = doctor_line.front();
    // dequeue doctor_line
    doctor_line.pop();
    cout << "Nurse " << num
    << " takes patient " << patient_num << " to doctor's office" << endl;
    
    patient_doctor[patient_num] = num;
    doctor_patient[num] = patient_num;
    sleep(0.5);
    sem_post(&mutex);
    return patient_num;
}

void patient_enter_office(int num)
{
    sem_wait(&mutex);
    int doctor_num = patient_doctor[num];
    cout << "Patient " << num
    << " enters doctor "<< doctor_num << "'s office" << endl;
    sleep(0.5);
    sem_post(&mutex);
}

void doctor_listen(int num)
{
    sem_wait(&mutex);
    int patient_num = doctor_patient[num];
    cout << "Doctor " << num
    << " listens to symptoms from patient " << patient_num << endl;
    sleep(0.5);
    sem_post(&mutex);
}

void patient_receive(int num)
{
    sem_wait(&mutex);
    int doctor_num = patient_doctor[num];
    cout << "Patient " << num
    << " receives advice from doctor " << doctor_num << endl;
    sleep(0.5);
    sem_post(&mutex);
}

void patient_leave(int num)
{
    sem_wait(&mutex);
    cout << "Patient " << num
    << " leave" << endl;
    sleep(0.5);
    sem_post(&mutex);
}



// define threads
void* patient_thread(void* arg)
{
    int patient_num;
    int doctor_num;
    
    sem_wait(&mutex);
    patient_num = count;
    count++;
    sem_post(&mutex);
    
    patient_enter_clinic(patient_num);
    // enqueue reception_line
    reception_line.push(patient_num);
    sem_wait(&sem_receptionist);
    sem_post(&sem_receptionist_register);
    sem_wait(&sem_patient_sit);
    //sem_post(&sem_receptionist);
    patient_sit(patient_num);
    
    // enqueue doctor_line
    doctor_line.push(patient_num);
    sem_wait(&sem_nurse);
    sem_post(&sem_take_office);
    
    // assign patients with doctors
    sem_wait(&(sem_assignment[patient_num]));
    doctor_num = patient_doctor[patient_num];
    
    sem_wait(&(sem_enter_office[doctor_num]));
    patient_enter_office(patient_num);
    sem_post(&(sem_patient_ready[doctor_num]));
    sem_wait(&(sem_listen_symptom[doctor_num]));
    patient_receive(patient_num);
    sem_post(&(sem_receive_advice[doctor_num]));
    
    patient_leave(patient_num);
}

void* receptionist_thread(void* arg)
{
    while (true)
    {
        sem_wait(&sem_receptionist_register);
        receptionist_register(); // dequeue reception_line
        sem_post(&sem_patient_sit);
        sem_post(&sem_receptionist);
    }
}

void* nurse_thread(void* num)
{
    int nurse_num = *(int*) num;
    int patient_num;
    
    while (true)
    {
        sem_wait(&sem_take_office);
        sem_wait(&(sem_doctor_ready[nurse_num]));
        patient_num = nurse_take_office(nurse_num); // dequeue doctor_line
        sem_post(&(sem_assignment[patient_num]));
        sem_post(&(sem_enter_office[nurse_num]));
        sem_post(&sem_nurse);
    }
}

void* doctor_thread(void* num)
{
    int doctor_num = *(int*) num;
    while (true)
    {
        sem_wait(&sem_patient_ready[doctor_num]);
        doctor_listen(doctor_num);
        sem_post(&(sem_listen_symptom[doctor_num]));
        sem_wait(&(sem_receive_advice[doctor_num]));
        sem_post(&(sem_doctor_ready[doctor_num]));
    }
}




// main function starts here----------------------------------------------------------
int main(int argc, char* argv[])
{
    count = 0;
    
    if (argc == 3)
    {
        num_patient = atoi(argv[1]);
        num_doctor = atoi(argv[2]);
    }
    else
    {
        cout << "Type in number of patients: ";
        cin >> num_patient;
        cout << "Type in number of doctors: ";
        cin >> num_doctor;
    }

    //patient_doctor[num_patient] = {-1};
    //doctor_patient[num_doctor] = {-1};
    
    cout << "Simulation with " << num_patient << " patients and "
    << num_doctor << " doctors..." << endl << endl;

    // initialize thread
    pthread_t receptionist;
    pthread_t patient[num_patient];
    pthread_t doctor[num_doctor];
    pthread_t nurse[num_doctor];
    
    // initialize semaphores
    sem_init(&sem_receptionist, 0, 1);
    sem_init(&sem_doctor, 0, num_doctor);
    sem_init(&sem_nurse, 0, num_doctor);
    sem_init(&sem_receptionist_register, 0, 0);
    sem_init(&sem_patient_sit, 0, 0);
    sem_init(&sem_take_office, 0, 0);

    
    for(int i = 0; i < num_patient; i++)
    {
        sem_init(&(sem_assignment[i]), 0, 0);
    }
    
    for(int i = 0; i < num_doctor; i++)
    {
        sem_init(&(sem_doctor_ready[i]), 0, 1);
        sem_init(&(sem_enter_office[i]), 0, 0);
        sem_init(&(sem_patient_ready[i]), 0, 0);
        sem_init(&(sem_listen_symptom[i]), 0, 0);
        sem_init(&(sem_receive_advice[i]), 0, 0);
    }
    
    
    /*
    for(int i = 0; i < sem_assignment.size(); i++)
        sem_init(&(sem_assignment[i]), 0, 0);
    
    for(int i = 0; i < sem_doctor_ready.size(); i++)
        sem_init(&(sem_doctor_ready[i]), 0, 1);
    for(int i = 0; i < sem_enter_office.size(); i++)
        sem_init(&(sem_enter_office[i]), 0, 0);
    for(int i = 0; i < sem_patient_ready.size(); i++)
        sem_init(&(sem_patient_ready[i]), 0, 0);
    for(int i = 0; i < sem_listen_symptom.size(); i++)
        sem_init(&(sem_listen_symptom[i]), 0, 0);
    for(int i = 0; i < sem_receive_advice.size(); i++)
        sem_init(&(sem_receive_advice[i]), 0, 0);
    */
    
    // initialize mutex
    sem_init(&mutex, 0, 1);
    
    
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
    for (int i = 0; i < num_doctor; i++)
    {
        sem_wait(&mutex);
        doctor_num = (int*)malloc(sizeof(int));
        *doctor_num = i;
        pthread_create(&doctor[i], NULL, doctor_thread, doctor_num);
        pthread_create(&nurse[i], NULL, nurse_thread, doctor_num);
        sem_post(&mutex);
    }
    

    for (int i = 0; i < num_patient; i++)
    {
        pthread_join(patient[i], NULL);
    }

    return 0;
}









