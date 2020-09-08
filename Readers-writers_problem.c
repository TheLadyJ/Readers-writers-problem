#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//****** Lightswitch struktura ******
typedef struct {
    int counter;
    pthread_mutex_t mutex;
}Lightswitch;

void lightswitch_init(Lightswitch *lsw){
    lsw->counter=0;
    pthread_mutex_init(&lsw->mutex,NULL);
}

void lightswitch_lock(Lightswitch *lsw, sem_t *semaphore){
    pthread_mutex_lock(&lsw->mutex);
    lsw->counter++;
    if(lsw->counter==1)
        sem_wait(semaphore);
    pthread_mutex_unlock(&lsw->mutex);
}

void lightswitch_unlock(Lightswitch *lsw, sem_t *semaphore){
    pthread_mutex_lock(&lsw->mutex);
    lsw->counter--;
    if(lsw->counter==0)
        sem_post(semaphore);
    pthread_mutex_unlock(&lsw->mutex);
}
//**********************************

//Korisæene promenljive
Lightswitch readSwitch;
sem_t roomEmpty;
sem_t turnstile;
int sharedVariable;


//Nit pisca
void *writer_thread(void *serialNum){
    int randomTime = rand() %6;

    sem_wait(&turnstile);
    sem_wait(&roomEmpty);

    sleep(randomTime);
    
    //Kritièan deo za pisce
    printf("\nWriter %d - New value of shared variable is:", *((int*)serialNum));
    scanf("%d",&sharedVariable);
    
    sem_post(&turnstile);
    sem_post(&roomEmpty);
}

//Nit èitaoca
void*reader_therad(void *serialNum){
    int randomTime = rand() %3;
    
    sem_wait(&turnstile);
    sem_post(&turnstile);
    
    sleep(randomTime);
    
    lightswitch_lock(&readSwitch,&roomEmpty);
    
    //Kritièan deo za èitaoce
    printf("\nReader %d - Read value is %d\n",*((int*)serialNum),sharedVariable);
    
    lightswitch_unlock(&readSwitch,&roomEmpty);
}




int main()
{
    lightswitch_init(&readSwitch);
    sem_init(&roomEmpty,0,1);
    sem_init(&turnstile,0,1);
    
    pthread_t read[5],write[5];
    int a[5] = {1,2,3,4,5};
    
    for(int i = 0; i < 5; i++) {
        pthread_create(&read[i], NULL, (void *)reader_therad, (void *)&a[i]);
    }
    for(int i = 0; i < 5; i++) {
        pthread_create(&write[i], NULL, (void *)writer_thread, (void *)&a[i]);
    }
    
    for(int i = 0; i < 5; i++) {
        pthread_join(write[i], NULL);
    }
    for(int i = 0; i < 5; i++) {
        pthread_join(read[i], NULL);
    }

    return 0;
}


