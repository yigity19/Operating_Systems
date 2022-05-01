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

int nr = 1;

sem_t full, empty, sema;

int APPLICANT_NUM = 0;

/**
 * @brief represents the applicants applying for jobs
 * ni is the number of seconds interview takes for different jobs 
 */
struct applicant{
    int num;
    int ni;
    bool interviewed;
    bool registered;
};

/**
 * @brief Interviewer struct
 *  has an id called num & a pointer to the array applicants struct in order to reach applicants
 */
struct interviewer{
    char num;
    struct applicant* applicant_ptr;
};

/**
 * @brief read input file
 * 
 * @param name file name to read
 * @return return pointer to array of int whose values come from file 
 */
int* readFile(char name[]){
    FILE* fptr;
    fptr = fopen(name,"r");
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

    // return file pointer to the head of the input file 
    // this is done in order to fill the array with numbers
    fptr = fopen(name,"r"); 
    if(fptr == NULL){
        printf("file could not be opened!\n");
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

/**
 * @brief Registers applicants. Acts like the Producer in producer-consumer problem
 * 
 * @param ptr pointer to the applicant list
 * @return void* 
 */
void* registerApplicant(void* ptr){
    
    // local pointer to the applicants list
    struct applicant* head = ptr;

    for(int i = 0; i < APPLICANT_NUM; i++){
        // if waiting room is not empty, wait until it becomes empty !!!!!!!!
        sem_wait(&empty); 
        // if another is in critical section, wait until it finishtes its job
        sem_wait(&sema);
        // registering takes nr seconds, wait until nr seconds passes
        sleep(nr);
        // mark the current applicant as registered
        head[i].registered = true;
        // unlock the critical section semaphore
        sem_post(&sema);
        // increment full semaphore, as there are at least one applicant in the waiting room
        sem_post(&full);
        printf("Applicant %d's registration is done\n", head[i].num);
    }
    pthread_exit(NULL);
}

// interview counter used to count total number of interviews that are done so far
int int_counter = 0;

/**
 * @brief Interview with an applicant whose is in the waiting room. Acts like a consumer  
 * 
 * @param ptr Pointer to the current interviewer (or consumer thread)
 * @return void* 
 */
void* interviewApplicant(void* ptr){
    // form a pointer to the calling interviewer
    struct interviewer* interviewerPtr = ptr;
    // form a pointer to the start of the array of applicants list
    struct applicant* head = interviewerPtr->applicant_ptr;

    
    int counter = 0;
    for(int i = 0; i < APPLICANT_NUM; i++){
           // wait until there is at least one applicant in the waiting room !!!!!!
            sem_wait(&full);
            // !!!!!!1
            sem_wait(&sema);
            counter=0;
            // find index of the applicant which is registered and has not been interviewed 
            for(int j = 0; j < APPLICANT_NUM; j++){
                if(head[j].registered && !head[j].interviewed){
                    break;
                }
                else
                    counter++;
            }

            // mark the found applicant as interviewed
            head[counter].interviewed = true;

            // increment semaphores
            sem_post(&sema);
            sem_post(&empty);

            // increment the total number of interviewed done so far
            int_counter++;

            printf("Interviewer %d started interview with Applicant %d\n", interviewerPtr->num, head[counter].num);
            sleep(head[counter].ni);
            printf("Interviewer %d finished interview with Applicant %d\n", interviewerPtr->num, head[counter].num);

            // if interviews done so far exceeds or equal to the number of all applicants end interviewing
            if(int_counter >= APPLICANT_NUM){
                free(head);
                printf("All applicants have interviewed successfully.\n");
                break;
            }
    }
    pthread_exit(NULL);

}

int main(int argc, char* argv[]){

    int* array = readFile(argv[1]);
    nr = atoi(argv[2]);
    struct applicant applicants[APPLICANT_NUM];
    struct interviewer interviewers[3];

    // initializing semaphores
    sem_init(&empty, 0, APPLICANT_NUM);
    sem_init(&full,  0, 0);
    sem_init(&sema,  0, 1);

    // mark all applicants as not registered and not interviewed
    // copy all ni values to applicants list
    // print all applicants as applied to receptionist
    for(int i = 0; i < APPLICANT_NUM; i++){
        applicants[i].num = i + 1;
        printf("Applicant %d applied to the receptionist\n", applicants[i].num);
        applicants[i].ni = array[i];
        applicants[i].interviewed = false;
        applicants[i].registered = false;
    }

    // initialize interviewer structs    
    interviewers[0].applicant_ptr = applicants;
    interviewers[0].num = 1;
    interviewers[1].applicant_ptr = applicants;
    interviewers[1].num = 2;
    interviewers[2].applicant_ptr = applicants;
    interviewers[2].num = 3;

    // declare thread objects
    pthread_t recept, inter1, inter2, inter3;

    // create threads
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
    
    pthread_exit(NULL);
   return 0;
}