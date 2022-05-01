//Rahman ve Rahim Olan Allah'ın adıyla

/**
 * @file main.c
 * @author Yunus Emre Yiğit (yigity19@itu.edu.tr)
 * @brief 
 * @Student_Number 150190107
 * @date 2022-05-01
 * 
 * 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

sem_t full, empty, sema;


const int nr = 1;
int APPLICANT_NUM = 0;
int registered_count = 0;

struct applicant{
    int num;
    int ni;
    bool interviewed;
    bool registered;
};

struct applicant buffer[4];

struct interviewer{
    char num;
    struct applicant* applicant_ptr;
};

int* readFile(){
    FILE* fptr;
    char ch;
    fptr = fopen("input.txt","r");
    if(fptr == NULL){
        printf("file could not be opened");
        return NULL;
    }

    char buf[8];
    int i = 0;
    int counter = 0;
    while(fscanf(fptr, "%s", buf) == 1){
        APPLICANT_NUM++;
        counter++;
        i++;
    }

    fptr = fopen("input.txt","r");
    if(fptr == NULL){
        printf("file could not be opened");
        return NULL;
    }
    const int deneme = counter;
    int* array = (int *) malloc((deneme +1) * sizeof(int));
    i = 0;
    while(fscanf(fptr, "%s", buf) == 1){
        array[i] = atoi(buf);
        i++;
    }

    array[counter] = -1;
    fclose(fptr);
    return array;
}

void* registerApplicant(void* ptr){
    //struct interviewer* interviewerPtr = ptr;
    struct applicant* head = ptr;

    for(int i = 0; i < APPLICANT_NUM; i++){
        //if(head[i].registered == false && head[i].interviewed == false){
            printf("register empty\n");
            sem_wait(&empty);
            printf("register sema\n");  
            sem_wait(&sema);
            sleep(1);
            head[i].registered = true;
            buffer[i] = head[i];
            buffer[i].registered = true; 
            registered_count++;
            printf("**Applicant %d's registration is done**\n", head[i].num);
            sem_post(&sema);
            sem_post(&full);
            sem_post(&full);
            sem_post(&full);
            sem_post(&full);
            
        //}
    }
    pthread_exit(NULL);
}

void* interviewApplicant(void* ptr){
    struct interviewer* interviewerPtr = ptr;
    struct applicant* head = interviewerPtr->applicant_ptr;

    for(int i = 0; i < APPLICANT_NUM; i++){
            printf("interview full\n");
            sem_wait(&full);
            printf("interview sema\n");
            sem_wait(&sema);
          

            for(int j = 0; j < APPLICANT_NUM; j++ ){
                printf("+++ applicant %d situation %d, reg: %d\n",head[j].num, head[j].interviewed, head[j].registered);
            }
            
            printf("Interviewer %d started interview with Applicant %d\n", interviewerPtr->num, head[i].num);
            sleep(2);
            head[i].interviewed = true;
            
            printf("Interviewer %d finished interview with Applicant %d\n", interviewerPtr->num, head[i].num);
            
            for(int j = 0; j < APPLICANT_NUM; j++ ){
                printf("--- applicant %d situation %d, reg: %d\n",head[j].num, head[j].interviewed, head[j].registered);
            }

            printf("\n");
            
            sem_post(&sema);
            sem_post(&empty);
        
    }
    pthread_exit(NULL);

}

int main(){

    int* array = readFile();

    struct applicant applicants[APPLICANT_NUM];
    struct interviewer interviewers[3];

    printf("apllicant count %d-------\n", APPLICANT_NUM);
    sem_init(&empty, 0, APPLICANT_NUM);
    sem_init(&full,  0, 0);
    sem_init(&sema,  0, 1);


    for(int i = 0; i < APPLICANT_NUM; i++){
        applicants[i].num = i + 1;
        applicants[i].ni = array[i];
        applicants[i].interviewed = false;
        applicants[i].registered = false;
    }

    
    interviewers[0].applicant_ptr = applicants;
    interviewers[0].num = 1;
    interviewers[1].applicant_ptr = applicants;
    interviewers[1].num = 2;
    interviewers[2].applicant_ptr = applicants;
    interviewers[2].num = 3;


    pthread_t recept, inter1, inter2, inter3;


    if(pthread_create(&recept, NULL, registerApplicant, (void *) applicants)){
        printf("Could not create  thread");
        exit(-1);
    }

    if(pthread_create(&inter1, NULL, interviewApplicant, (void *) &interviewers[0])){
        printf("Could not create  thread");
        exit(-1);
    }

    if(pthread_create(&inter2, NULL, interviewApplicant, (void *) &interviewers[1])){
        printf("Could not create  thread");
        exit(-1);
    }

    if(pthread_create(&inter3, NULL, interviewApplicant, (void *) &interviewers[2])){
        printf("Could not create  thread");
        exit(-1);
    }
    
    //pthread_join(recept, NULL);
    //pthread_join(inter1, NULL);
    //pthread_join(inter2, NULL);
    //pthread_join(inter3, NULL);

    pthread_exit(NULL);
    //sem_destroy(&sema);
   return 0;
}